/*
 * $Id: rate_skip.c,v 1.1 2005/02/23 14:33:40 tobiasb Exp $
 *
 * rate database (Tarifdatenbank) -- skipping of providers
 *
 * Copyright 2005 Tobias Becker <tobiasb@isdn4linux.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * Skipped providers:  The user may decide to completely ignore ("skip")
 * a subset of providers defined in the rate file.  This done by Q:<...>
 * lines in rate.conf and/or a skipProv=<...> line in the parameter file.
 * Skipping is either unconditional or takes place only if the provider
 * is not explicit mentioned by a P:... line in rate.conf.
 *
 * The internal list of skipped providers is kept simple as it will be
 * used only while reading the ratefile.  No care is taken about
 * overlapping entries and similar things.
 *
 * If the rate file uses redirs (R:...), disabling all non booked providers by
 * the * wildcard entry will probably cause error during rate file loading like
 * "rate-de.dat:@1938 Couldn't find provider 33_1 for redir #3 pnum 33_2 (7)".
 * Because of this, a second list of entries becomes necessary.  If a provider
 * is matched by this "white" list, it is not skipped, regardless of any
 * entries in the regular "black" list.
 *
 * Interface:
 *
 * int add_skipped_provider(char *line, char **msg);
 *   Parse line as list of skipped providers.  The format is explained in
 *   rate.conf(5).  The return value is 0 for proper input and 1 otherwise.
 *   If msg is not NULL, it will be used for an description in case of error.
 *   line is modified during parsing.
 *
 * int is_skipped_provider(int prov, int var, int booked);
 *   Returns 1 if the provider written as P:prov,var in the rate file is to
 *   be skipped or 0 otherwise.  var may be UNKNOWN (-1).  booked states
 *   whether there is a line P:prov=var in rate.conf or not.
 *
 * int clear_skipped_provider(void);
 *   This clears the internal list of skipped providers.  Returns the number
 *   of freed list entries.
 *
 * int dump_skipped_provider(char *str, int len);
 *   Write the internal list of skipped providers to str with a maxium of len
 *   characters.  The format of the output is the same as expected for input
 *   in add_skipped_provider.  Returns the number of list entries or -1 in
 *   case of error.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "rate_skip.h"

#ifndef UNKNOWN              /* tools.h is a little too heavy ... */
#define UNKNOWN (-1)
#endif

#define BLACK 0              /* indices for regular and */
#define WHITE 1              /* exceptional list */

#define FLAG_ALL        0x1  /* matches all providers */
#define FLAG_UNCOND     0x2  /* valid for booked providers too */
#define FLAG_PROV0      0x4  /* value for prov[0] present */
#define FLAG_PROV1      0x8  /* value for prov[1] present */
#define FLAG_VAR0      0x10  /* value for var[0] present */
#define FLAG_VAR1      0x20  /* value for var[1] present */
#define FLAG_WHITE     0x40  /* value for entry in white list */

struct skip {
	struct skip  *next;      /* next entry in list or NULL if last */
	int           flags;     /* details about kind of entry */
	int           prov[2];   /* first [0] and last [1] provider number */
	int           var[2];    /* same for variant number */
};

static struct skip *list[2] = {NULL, NULL};

/* remove space and comment in string */
static void strip (char *from) {
	char *to = from;
  while (*from && *from != '#') {
		if (!isspace(*from))
			*to++ = *from;
		++from;
	}
	*to = '\0';
}


int add_skipped_provider(char *line, char **msg) {
	char *next, *p[2], *q;
	int l, colour, uncond = 0, all_white = 0;
	struct skip entry, *add, *last[2] = {NULL, NULL};
	static char errmsg[128];

	*msg = errmsg;
	**msg = '\0';

	strip(line);

	/* whole line unconditional? */
	if (*line == '!') {
		uncond = 1;
		line++;
	}
	/* whole line for white list? */
	else if (*line == '~') {
		all_white = 1;
		line++;
	}

	while (1) {
		next = strchr(line, ';');
		if (next)
			*next++ = '\0';

		/* silently ignore empty ranges */
		if (!*line) {
			if (next) {
				line = next;
				continue;
			}
			else
				break;
		}

		/* select default list */
		colour = all_white ? WHITE : BLACK;

		/* line points to a NUL-terminated range */
		entry.flags = uncond ? FLAG_UNCOND : 0;

		/* current entry unconditional? */
		l = strlen(line);
		if ( *(line + l - 1) == '!' ) {
			*(line + --l) = '\0';
			entry.flags |= FLAG_UNCOND;
		}
		/* current entry for white list? */
		else if ( *(line + l - 1) == '~' ) {
			*(line + --l) = '\0';
			colour = WHITE;	
		}
		if (colour == WHITE) {
			entry.flags |= FLAG_WHITE;
			/* either on white list or uncondional or none of both */
			entry.flags &= ~FLAG_UNCOND;
		}
		/* wildcard entry? */
		if (*line == '*') {
			if (*(line + 1)) {
				snprintf(*msg, sizeof errmsg, "unexpected characters after '*': %s",
				         line +1);
				return 1;
			}
			else
				entry.flags |= FLAG_ALL;
		}
		/* range entry: <prov>- | -<prov> | <prov>-<prov> | <prov> */
		else {
			if (*line == '-') {
				p[0] = NULL;
				p[1] = line+1;
			}
			else if (*(line + l - 1) == '-') {
				*(line + --l) = '\0';
				p[0] = line;
				p[1] = NULL;
			}
			else if ( (q = strchr(line,'-')) ) {
				*q = '\0';
				p[0] = line;
				p[1] = q + 1;
			}
			else {
				p[0] = p[1] = line;
			}
			for (l=0; l < 2; l++) {
				if (!p[l])
					continue;
				q = NULL;
				entry.prov[l] = (int) strtol(p[l], &q, 10);
				entry.flags |= l ? FLAG_PROV1 : FLAG_PROV0;
				if (q && *q == ',') {
					p[l] = q + 1;
					q = NULL;
					entry.var[l] = (int) strtol(p[l], &q, 10);
					entry.flags |= l ? FLAG_VAR1 : FLAG_VAR0;
				}
				if (q && *q) {
					snprintf(*msg, sizeof errmsg,
                   "unexpected characters for prov[,var]: %s", q);
					return 1;
				}
			}
		} /* /range entry */
				
		/* entry is complete, save it to list */
		add = malloc(sizeof(struct skip));
		if (!add) {
			snprintf(*msg, sizeof errmsg, "Out of memory!");
			return 1;
		}
		*add = entry;
		/* wildcard entries are added on top of list */
		if (entry.flags & FLAG_ALL) {
			add->next = list[colour];
			list[colour] = add;
		}
		else {
			if (!last[colour] && list[colour]) {
				last[colour] = list[colour];
				while (last[colour]->next)
					last[colour] = last[colour]->next;
			}
			if (last[colour])
				last[colour]->next = add;
			else
				list[colour] = add;
			add->next = NULL;
			last[colour] = add;
		}
					
		if (next)
			line = next;
		else
			break;
	} /* /loop over ranges: <range>;<range> */

	*msg = NULL; 
	return 0;
}	


static int on_list(struct skip *testlist, int prov, int var, int booked) {
	struct skip *test = testlist;
	register int match, flags;

	while (test) {
		/* compare entry with query */
		match = 1;
		flags = test->flags;
		if (!(flags & FLAG_ALL)) {
			if (match && flags & FLAG_PROV0 && prov < test->prov[0])
				match = 0;
			if (match && flags & FLAG_PROV1 && prov > test->prov[1])
				match = 0;
			if (match && var != UNKNOWN) {
				if (flags & FLAG_PROV0 && prov == test->prov[0] &&
				    flags & FLAG_VAR0 && var < test->var[0])
					match = 0;
				if (match && flags & FLAG_PROV1 && prov == test->prov[1] &&
				    flags & FLAG_VAR1 && var > test->var[1])
					match = 0;
			}
			/* for UNKNOWN variant, entry must be valid for whole provider,
       * not only for some variant of this provider
       */
			if (match && var == UNKNOWN) {
				if (flags & FLAG_PROV0 && prov == test->prov[0] && flags & FLAG_VAR0)
					match = 0;
				if (match && flags & FLAG_PROV1 && prov == test->prov[1] &&
				    flags & FLAG_VAR1)
					match = 0;
			}
		} /* /no wildcard entry */
	
		if (match && ((test->flags & FLAG_UNCOND) == FLAG_UNCOND) >= booked)
			return 1;

		test = test->next;
	}

	return 0;
}

int is_skipped_provider(int prov, int var, int booked) {
	/* first look at white list, where there is no booked attribute */
	if (on_list(list[WHITE], prov, var, 0))
		return 0;

	return on_list(list[BLACK], prov, var, booked);
}


static int clear_list(struct skip **alist) {
	struct skip *this, *next = *alist;
	int n = 0;

	while ( (this=next) != NULL ) {
		next = this->next;
		free(this);
		++n;
	}

	*alist = NULL;

	return n;
}

int clear_skipped_provider(void) {
	return clear_list(&list[WHITE]) + clear_list(&list[BLACK]);
}


int dump_skipped_provider(char *str, int len) {
	int colour = BLACK;
	struct skip *entry = list[colour];
	char *end = str + len, *p;
	int n = 0, v;
	char range[64];

	if (len < 1)
		return -1;
	*str = '\0';
	
	if (!entry && colour == BLACK)
		entry = list[colour = WHITE];
	while (entry && str < end) {

		/* build string from range: * | [<prov>[,<var>][-][<prov>[,<var>]] */
		p = range;
		*p = '\0';
		if (entry->flags & FLAG_PROV0) {
			p += snprintf(p, 15, "%i", entry->prov[0]);
			if (entry->flags & FLAG_VAR0)
				p += snprintf(p, 15, ",%i", entry->var[0]);
		}
		/* write '-' separator when necessary:
     * - not both provider numbers present
     * - different provider numbers
     * (both provider number present and identical and:)
     * - both variants present and different
     * - exact one variant present
     */
		if ((entry->flags & (FLAG_PROV0|FLAG_PROV1)) != (FLAG_PROV0|FLAG_PROV1) ||
		    entry->prov[0] != entry->prov[1] ||
		    ( (v=(entry->flags & (FLAG_VAR0|FLAG_VAR1))) == (FLAG_VAR0|FLAG_VAR1)
        && entry->var[0] != entry->var[1] ) || 
		    v == FLAG_VAR0 || v== FLAG_VAR1) {
			*p++ = '-';
			if (entry->flags & FLAG_PROV1) {
				p += snprintf(p, 15, "%i", entry->prov[1]);
				if (entry->flags & FLAG_VAR1)
					p += snprintf(p, 15, ",%i", entry->var[1]);
			}
		}
		if (entry->flags & FLAG_ALL) {
			/* range should be "-" */
			range[0] = '*';
			range[1] = '\0';
		}
		
		str += snprintf(str, end-str, "%s%s%s%s", n ? ";" : "", range,
		                entry->flags & FLAG_WHITE  ? "~" : "",
		                entry->flags & FLAG_UNCOND ? "!" : "");
	
		++n;
		entry = entry->next;
		if (!entry && colour == BLACK)
			entry = list[colour = WHITE];
	}

	return (entry) ? -1 : n;
}


#ifdef STANDALONE
/* build it using
   gcc -g -Wall -DSTANDALONE -o rate_skip rate_skip.c
 */

int main (int argc, char *argv[]) {
	char input[BUFSIZ], *p;
	int ret, prov, var, booked;
	
	printf("Testing provider skipping, available commands:\n"
	       "  a<string>              -- add string to list of skipped providers\n"
         "  c                      -- clear list of skipped providers\n"
	       "  d                      -- dump list of skipped providers\n"
	       "  i<prov> <var> <booked> -- is <prov> ... on list?\n"
         "  q                      -- quit\n"
	       "  s<string>              -- strip <string>\n");

	do {
		printf(".");
		fflush(stdout);
		fgets(input, BUFSIZ, stdin);
		input[BUFSIZ-1] = '\0';	
		switch (*input) {
			case 'a': ret = add_skipped_provider(input+1, &p);
								if (ret)
									printf("msg = %s\n", p);
			          printf("ret = %i (%s)\n", ret, ret ? "ERROR" : "OK");
			          break;

			case 'c': ret = clear_skipped_provider();
			          printf("ret = %i (number of freed entries)\n", ret);
			          break;

			case 'd': ret = dump_skipped_provider(input, BUFSIZ);
								printf("out =>%s<\n", input);
								printf("ret = %i (%s)\n", ret,
								       ret >= 0 ? "number dumped entries" : "ERROR");
			          break;

			case 'i': p = input + 1;
			          prov = var = UNKNOWN;
								booked = 0;
			          prov = (int) strtol(p, &p, 10);
								if (p && *p) {
									var = (int) strtol(p, &p, 10);
									if (p && *p)
										booked = (int) strtol(p, NULL, 10);
								}
								ret = is_skipped_provider(prov, var, booked);
								printf("ret = %i (%s) for prov = %i, var = %i, booked = %i\n",
								       ret, ret ? "YES" : "NO", prov, var, booked);
			          break;

			case 'q':
			          break;

			case 's': strip(input+1);
			          printf("res =>%s<\n", input+1);
			          break;

			default:  printf("Unknown command (q to quit)\n");
			          break;

		} /* /switch */
	} while (*input != 'q');

	return 0;
}
#endif /* STANDALONE */


/*
 * $Log: rate_skip.c,v $
 * Revision 1.1  2005/02/23 14:33:40  tobiasb
 * New feature: provider skipping.
 * Certain providers can be completely ignored (skipped) when loading the
 * rate-file.  The selection is done by Q: tags in rate.conf or by skipProv=
 * in the parameter file.  The syntax is explained in the new manual page
 * rate.conf(5).  Absurd settings for provider skipping may cause trouble.
 * The version number will change to 4.70 in a few days after an update
 * of the rate-de.dat.
 *
 */

/* vim:set ts=2: */

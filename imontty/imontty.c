/* tty line ISDN status monitor
 *
 * (c) 1995-97 Volker Götz
 *
 * $Id: imontty.c,v 1.3 1998/04/28 08:33:52 paul Exp $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/isdn.h>
#include "imontty.h"

struct phone_entry {
   struct phone_entry *next;
   char phone[30];
   char name[30];
};

static struct phone_entry *phones;
static char *phonebook;
static int show_names = 1;

static int Star(char *, char *);

/*
 * Wildmat routines
 */
static int wildmat(char *s, char *p) {
  register int   last;
  register int   matched;
  register int   reverse;

  for ( ; *p; s++, p++)
    switch (*p) {
    case '\\':
      /* Literal match with following character; fall through. */
      p++;
    default:
      if (*s != *p)
	return(0);
      continue;
    case '?':
      /* Match anything. */
      if (*s == '\0')
	return(0);
      continue;
    case '*':
      /* Trailing star matches everything. */
      return(*++p ? Star(s, p) : 1);
    case '[':
      /* [^....] means inverse character class. */
      if ((reverse = (p[1] == '^')))
	p++;
      for (last = 0, matched = 0; *++p && (*p != ']'); last = *p)
	/* This next line requires a good C compiler. */
	if (*p == '-' ? *s <= *++p && *s >= last : *s == *p)
	  matched = 1;
      if (matched == reverse)
	return(0);
      continue;
    }
  return(*s == '\0');
}

static int Star(char *s, char *p) {
  while (wildmat(s, p) == 0)
    if (*++s == '\0')
      return(0);
  return(1);
}

/*
 * Read phonebook file and create a linked
 * list of phone entries.
 */
static void readphonebook() {
  FILE *f;
  struct phone_entry *p;
  struct phone_entry *q;
  char line[255];
  char *s;

  if (!(f = fopen(phonebook, "r"))) {
    fprintf(stderr, "Can't open %s\n",phonebook);
  }
  p = phones;
  while (p) {
    q = p->next;
    free(p);
    p = q;
  }
  phones = NULL;
  while (fgets(line,sizeof(line),f)) {
    if ((s = strchr(line,'\n')))
      *s = '\0';
    if (line[0]=='#')
      continue;
    s = strchr(line,'\t');
    *s++ = '\0';
    if (strlen(line) && strlen(s)) {
      q = malloc(sizeof(struct phone_entry));
      q->next = phones;
      strcpy(q->phone,line);
      strcpy(q->name,s);
      phones = q;
    }
  }
  fclose(f);
  if (!phones)
    show_names = 0;
}

/*
 * Find a phone number in list of phone entries.
 * If found, return corresponding name, else
 * return number string.
 */
char *find_name(char *num) {
  struct phone_entry *p = phones;
  static char tmp[100];

  sprintf(tmp, "%-28s", num);
  if (!show_names)
	return(tmp);
  while (p) {
    if (wildmat(num, p->phone)) {
      sprintf(tmp, "%-28s", p->name);
      break;
    }
    p = p->next;
  }
  return tmp;
}

void scan_str(char * buffer, char * field[], int max) {
    char * buf = buffer;
    int i;
    char * token = strtok(buf, "\t");

    for(i=0; i < max; i++) {
        token = strtok(NULL, " ");
        if ((field[i] = malloc(strlen(token) * sizeof(char))) != NULL)
            strcpy(field[i], token);
    }
}


void scan_int(char * buffer, int (*field)[], int max) {
    char * buf = buffer;
    int i;
    char * token = strtok(buf, "\t");

    for(i=0; i < max; i++) {
        token = strtok(NULL, " ");
        (*field)[i] = atoi(token);
    }
}

int main(int ac, char **argv) {

    FILE * isdninfo;
    char buf[IM_BUFSIZE];

    char * idmap[ISDN_MAX_CHANNELS];
    int chmap[ISDN_MAX_CHANNELS];
    int drmap[ISDN_MAX_CHANNELS];
    int usage[ISDN_MAX_CHANNELS];
    int flags[ISDN_MAX_DRIVERS];
    char * phone[ISDN_MAX_CHANNELS];

    int i, lines;

    if (!(isdninfo = fopen(PATH_ISDNINFO, "r"))) {
        fprintf(stderr, "ERROR: Can't open '%s'\n", PATH_ISDNINFO);
        exit(1);
    }

    phonebook = argv[1];

    if (phonebook)
       readphonebook();

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_str(buf, idmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &chmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &drmap, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &usage, ISDN_MAX_CHANNELS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_int(buf, &flags, ISDN_MAX_DRIVERS);

    fgets(buf, IM_BUFSIZE, isdninfo);
    scan_str(buf, phone, ISDN_MAX_CHANNELS);

    fclose(isdninfo);

    printf("\n%s\n\n", "ISDN channel status:");
    printf("%-20.20s\t%-6.6s%-6.6s%s\n", "Channel", "Usage", "Type", "Number");
    printf("----------------------------------------------------------------------\n");

    lines = 0;
    for(i=0; i < ISDN_MAX_CHANNELS; i++)
        if (chmap[i] >= 0 && drmap[i] >= 0) {

	    /* printf("%-20.20s/%i\t", idmap[i], chmap[i]); */
	    printf("%-20.20s\t", idmap[i]);

            if (usage[i] & ISDN_USAGE_OUTGOING)
		printf("%-6.6s", "Out");
            else if (usage[i] == ISDN_USAGE_EXCLUSIVE)
		printf("%-6.6s", "Excl.");
            else if (usage[i])
		printf("%-6.6s", "In");
            else
		printf("%-6.6s", "Off");

            switch(usage[i] & ISDN_USAGE_MASK) {
		case ISDN_USAGE_RAW:
		    printf("%-6.6s", "raw");
		    break;
		case ISDN_USAGE_MODEM:
		    printf("%-6.6s", "Modem");
		    break;
		case ISDN_USAGE_NET:
		    printf("%-6.6s", "Net");
		    break;
		case ISDN_USAGE_VOICE:
		    printf("%-6.6s", "Voice");
		    break;
		case ISDN_USAGE_FAX:
		    printf("%-6.6s", "Fax");
		    break;
		default:
		    printf("%-6.6s", "");
		    break;
	    }

	    if(usage[i] & ISDN_USAGE_MASK)
		printf("%s\n", find_name(phone[i]));
            else
		printf("\n");

	    if( !(++lines % 2) )
		printf("\n");
	}
	return 0;
}

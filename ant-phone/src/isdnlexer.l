%option prefix="isdn_"
%option nounput
%{
/*
 * isdn4linux options file format lexer
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
 *
 * ANT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ANT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ANT; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define YYSTYPE char*

/* GNU headers */
#include <string.h>
#include <libgen.h>

/* own headers */
#include "globals.h"
#include "isdnparser.h"
#include "isdnlexer.h"

typedef struct isdn_include_t isdn_include_t;
struct isdn_include_t {
  char* filename;
  YYLTYPE location;
  YY_BUFFER_STATE state;
  isdn_include_t* next;
};

isdn_include_t* isdn_include_list;
char* isdn_filename;
YYLTYPE isdn_lloc;

void isdn_locate();
%}

LETTER    [a-zA-Z]
DIGIT     [0-9]
STRING    ([^ {}"\n]|\\\n)+

/* exclusive start condition for C-style comments */
%x _INCLUDE_
%x _VALUE_
%%

INCLUDE([ \t]|"\\\n")*"("    {
                               isdn_locate();
                               BEGIN(_INCLUDE_);
			     }
<_INCLUDE_>[^ \t)]+	     { /* got absolute or relative (to the current
                                * file) filename
				*/
			       isdn_include_t* temp = (isdn_include_t*)
			         malloc(sizeof(isdn_include_t));
			       isdn_include_t* i; /* iterator */
			       int loop_detected;
			       FILE* temp_in;
			       
                               isdn_locate();
			       temp->filename = isdn_filename;
			       temp->location = isdn_lloc;
			       temp->state = YY_CURRENT_BUFFER;
			       temp->next = isdn_include_list;
			       isdn_include_list = temp;

			       if (isdn_text[0] == '/') { /* absolute path */
			         isdn_filename = strdup(isdn_text);
			       } else { /* relative path */
			         char* path1 = strdup(isdn_filename); /* old
				                                         name */
				 char* path2 = dirname(path1); /* just dir */

				 asprintf(&isdn_filename, "%s/%s",
				          path2, isdn_text);
				 free(path1);
			       }

			       /* loop detection */
			       loop_detected = 0;
			       for (i = isdn_include_list;
			            i != NULL && !loop_detected;
				    i = i->next)
			       {
			         if (!strcmp(i->filename, isdn_filename))
				   loop_detected = 1;
			       }

			       if (loop_detected ||
			           !(temp_in = fopen(isdn_filename, "r")))
			       {
			         /* ignoring non-existent included file */
			         if (debug) {
				   if (loop_detected)
				     fprintf(stderr, "Loop detected ");
				   else
				     fprintf(stderr, "Error ");
				   fprintf(stderr,
				     "reading ISDN options file %s "
				     "from %s. Ignoring.\n",
				     isdn_filename, temp->filename);
				 }
				 /* recover to old buffer */
				 free(isdn_filename);
				 isdn_filename = temp->filename;
				 isdn_include_list = temp->next;
			         free(temp);
			       } else {
			         /* include file OK */
				 isdn_in = temp_in;
                                 if (debug)
                                   fprintf(stderr,
				     "Reading options file %s "
				     "from %s ...\n",
				     isdn_filename, temp->filename);
				 yy_switch_to_buffer(
				   yy_create_buffer(isdn_in, YY_BUF_SIZE));
				 BEGIN(INITIAL);
				 isdn_lloc.first_line = 1;
				 isdn_lloc.last_line = 1;
				 isdn_lloc.first_column = 1;
				 isdn_lloc.last_column = 1;
			       }
                             }
<<EOF>>                      { /* end of current file */
                               if (!isdn_include_list) { /* last buffer */
			         yyterminate();
			       } else { /* restore previous buffer */
			         isdn_include_t* temp = isdn_include_list;
				 
				 if (debug)
				   fprintf(stderr, "Returning to %s ...\n",
				           temp->filename);
			         yy_delete_buffer(YY_CURRENT_BUFFER);
				 yy_switch_to_buffer(temp->state);
				 BEGIN(_INCLUDE_);
				 isdn_lloc = temp->location;
				 free(isdn_filename);
				 isdn_filename = temp->filename;
				 isdn_include_list = temp->next;
				 free(temp);
			       }
                             }
<_INCLUDE_>[ \t]+            { /* eat whitespace in _INCLUDE_ mode */
                               isdn_locate();
                             }
<_INCLUDE_>")"               { /* return to normal mode */
                               isdn_locate();
			       BEGIN(INITIAL);
                             }
{LETTER}+({DIGIT}|{LETTER})* { /* NAME */
                               isdn_locate();
                               isdn_lval = strdup(isdn_text);
			       return ISDN_TOKEN_NAME;
                             }
<_VALUE_>([ \t]|"\\\n")+     { /* whitespace */
                               isdn_locate();
                             }
<_VALUE_>"{"                 { /* subsection begin */
                               isdn_locate();
			       BEGIN(INITIAL);
			       return *isdn_text;
			     }
<_VALUE_>(""|{STRING}(([ \t]|"\\\n")+{STRING})*)\n { /* VALUE */
                               isdn_locate();
                               isdn_lval = strdup(isdn_text);
			       isdn_lval = realloc(isdn_lval,
			                           strlen(isdn_text));
			       isdn_lval[strlen(isdn_text) - 1] = '\0';
			       BEGIN(INITIAL);
                               return ISDN_TOKEN_VALUE;
                             }
<_VALUE_>\"[^"]*\"([ \t]|"\\\n")*\n { /* quoted VALUE */
                               int len = strchr(isdn_text + 1, '"') - isdn_text;
			       
                               isdn_locate();
                               isdn_lval = (char *)malloc(len);
                               strncpy(isdn_lval, isdn_text + 1, len - 1);
                               isdn_lval[len - 1] = '\0';
			       BEGIN(INITIAL);
			       return ISDN_TOKEN_VALUE;
			     }
"#".*                        { /* eat up comments */
                               isdn_locate();
			     }
"="                          {
                               isdn_locate();
			       BEGIN(_VALUE_);
			       return *isdn_text;
			     }
([ \t]|"\\\n")+		     { /* eat up whitespace */
                               isdn_locate();
			     }
("}"|"["|"]")                { /* subsection end or NAME bracket */
                               isdn_locate();
			       return *isdn_text;
			     }
\n                           { /* real new line */
                               isdn_locate();
			     }
.                            { /* eat up rest */
                               isdn_locate();
			       if (debug) {
			         fprintf(stderr,
				   "isdnlexer: Unrecognized character: %c "
				   "at %d:%d\n",
				   *isdn_text,
				   isdn_lloc.first_line,
				   isdn_lloc.first_column);
			       }
			     }

%%

/* For portability's sake */
int isdn_wrap() { return 1; }

/*
 * adjusts isdn_lloc according to isdn_text
 */
void isdn_locate() {
  char* temp;

  isdn_lloc.first_line = isdn_lloc.last_line;
  isdn_lloc.first_column = isdn_lloc.last_column;
  
  for (temp = yytext; *temp != '\0'; temp++) {
    if (*temp == '\n') {
      ++ isdn_lloc.last_line;
      isdn_lloc.last_column = 1;
    } else {
      ++ isdn_lloc.last_column;
    }
  }
}

/*
 * to be called directly before the parser run
 *
 * on success, isdn_in contains valid FILE* handle, else NULL
 */
void isdn_lexer_init(char* filename) {
  isdn_lloc.first_line = 1;
  isdn_lloc.first_column = 1;
  isdn_lloc.last_line = 1;
  isdn_lloc.last_column = 1;
  isdn_include_list = NULL;
  if (debug)
    fprintf(stderr, "Reading options file: %s ...\n", filename);
  if (!(isdn_in = fopen(filename, "r"))) {
    fprintf(stderr, "Error opening options file %s.\n", filename);
  } else {
    isdn_filename = strdup(filename);
  }
}

/*
 * to be called directly after the parser run
 */
void isdn_lexer_deinit() {
  if (fclose(isdn_in) == EOF) {
    fprintf(stderr, "Warning: Couldn't close options file.\n");
  }
  free(isdn_filename);
}


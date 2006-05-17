%option prefix="callerid_"
%option nounput
%{
/*
 * saved caller id file lexer
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

#define YYSTYPE char *

#include <string.h>
#include "globals.h"
#include "calleridparser.h"
YYLTYPE callerid_lloc;

void callerid_locate();
%}

LETTER    [a-zA-Z]
DIGIT     [0-9]
NOEM      {LETTER}|{DIGIT}|[-:)(*#]

%%

"|"|!!                         callerid_locate(); return CID_TOKEN_DELIMITER;
{DIGIT}+-{DIGIT}+-{DIGIT}+" "{DIGIT}+:{DIGIT}+:{DIGIT}+ {
                                 callerid_locate();
                                 callerid_lval = strdup(callerid_text);
                                 return CID_TOKEN_DATE;
                               }
IN|OUT                         {
                                 callerid_locate();
				 callerid_lval = strdup(callerid_text);
				 return CID_TOKEN_TYPE;
			       }
({DIGIT}|[*#+?A-D])+           {
                                 callerid_locate();
				 callerid_lval = strdup(callerid_text);
				 return CID_TOKEN_NUMBER;
			       }
(!?{NOEM}+)+!?                 {
                                 callerid_locate();
				 callerid_lval = strdup(callerid_text);
				 return CID_TOKEN_MSG;
			       }

\n                             callerid_locate(); return *callerid_text;
.                              callerid_locate();
                               /* eat up rest (handled as white space) */

%%

/* For portability's sake */
int callerid_wrap() { return 1; }

/*
 * adjusts callerid_lloc according to callerid_text
 */
void callerid_locate() {
  char* temp;

  callerid_lloc.first_line = callerid_lloc.last_line;
  callerid_lloc.first_column = callerid_lloc.last_column;
  
  for (temp = yytext; *temp != '\0'; temp++) {
    if (*temp == '\n') {
      ++ callerid_lloc.last_line;
      callerid_lloc.last_column = 1;
    } else {
      ++ callerid_lloc.last_column;
    }
  }
}


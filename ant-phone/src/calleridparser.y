%name-prefix="callerid_"
%{
/*
 * saved callerid file parser
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

/* type for semantic values of symbols: malloc'ed strings */
#define YYSTYPE char*

/* let yyparse() accept one argument of type void * */
#define YYPARSE_PARAM session

/* regular GNU system includes */
#include <stdio.h>
#include <stdlib.h>

/* own header files */
#include "globals.h"
#include "util.h"
#include "callerid.h"

int callerid_lex (void);
void callerid_error(const char *message);
%}

/* terminal symbols */
%token CID_TOKEN_DATE
%token CID_TOKEN_TYPE
%token CID_TOKEN_NUMBER
%token CID_TOKEN_MSG
%token CID_TOKEN_DELIMITER

%%
settings   :
           | settings line
;

line       : '\n'
           | CID_TOKEN_DATE CID_TOKEN_DELIMITER
             CID_TOKEN_TYPE CID_TOKEN_DELIMITER
             CID_TOKEN_NUMBER CID_TOKEN_DELIMITER
             CID_TOKEN_NUMBER CID_TOKEN_DELIMITER
             CID_TOKEN_MSG '\n'
               { cid_add_saved_line(session, $1, $3, $5, $7, $9);
	         free($1); free($3); free($5); free($7); free($9); }
           | error '\n'
             { if (debug)
	         fprintf(stderr,
		         "Warning: Parsing callerid history file:%d, "
			 "recovering after error.\n", @1.last_line);
	     }
;

%%

/*
 * callback for yyparse(), (also) called on errors (hopefully) handled
 * by error token actions in grammar, but not if errors occur to
 * often (bison needs 3 "correct" tokens to recover)
 */
void callerid_error(const char *message) {
  if (debug)
    fprintf(stderr,
	    "Warning: Parsing callerid history file line %d: %s.\n",
	    callerid_lloc.first_line, message);
}


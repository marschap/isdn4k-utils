%name-prefix="isdn_"
%{
/*
 * isdn4linux options file format parser
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

/* regular GNU system includes */
#include <stdio.h>
#include <stdlib.h>

/* own header files */
#include "globals.h"
#include "util.h"
#include "settings.h"
#include "isdntree.h"

typedef struct isdn_list_store_t isdn_list_store_t;
struct isdn_list_store_t {
  isdn_tree_node_t* list; /* list itself (start) */
  isdn_tree_node_t** last;  /* pointer to pointer to last element */
                            /* (which in turn should always be NULL) */
};

int isdn_lex(void);
void isdn_error(const char *message);
%}

/* types of semantic values */
%union {
  char* s;
  isdn_list_store_t list;
  isdn_tree_node_t* element;
}

/* terminal symbols */
%token <s> ISDN_TOKEN_NAME
%token <s> ISDN_TOKEN_VALUE

/* non terminal symbols */
%type <list> config;
%type <list> sections;
%type <element> section;
%type <list> entries;
%type <element> entry;
%type <s> value;

%%

config     : entries sections
	       {
	         if ($1.list == NULL) {
		   $$ = $2;
		 } else {
		   $$.list = $1.list;
		   *$1.last = $2.list;
		   $$.last = $2.last;
		 }
		 isdn_tree = $$.list;
	       }
;

sections   : { $$.list = NULL; $$.last = NULL; }
           | sections section
	       {
	         if ($1.list == NULL) {
		   $$.list = $2;
		   $$.last = &$2->next;
		 } else {
		   $$.list = $1.list;
		   *$1.last = $2;
		   $$.last = &$2->next;
		 }
	       }
;

section    : '[' ISDN_TOKEN_NAME ']' entries
	       {
	         if (!($$ =
		       (isdn_tree_node_t*) malloc(sizeof(isdn_tree_node_t))))
		 {
		   fprintf(stderr, "Error: Out of memory.\n");
		   exit(1);
		 }
		 $$->type = ISDN_NODE_TYPE_SECTION;
		 $$->name = $2;
		 $$->content.section = $4.list;
		 /* finally: only list needed */
		 $$->next = NULL;
	       }
;

entries    : { $$.list = NULL; $$.last = NULL; }
	   | entries entry
	       {
	         if ($1.list == NULL) {
		   $$.list = $2;
		   $$.last = &$2->next;
		 } else {
		   $$.list = $1.list;
		   *$1.last = $2;
		   $$.last = &$2->next;
		 }
	       }
	   | entries error
	       {
	         fprintf(stderr, "Error region from %d:%d up to %d:%d.\n",
		         @2.first_line, @2.first_column,
			 @2.last_line, @2.last_column);
	         $$ = $1;
	       }
;

entry      : ISDN_TOKEN_NAME '=' value
	       {
	         if (!($$ =
		       (isdn_tree_node_t*) malloc(sizeof(isdn_tree_node_t))))
		 {
		   fprintf(stderr, "Error: Out of memory.\n");
		   exit(1);
		 }
		 $$->type = ISDN_NODE_TYPE_ENTRY;
		 $$->name = $1;
		 $$->content.value = $3;
		 $$->next = NULL;
	       }
	   | ISDN_TOKEN_NAME '=' '{' config '}'
	       {
	         if (!($$ =
		       (isdn_tree_node_t*) malloc(sizeof(isdn_tree_node_t))))
		 {
		   fprintf(stderr, "Error: Out of memory.\n");
		   exit(1);
		 }
		 $$->type = ISDN_NODE_TYPE_SUBSECTION;
		 $$->name = $1;
		 $$->content.subsection = $4.list;
		 /* finally: only list needed */
		 $$->next = NULL;
	       }
;

value      : ISDN_TOKEN_VALUE
	   | ISDN_TOKEN_NAME
;

%%

/*
 * callback for yyparse(), (also) called on errors (hopefully) handled
 * by error token actions in grammar, but not if errors occur to
 * often (bison needs 3 "correct" tokens to recover)
 */
void isdn_error(const char *message) {
  if (debug)
    fprintf(stderr,
	    "Warning: Parsing isdn options file: %d:%d: %s.\n",
	    isdn_lloc.first_line, isdn_lloc.first_column, message);
}


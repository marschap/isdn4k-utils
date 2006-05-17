/*
 * isdn4linux options file tree support
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2003 Roland Stigge
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

/* GNU headers */
#include <stdio.h>
#include <stdlib.h>

/* own headers */
#include "isdntree.h"

isdn_tree_node_t* isdn_tree;

/*
 * helper function for isdn_tree_dump recursively dumping the tree
 */
static void isdn_tree_dump_list(isdn_tree_node_t* list, int indent) {
  while (list != NULL) {
    switch (list->type) {
      case ISDN_NODE_TYPE_ENTRY:
	printf("%*sEntry \"%s\" => \"%s\"\n",
	       indent, "", list->name, list->content.value);
	break;
      case ISDN_NODE_TYPE_SECTION:
	printf("%*sSection [%s]:\n",
	       indent, "", list->name);
	isdn_tree_dump_list(list->content.section, indent + 2);
	break;
      case ISDN_NODE_TYPE_SUBSECTION:
	printf("%*sSubsection \"%s\":\n",
	       indent, "", list->name);
	isdn_tree_dump_list(list->content.subsection, indent + 2);
        break;
      default:
	fprintf(stderr, "Unknown ISDN_NODE_TYPE\n");
    }
    
    list = list->next;
  }
}

/*
 * dump whole tree to stdout
 */
void isdn_tree_dump() {
  printf("ISDN config dump:\n");
  isdn_tree_dump_list(isdn_tree, 2);
}

/*
 * helper for isdn_tree_free
 * -> recursive release of memory from tree
 */
static void isdn_tree_free_list(isdn_tree_node_t* list) {
  while (list != NULL) {
    isdn_tree_node_t* temp;

    free(list->name);

    switch (list->type) {
      case ISDN_NODE_TYPE_ENTRY:
	free(list->content.value);
	break;
      case ISDN_NODE_TYPE_SECTION:
	isdn_tree_free_list(list->content.section);
	break;
      case ISDN_NODE_TYPE_SUBSECTION:
	isdn_tree_free_list(list->content.subsection);
        break;
      default:
	fprintf(stderr, "Unknown ISDN_NODE_TYPE\n");
    }
    
    temp = list->next;
    free(list);
    list = temp;
  }
}

/*
 * cleans up: releases all memory associated with tree
 */
void isdn_tree_free() {
  isdn_tree_free_list(isdn_tree);
}


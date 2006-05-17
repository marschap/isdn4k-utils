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

#ifndef _ISDNTREE_H
#define _ISDNTREE_H

typedef enum isdn_node_type_t isdn_node_type_t;
typedef union isdn_node_value_t isdn_node_value_t;
typedef struct isdn_tree_node_t isdn_tree_node_t;

enum isdn_node_type_t {
  ISDN_NODE_TYPE_ENTRY,
  ISDN_NODE_TYPE_SECTION,
  ISDN_NODE_TYPE_SUBSECTION
};

union isdn_node_value_t {
  char* value;
  isdn_tree_node_t* section;
  isdn_tree_node_t* subsection;
};

struct isdn_tree_node_t {
  isdn_node_type_t type;
  char* name;
  isdn_node_value_t content;
  isdn_tree_node_t* next; /* it's actually a list */
};

extern isdn_tree_node_t* isdn_tree;

void isdn_tree_dump();
void isdn_tree_free();

#endif /* _ISDNTREE_H */

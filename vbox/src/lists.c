/*
** $Id: lists.c,v 1.3 1997/02/26 13:10:39 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#include <stdlib.h>
#include <string.h>

#include "lists.h"
#include "libvbox.h"

/*************************************************************************
 ** list_init():	Initialize a minlist structure.								**
 *************************************************************************/

void list_init(struct minlist *list)
{
	if (list)
	{
		list->lh_beg = NULL;
		list->lh_end = NULL;
	}
}

/*************************************************************************
 ** list_exit():	Frees all nodes and initialize the minlist structure.	**
 *************************************************************************/

void list_exit(struct minlist *list)
{
	struct minnode *node;
	struct minnode *kill;

	if (list)
	{
		node = list->lh_beg;
		
		while (node)
		{
			kill = node;
			node = node->ln_next;
			
			free(kill);
		}

		list_init(list);
	}
}

/*************************************************************************
 ** list_add_node():	Adds a node to a minlist structure.						**
 *************************************************************************/

void list_add_node(struct minlist *list, struct minnode *node)
{
	if ((list) && (node))
	{
		if (list->lh_end)
		{
			list->lh_end->ln_next = node;

			node->ln_next = NULL;
			node->ln_prev = list->lh_end;

			list->lh_end = node;
		}
		else
		{
			node->ln_next = NULL;
			node->ln_prev = NULL;
		
			list->lh_beg = node;
			list->lh_end = node;
		}
	}
}

/*************************************************************************
 ** list_find_node():	Searchs a node in a minlist structure.				**
 *************************************************************************/

struct minnode *list_find_node(struct minlist *list, char *name)
{
	struct minnode *node;

	if ((list) && (name))
	{
		if (list->lh_beg)
		{
			node = list->lh_beg;

			while (node)
			{
				if (strcmp(node->ln_name, name) == 0) return(node);
				
				node = node->ln_next;
			}
		}
	}

	return(NULL);
}

/*************************************************************************
 ** list_rem_node():	Removes a node from a minlist structure.				**
 *************************************************************************/

void list_rem_node(struct minlist *list, struct minnode *kill)
{
	struct minnode *node;
	struct minnode *next;
	struct minnode *prev;

	if ((list) && (kill))
	{
		if (list->lh_beg)
		{
			node = list->lh_beg;

			while (node)
			{
				if (node == kill)
				{
					next = node->ln_next;
					prev = node->ln_prev;

					if (next) next->ln_prev = prev;
					if (prev) prev->ln_next = next;

					if (node == list->lh_beg) list->lh_beg = node->ln_next;
					if (node == list->lh_end) list->lh_end = node->ln_prev;

					free(node);

					return;
				}

				node = node->ln_next;
			}
		}
	}
}

/*************************************************************************
 ** list_make_node():	Creates a new node filled with the <name>.		**
 *************************************************************************/

struct minnode *list_make_node(char *name)
{
	struct minnode *node = NULL;
	char				*temp = NULL;
	
	if (name)
	{
		if ((node = malloc(sizeof(node))))
		{
			if ((temp = malloc(strlen(name))))
			{
				strcpy(temp, name);
			
				node->ln_next = NULL;
				node->ln_prev = NULL;
				node->ln_name = temp;
			}
			else free(node);
		}
	}

	return(node);
}

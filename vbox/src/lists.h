/*
** $Id: lists.h,v 1.3 1997/02/26 13:10:40 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LISTS_H
#define _VBOX_LISTS_H 1

/** Strukturen ***********************************************************/

struct minnode
{
	struct minnode *ln_next;					  /* Pointer to the next node	*/
	struct minnode *ln_prev;				  /* Pointer to the preview node	*/
	char				*ln_name;					  /* Pointer to the node name	*/
};

struct minlist
{
	struct minnode *lh_beg;		 /* Pointer to the first node in the list	*/
	struct minnode *lh_end;		  /* Pointer to the last node in the list	*/
};

/** Prototypes ***********************************************************/

extern void		 			 list_init(struct minlist *);
extern void		 			 list_exit(struct minlist *);
extern void		 			 list_add_node(struct minlist *, struct minnode *);
extern void		 			 list_rem_node(struct minlist *, struct minnode *);
extern struct minnode	*list_find_node(struct minlist *, char *);
extern struct minnode	*list_make_node(char *);

#endif /* _VBOX_LISTS_H */

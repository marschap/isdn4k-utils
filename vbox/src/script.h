
#ifndef _VBOX_SCRIPT_H
#define _VBOX_SCRIPT_H 1

#include "lists.h"

/** Prototypes ***********************************************************/

extern struct minlist breaklist;

extern int script_run(char *);
extern int script_check_interpreter(void);

#endif /* _VBOX_SCRIPT_H */

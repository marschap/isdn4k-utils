/*
** $Id: libvbox.h,v 1.5 1997/04/04 09:32:41 michael Exp $
**
** Copyright (C) 1996, 1997 Michael 'Ghandi' Herold
*/

#ifndef _VBOX_LIBVBOX_H
#define _VBOX_LIBVBOX_H 1

#include <stdio.h>

/** Defines **************************************************************/

#define CTRL_NAME_STOP			"vboxctrl-stop"
#define CTRL_NAME_ANSWERNOW	"vboxctrl-answernow"
#define CTRL_NAME_ANSWERALL	"vboxctrl-answerall"
#define CTRL_NAME_REJECT		"vboxctrl-reject"

#define CTRL_MODE_REMOVE		(0)
#define CTRL_MODE_CREATE		(1)

#define KERNEL_SAMPLE_FREQ		(8000)

#define VAH_MAGIC					"VBOX"							/* Magic id	*/
#define VAH_MAX_MAGIC			(4)
#define VAH_MAX_NAME				(32)
#define VAH_MAX_CALLERID		(32)
#define VAH_MAX_PHONE			(32)
#define VAH_MAX_LOCATION		(64)
#define VHA_MAX_RESERVED_A    (32)
#define VHA_MAX_RESERVED_B    (64)

/** vboxd support *********************************************************/

#define VBOXD_DEF_PORT           (20012)           /* Default server port */

#define VBOXD_VAL_HELP           "180"                   /* Help messages */
#define VBOXD_VAL_COUNT          "181"              /* Count new messages */
#define VBOXD_VAL_MESSAGE        "182"                   /* Get a message */
#define VBOXD_VAL_HEADER         "183"            /* Get a message header */
#define VBOXD_VAL_LIST           "184"                   /* List messages */

#define VBOXD_VAL_SERVEROK       "280"                    /* Server ready */
#define VBOXD_VAL_SERVERQUIT     "281"                     /* Server quit */
#define VBOXD_VAL_ACCESSOK       "282"                       /* Access ok */
#define VBOXD_VAL_LOGINOK        "283"                   /* User login ok */

#define VBOXD_VAL_ACCESSDENIED   "580"                       /* No access */
#define VBOXD_VAL_BADARGS        "583"     /* Not enough or too many args */
#define VBOXD_VAL_BADPASSWD      "584"                    /* Bad password */
#define VBOXD_VAL_BADMESSAGE     "585"                     /* Bad message */
#define VBOXD_VAL_BADCOMMAND     "586"                     /* Bad command */
#define VBOXD_VAL_TEMPERROR      "589"                      /* Misc error */

#define VBOXC_ERR_OK             ( 0)
#define VBOXC_ERR_UNKNOWNHOST    (-1)
#define VBOXC_ERR_NOSOCKET       (-2)
#define VBOXC_ERR_NOCONNECT      (-3)
#define VBOXC_ERR_NOFILEIO       (-4)
#define VBOXC_ERR_GETMESSAGE     (-5)

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE					(1)
#define FALSE					(0)

#define returnerror()		return(FALSE)
#define returnok()			return(TRUE)
#define printstring			sprintf
#define close_and_mone(F)  { close(F); F = -1; }
#define close_and_null(S)  { fclose(S); S = NULL; }

/** Structures ***********************************************************/

typedef struct
{
	char					magic[4];
	unsigned long int	time;
	unsigned long int	compression;
	char					callerid[VAH_MAX_CALLERID + 1];
	char					name[VAH_MAX_NAME + 1];
	char					phone[VAH_MAX_PHONE + 1];
	char					location[VAH_MAX_LOCATION + 1];
	char              reserved1[VHA_MAX_RESERVED_A + 1];
	char              reserved2[VHA_MAX_RESERVED_A + 1];
	char              reserved3[VHA_MAX_RESERVED_B + 1];
	char              reserved4[VHA_MAX_RESERVED_B + 1];
} vaheader_t;

/** Variables ************************************************************/

extern char *compressions[];

/** Prototypes ***********************************************************/

extern int ctrl_create(char *, char *);
extern int ctrl_remove(char *, char *);
extern int ctrl_ishere(char *, char *);

extern void xstrncpy(char *, char *, int);
extern void xstrncat(char *, char *, int);
extern void xpause(unsigned long);
extern long xstrtol(char *, long);

extern int header_put(int, vaheader_t *);
extern int header_get(int, vaheader_t *);

extern int get_nr_messages(char *, int);
extern int get_message_ptime(int, int);







extern FILE *vboxd_r_stream;
extern FILE *vboxd_w_stream;
extern int   vboxd_r_fd;
extern int   vboxd_w_fd;



extern int   vboxd_connect(char *, int);
extern void  vboxd_disconnect(void);
extern char *vboxd_get_message(void);
extern void  vboxd_put_message(char *, ...);
extern int   vboxd_test_response(char *);


#endif /* _VBOX_LIBVBOX_H */

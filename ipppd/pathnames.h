/*
 * define path names
 *
 * $Id: pathnames.h,v 1.2 1997/03/24 01:18:01 fritz Exp $
 */

#include "config.h"

#ifdef HAVE_PATHS_H
# include <paths.h>
#else
# define _PATH_VARRUN 	"/etc/ppp/"
# define _PATH_DEVNULL	"/dev/null"
#endif

#define _PATH_UPAPFILE 	"/etc/ppp/pap-secrets"
#define _PATH_CHAPFILE 	"/etc/ppp/chap-secrets"
#define _PATH_SYSOPTIONS "/etc/ppp/ioptions"
#define _PATH_IPUP	"/etc/ppp/ip-up"
#define _PATH_IPDOWN	"/etc/ppp/ip-down"
#define _PATH_TTYOPT	"/etc/ppp/ioptions."
#define _PATH_CONNERRS	"/etc/ppp/connect-errors"
#define _PATH_PEERFILES     "/etc/ppp/peers/"
#define _PATH_USEROPT	".ippprc"

/* Programs for processing authenticated logins */
#define _PATH_AUTHUP   "/etc/ppp/auth-up"
#define _PATH_AUTHDOWN "/etc/ppp/auth-down"

# define _PATH_IPXUP	"/etc/ppp/ipx-up"
# define _PATH_IPXDOWN	"/etc/ppp/ipx-down"


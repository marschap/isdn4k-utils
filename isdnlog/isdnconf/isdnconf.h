/* $Id: isdnconf.h,v 1.2 1997/03/23 20:58:09 luethje Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1995, 1996 by Andreas Kool (akool@Kool.f.EUnet.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Log: isdnconf.h,v $
 * Revision 1.2  1997/03/23 20:58:09  luethje
 * some bugfixes
 *
 */

#ifndef _ISDN_CONF_H_
#define _ISDN_CONF_H_

#define PUBLIC extern
#include "tools.h"

#ifdef _ISDN_CONF_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

#undef _EXTERN

#endif /* _ISDN_CONF_H_ */

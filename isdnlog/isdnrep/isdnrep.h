/* $Id: isdnrep.h,v 1.2 1997/03/23 20:25:25 luethje Exp $
 *
 * ISDN accounting for isdn4linux.
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
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
 */

#ifndef _ISDNREP_H_
#define _ISDNREP_H_

#define PUBLIC extern
#include <tools.h>

#ifdef  MAXUNKNOWN
#undef  MAXUNKNOWN
#endif
#define MAXUNKNOWN   500

#ifdef  MAXCONNECTS
#undef  MAXCONNECTS
#endif
#define MAXCONNECTS  500

#ifdef _ISDNREP_C_
#define _EXTERN
#else
#define _EXTERN extern
#endif

_EXTERN double cheap96(time_t when, int zone, int *zeit);
_EXTERN double cheap(time_t when, int zone);

#undef _EXTERN

#endif /* _ISDNREP_H_ */

/*
 * $Id: rate_skip.h,v 1.1 2005/02/23 14:33:40 tobiasb Exp $
 *
 * rate database (Tarifdatenbank) -- skipping of providers
 *
 * Copyright 2005 Tobias Becker <tobiasb@isdn4linux.de>
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
 */

#ifndef _RATE_SKIP_H_
#define _RATE_SKIP_H_

int add_skipped_provider(char *line, char **msg);
int is_skipped_provider(int prov, int var, int booked);
int clear_skipped_provider(void);
int dump_skipped_provider(char *str, int len);

#endif /* _RATE_SKIP_H_ */

/*
 * $Log: rate_skip.h,v $
 * Revision 1.1  2005/02/23 14:33:40  tobiasb
 * New feature: provider skipping.
 * Certain providers can be completely ignored (skipped) when loading the
 * rate-file.  The selection is done by Q: tags in rate.conf or by skipProv=
 * in the parameter file.  The syntax is explained in the new manual page
 * rate.conf(5).  Absurd settings for provider skipping may cause trouble.
 * The version number will change to 4.70 in a few days after an update
 * of the rate-de.dat.
 *
 */

/* vim:set ts=2: */

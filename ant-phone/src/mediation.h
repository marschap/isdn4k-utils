/*
 * functions for mediation between ISDN and OSS
 *
 * This file is part of ANT (Ant is Not a Telephone)
 *
 * Copyright 2002, 2003 Roland Stigge
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

#include "session.h"

/* mediation internal recording buffer (number of shorts): */
#define MEDIATION_RECBUFSIZE 16384

extern int finished;
extern int hangup;

int mediation_makeLUT(int format_in, unsigned char **LUT_in,
		      int format_out, unsigned char **LUT_out,
		      unsigned char **LUT_generate,
		      unsigned char **LUT_analyze,
		      short **LUT_ulaw2short);
int write_buf(int fd, unsigned char *outbuf, int outbuf_size);
void process_isdn_source(session_t *session);
void process_audio_source(session_t *session);

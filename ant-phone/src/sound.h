/*
 * OSS sound handling functions
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

#include <sys/soundcard.h>

#define DEFAULT_FRAGMENT_SIZE 1024
#define DEFAULT_AUDIO_DEVICE_NAME_IN "/dev/dsp"
#define DEFAULT_AUDIO_DEVICE_NAME_OUT "/dev/dsp"

extern int default_audio_priorities[];

int init_audio_device(int audio_fd, int format, int channels, int *speed,
		      int *fragment_size);
int open_audio_devices(char *in_audio_device_name,
		       char *out_audio_device_name,
		       int channels, int *format_priorities,
		       int *audio_fd_in, int *audio_fd_out,
		       int *fragment_size_in, int *fragment_size_out,
		       int *format_in, int *format_out,
		       int *speed_in, int *speed_out);
int close_audio_devices(int audio_fd_in, int audio_fd_out);
int audio_stop(int audio_fd_in, int audio_fd_out);
int sample_size_from_format(int format);
int audio_get_read_fragments_number(int fd);
int audio_get_read_fragments_total(int fd);
int audio_get_write_fragments_number(int fd);
int audio_get_write_fragments_total(int fd);

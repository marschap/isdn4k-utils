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

#include "config.h"

/* GNU headers */
#include <stdio.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
  #include <fcntl.h>
#endif
#include <math.h>
#include <errno.h>
#include <sys/soundcard.h>
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#include <string.h>

/* own header files */
#include "globals.h"
#include "sound.h"

int default_audio_priorities[] = {AFMT_S16_LE,/* try formats in this order */ \
			          AFMT_S16_BE,\
			          AFMT_U16_LE,\
			          AFMT_U16_BE,\
                                  AFMT_U8,\
                                  AFMT_S8,\
            /* ulaw at last because no native soundcard support assumed */ \
			          AFMT_MU_LAW,\
				  0}; /* end of list */
/*
 * common initialization for a specific audio device
 *
 * NOTE: Assumes opened device, but leaves it in a state not able for further
 *       use when not successful. The device has to be closed and opened again.
 *
 * input: audio_fd: valid file descriptor of an audio device,
 *        format, number of channels, requested sampling rate
 *        requested fragment sizes
 *
 * returns: 0 if successful, non-zero otherwise
 *
 * output:  speed: actually sampling rate
 *          fragment_size: actually fragment size for this device
 * 
 */
int init_audio_device(int audio_fd, int format, int channels, int *speed,
		      int *fragment_size) {
  int temp_format;
  int temp_channels;
  int temp_speed;
  int formats_mask;
  int fragment_arg;

  /* set fragment size */
  fragment_arg = 0x7FFF0000 + logb(*fragment_size);
  
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &fragment_arg)) {
    perror("SNDCTL_DSP_SETFRAGMENT");
    return(-1);
  }

  /* query supported audio formats */
  if (ioctl(audio_fd, SNDCTL_DSP_GETFMTS, &formats_mask) == -1) {
    perror("SNDCTL_DSP_GETFMTS");
    return(-1);
  }
  if (!(formats_mask & format)) { /* requested format not supported */
    return(-1);                   /* caller should try another one  */
  }

  /* audio format */
  temp_format = format;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &temp_format) == -1) {
    perror("SNDCTL_DSP_SETFMT");
    return(-1);
  }

  /* number of channels */
  temp_channels = channels;
  if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &temp_channels) == -1) {
    perror("SNDCTL_DSP_CHANNELS");
    return(-1);
  }
  if (temp_channels != channels) {
    fprintf(stderr, "Error: %d not supported as number of channels", channels);
    return(-1);
  }

  /* sampling rate */
  temp_speed = *speed;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &temp_speed) == -1) {
    perror("SNDCTL_DSP_SPEED");
    /* rarely returned because of oss' liberal use of other possible speeds */
    return(-1);
  }
  *speed = temp_speed; /* actually used speed  */

  /* "verify" fragment size:
   *  let the driver actually calculate the fragment size
   */
  if (ioctl(audio_fd, SNDCTL_DSP_GETBLKSIZE, &fragment_arg)) {
    perror("SNDCTL_DSP_GETBLKSIZE");
    return(-1);
  }
  if (fragment_arg != *fragment_size)
    fprintf(stderr, "Note: Using non-default fragment size %d.\n",
	    fragment_arg);
  *fragment_size = fragment_arg;

  return 0;
}

/*
 * opens the audio device(s). if in_audio_device_name and out_audio_device_name
 * are equal, full duplex mode is assumed
 *
 * input: in_audio_device_name, out_audio_device_name: strings
 *        channels: requestes number of channels (1 / 2)
 *        format_priorities: list of sorted integers with valid sound formats
 *                           (e.g. AFMT_U8). the first working one will be used
 *        speed_in, speed_out: requested speeds (equal for full duplex)
 *
 * returns:  0 if successful
 *          -1 if not successful
 *
 * output:
 *    audio_fd_in, audio_fd_out: file descriptors
 *    fragment_size_in, fragment_size_out: device buffer fragment sizes
 *    format_in, format_out: actually used formats
 *    speed_in, speed_out: actually used speeds
 */
int open_audio_devices(char *in_audio_device_name,
		       char *out_audio_device_name,
		       int channels, int *format_priorities,
		       int *audio_fd_in, int *audio_fd_out,
		       int *fragment_size_in, int *fragment_size_out,
		       int *format_in, int *format_out,
		       int *speed_in, int *speed_out) {
  int audio_fd;
  int initresult;
  int capabilities;
  int *priority;
  
  /* try to open the sound device */
  if (strcmp(in_audio_device_name, out_audio_device_name) == 0) {
    /* same device for input and output: full duplex */

    for (initresult = -1, priority = format_priorities;
	 priority  && initresult; priority++) { /* try different formats */
      if ((audio_fd = open(in_audio_device_name, O_RDWR | O_NONBLOCK, 0))
	  == -1) {
	perror(in_audio_device_name);
	return(-1);
      }
      /* device is in non-blocking mode now */

      *audio_fd_in = audio_fd;
      *audio_fd_out = audio_fd;
      
      /* set device to full duplex mode */
      if (ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, 0)) {
	perror("SNDCTL_DSP_SETDUPLEX");
	return(-1);
      }
      if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &capabilities)) {
	perror("SNDCTL_DSP_GETCAPS");
	return(-1);
      }
      if (!(capabilities & DSP_CAP_DUPLEX)) {
	fprintf(stderr, "Error: device doesn't support full duplex mode\n");
	return(-1);
      }
      
      /* *speed_in assumed to be equal to *speed_out */
      *format_in = *priority; /* get new format to try */
      initresult = init_audio_device(audio_fd, *format_in, channels, speed_in,
				     fragment_size_in);
      if (initresult) /* let's retry (and re-open) */
	close_audio_devices(*audio_fd_in, *audio_fd_out);
    }
    *format_out = *format_in;
    *speed_out = *speed_in;
    *fragment_size_out = *fragment_size_in;
    return initresult;
  } else {
    /* different devices for input and output */

    for (initresult = -1, priority = format_priorities;
	 initresult && priority; priority++) {
      if ((audio_fd = open(in_audio_device_name, O_RDONLY | O_NONBLOCK, 0)) ==
	  -1) {
	perror(in_audio_device_name);
	return(-1);
      }
      /* device is in non-blocking mode now */

      *audio_fd_in = audio_fd;
      *format_in = *priority;
      initresult = init_audio_device(audio_fd, *format_in, channels, speed_in,
				     fragment_size_in);
      if (initresult) /* let's retry (and re-open) */
	if (close(audio_fd)) {
	  perror("close audio in device");
	  return -1;
	}
    }

    if (initresult) return initresult; /* no chance */

    for (initresult = -1, priority = format_priorities;
	 initresult && priority; priority++) {
      if ((audio_fd = open(out_audio_device_name, O_WRONLY | O_NONBLOCK, 0))
	  == -1) {
	perror(out_audio_device_name);
	if (close(*audio_fd_in)) { /* error -> close audio_in device */
	  perror("close audio in device");
	}
	return(-1);
      }
      /* device is in non-blocking mode now */

      *audio_fd_out = audio_fd;
      *format_out = *priority;

      initresult = init_audio_device(audio_fd, *format_out, channels,
				     speed_out, fragment_size_out);
      if (initresult) /* let's retry (and re-open) */
	if (close(audio_fd)) {
	  perror("close audio out device");
	  return -1;
	}
    }
    return initresult;
  }
}

/*
 * stops audio playback and recording on specified file descriptors
 */
int audio_stop(int audio_fd_in, int audio_fd_out) {
  /* Instead of ioctl SNDCTL_DSP_RESET, OSS Programmer's Guide recommends
   * closing and reopening the devices (-> close/open_audio_devices).
   * Here, we flush the input buffer and use SNDCTL_DSP_RESET
   * to clean up for further recording, but on restart, old input comes again.
   */
  int result;
  int frag_size;
  char *buf; /* temporary allocated buffer */

  struct timeval tv;
  fd_set fds;
  int num; /* number of file descriptors with data (0/1) */

  /* flush input buffer */
  if (ioctl(audio_fd_in, SNDCTL_DSP_GETBLKSIZE, &frag_size))
    fprintf(stderr, "Error obtaining audio input fragment size "
	    "with SNDCTL_DSP_GETBLKSIZE in audio_stop().\n");
  else {
    buf = (char *) malloc(frag_size);
    
    tv.tv_sec = 0; /* return immediately */
    tv.tv_usec = 0;
    
    do {
      FD_ZERO(&fds);
      FD_SET(audio_fd_in, &fds);
      
      num = select(FD_SETSIZE, &fds, 0, 0, &tv); /* return immediately */
      if (num > 0)
	read(audio_fd_in, buf, frag_size);
    } while (num > 0);
    if (num == -1)
      perror("select at audio_stop");
    free(buf);
  }
  
  /* stop device */
  if (audio_fd_in == audio_fd_out)
    result = ioctl(audio_fd_in, SNDCTL_DSP_RESET, 0);
  else
    result = ioctl(audio_fd_in, SNDCTL_DSP_RESET, 0) |
      ioctl(audio_fd_out, SNDCTL_DSP_RESET, 0);

  return result;
}

/*
 * Closes specified input/output file descriptors
 * returns 0 on success, -1 on error
 */
int close_audio_devices(int audio_fd_in, int audio_fd_out) {
  audio_stop(audio_fd_in, audio_fd_out);

  if (close(audio_fd_in) == -1) {
    perror("close audio device");
    return -1;
  }
  if (audio_fd_in != audio_fd_out)
    if (close(audio_fd_out) == -1) {
      perror("close audio device");
      return -1;
    }
  return 0;
}

/*
 * returns number of bytes per sample for the specified
 * OSS format (e.g. AFMT_U8)
 *
 * returns >= 1 on success, 0 otherwise (when format not supported)
 */
int sample_size_from_format(int format) {
  switch(format) {
  case AFMT_U8:
  case AFMT_S8:
  case AFMT_MU_LAW:
    return 1;
    
  case AFMT_S16_LE:
  case AFMT_S16_BE:
  case AFMT_U16_LE:
  case AFMT_U16_BE:
    return 2;
   
  default:
    return 0;
  }
}

/*
 * returns the number of available fragents that can be read immediately
 * from specified sound device file descriptor fd
 */
int audio_get_read_fragments_number(int fd) {
  audio_buf_info info;
  
  if (ioctl(fd, SNDCTL_DSP_GETISPACE, &info) == -1) {
    perror("SNDCTL_DSP_GETISPACE");
  }
  return info.fragments;
}

/*
 * returns the total number of fragents available for OSS at
 * specified sound device file descriptor fd for reading
 */
int audio_get_read_fragments_total(int fd) {
  audio_buf_info info;
  
  if (ioctl(fd, SNDCTL_DSP_GETISPACE, &info) == -1) {
    perror("SNDCTL_DSP_GETISPACE");
  }
  return info.fragstotal;
}

/*
 * returns the number of fragents that can be written (non-blocking) to
 * specified sound device file descriptor fd
 */
int audio_get_write_fragments_number(int fd) {
  audio_buf_info info;
  
  if (ioctl(fd, SNDCTL_DSP_GETOSPACE, &info) == -1) {
    perror("SNDCTL_DSP_GETOSPACE");
  }
  return info.fragments;
}

/*
 * returns the total number of fragents available for OSS at
 * specified sound device file descriptor fd for writing
 */
int audio_get_write_fragments_total(int fd) {
  audio_buf_info info;
  
  if (ioctl(fd, SNDCTL_DSP_GETOSPACE, &info) == -1) {
    perror("SNDCTL_DSP_GETOSPACE");
  }
  return info.fragstotal;
}

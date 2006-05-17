/*
 * The sound effects generator
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

/* regular GNU system includes */
#include <stdio.h>
#include <math.h>

/* own header files */
#include "globals.h"
#include "session.h"

/* all time constants in seconds or Hz */
#define RING_PERIOD 4
#define RING_LENGTH 1.3
#define RING_FREQUENCY 1300
/* fade in/out reciprocal period: */
#define RING_FADE_LENGTH 0.003
#define RING_SHORT_PERIOD 0.044
#define RING_SHORT_LENGTH (RING_SHORT_PERIOD / 2)
#define RINGING_FREQUENCY 425

/*
 * Effect generator
 * input:   session: session, used element: audio_LUT_generate
 *          effect:  the effect
 *          index:   parameter for effect (e.g. which touchtone)
 *                     EFFECT_TOUCHTONE: row * 3 + column   (each 0-based)
 *          seconds: position of sample in seconds
 * returns: generated ulaw sample
 */
unsigned char fxgenerate(session_t *session, enum effect_t effect,
			 int index, double seconds) {
  unsigned char x; /* generated ulaw sample (return value)     */
  double rest;     /* monitor the period in seconds modulo 4   */
  double rest2;    /* monitor (short) sinus period             */
  double factor;   /* envelope factor                          */
  double factor2;  /* envelope help factor (short periods)     */
  double f1, f2;   /* frequencies for touchtones (row, column) */

  switch(effect) {
  case EFFECT_RING: /* somebody's calling */
  case EFFECT_TEST: /* play test sound */

    if (effect == EFFECT_RING) {
      rest = fmod(seconds, RING_PERIOD);               /* 4 sec period */
    } else {
      rest = seconds;                                  /* infinite period */
    }
    
    rest2 = fmod(seconds, RING_SHORT_PERIOD);          /* short period */
    
    if (rest < RING_FADE_LENGTH) { /* fade in */
      factor = -cos(rest * M_PI / RING_FADE_LENGTH) / 2 + 0.5;
    } else if (rest > RING_LENGTH - RING_FADE_LENGTH &&
	       rest < RING_LENGTH) { /* fade out */
      factor = -cos((RING_LENGTH - rest) * 2 * M_PI / (RING_FADE_LENGTH * 2))
	/ 2 + 0.5;
      
    } else if (rest >= RING_LENGTH) { /* pause */
      factor = 0;
    } else { /* (potential) beep */
      factor = 1;
    }
    
    if (rest2 > RING_SHORT_PERIOD - 0.5 * RING_FADE_LENGTH) {
      /* fade in short period (1/2) */
      factor2 = -sin((RING_SHORT_PERIOD - rest2)
		     * 2 * M_PI / (RING_FADE_LENGTH * 2)) / 2 + 0.5;
    } else if (rest2 < 0.5 * RING_FADE_LENGTH) {
      /* fade in short period (2/2) */
      factor2 = sin(rest2 * 2 * M_PI / (RING_FADE_LENGTH * 2)) / 2 + 0.5;
    } else if (rest2 > RING_SHORT_LENGTH - 0.5 * RING_FADE_LENGTH &&
	       rest2 < RING_SHORT_LENGTH + 0.5 * RING_FADE_LENGTH) {
      /* fade out short period */
      factor2 = -sin((rest2 - RING_SHORT_LENGTH)
		     * 2 * M_PI / (RING_FADE_LENGTH * 2)) / 2 + 0.5;
      
    } else if (rest2 <= RING_SHORT_LENGTH) { /* just beep */
      factor2 = 1;
    } else {
      factor2 = 0; /* short pause */
    }
    
    factor = factor * factor2;
    
    x = session->audio_LUT_generate[
	    (int)(sin(seconds * 2 * M_PI * RING_FREQUENCY)
		  * factor * 127.5 + 127.5)];
    break;
    
  case EFFECT_RINGING: /* waiting for the other end to pick up the phone */
    rest = fmod(seconds, 5);
    if (rest >= 2 && rest < 3) {
      x = session->audio_LUT_generate[ /* beep */
	   (int)(sin(seconds * 2 * M_PI * RINGING_FREQUENCY) * 127.5 + 127.5)];
    } else {
      x = session->audio_LUT_generate[128]; /* pause */
    }
    break;
  case EFFECT_TOUCHTONE: /* clicked key pad in conversation mode */
    switch (index / 3) { /* row frequency */
    case 0: f1 = 697; break;
    case 1: f1 = 770; break;
    case 2: f1 = 852; break;
    case 3: f1 = 941; break;
    default: f1 = 0;
    }
    switch (index % 3) { /* column frequency */
    case 0: f2 = 1209; break;
    case 1: f2 = 1336; break;
    case 2: f2 = 1477; break;
    case 3: f2 = 1633; break;
    default: f2 = 0;
    }
    x = session->audio_LUT_generate[
	  (int)((sin(seconds * 2 * M_PI * f1) + sin(seconds * 2 * M_PI * f2))
	  / 2 * 127.5 * 0.7 + 127.5)];
    break;
  default:
    fprintf(stderr, "fxgenerate: Unknown effect.\n");
    x = 0;
  }
  
  return x;
}

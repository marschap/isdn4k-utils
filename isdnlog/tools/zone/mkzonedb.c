/*
 * Make zone datafile
 *
 * Copyright 1999 by Leopold Toetsch <lt@toetsch.at>
 *
 * SYNOPSIS
 * mkzonedb -r Zonefile -d database -c Codef [-f] [-v] [-V] [-o Oz] [-l L]
 *
 * 1.23: w. USE_DESTINATION
 * mkzonedb -r Zonefile -d database [-f] [-v] [-V] [-o Oz] [-l L]
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

#ifdef USE_DESTINATION
static char progversion[] = "1.23";
#else
static char progversion[] = "1.11";
#endif

/*
 * Changes:
 *
 * 1.23 1999.10.12 lt moved /CC/code handling to destination
 *
 * 1.11 1999.07.08 lt added support for NL
 *
 *      in NL areacode may be shorter than actual areacodenumber
 *      in this case \tLEN is appended to text
 *
 */
 
#define STANDALONE

#define _MKZONEDB_C_

#include <limits.h>

#include <stdlib.h>
#include <stdio.h>
#ifdef __GLIBC__
# define __USE_GNU  /* for declaration of basename() */
#endif
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#if !defined(__GLIBC__) && !defined(basename)
extern const char *basename (const char *name);
#endif

#include "config.h"
#include "common.h"


void usage(char *argv[]) {
#ifdef USE_DESTINATION
	fprintf(stderr, "%s: -r RedZonefile -d Database [ -v ] [ -V ] [ -o Localzone ] [ -l Len ]\n",
#else	
	fprintf(stderr, "%s: -r RedZonefile -d Database -c Code [-a CC] [ -v ] [ -V ] [ -o Localzone ] [ -l Len ]\n",
#endif	
		basename(argv[0]));
	exit(EXIT_FAILURE);
}

static int (*zones)[3];
static int *numbers;
static bool verbose=false;
static int table[256];
static int tablelen, keylen, keydigs, maxnum;
static int n, nn;
#ifndef USE_DESTINATION
typedef struct {
	int num;
	char *code;
} code_t;
static code_t *codes = 0;
static int nc;
static int country=0;
#endif
static int ortszone=1;
static int numlen;

#ifndef USE_DESTINATION
static void read_codefile(char *cf) {
	FILE *fp;
	char *p;
	auto char line[BUFSIZ];

	nc = 0;
	if (verbose)
		printf("Reading %s\n", cf);
	if ((fp=fopen(cf, "r")) == 0) {
		fprintf(stderr, "Coudn't read '%s'\n", cf);
		exit(EXIT_FAILURE);
	}
	maxnum = 0;
	while (fgets(line, BUFSIZ, fp)) {
		if (verbose && (nc % 1000) == 0) {
			printf("%d\r", nc);
			fflush(stdout);
		}
		if (!isdigit(*line))
			break;
		line[strlen(line)-1] = '\0';
		if ((codes=realloc(codes, (nc+1)*sizeof(code_t))) == 0) {
			fprintf(stderr, "Out of mem\n");
			exit(EXIT_FAILURE);
		}
		codes[nc].num = strtoul(line, &p, 10);
		if (codes[nc].num > maxnum)
			maxnum = codes[nc].num;
		if (*p != '\t') {
			fprintf(stderr, "'%s' seems to be an invalide codfile\n", cf);
			exit(EXIT_FAILURE);
		}
		if ( (codes[nc].code = strdup(++p)) == 0) {
			fprintf(stderr, "Out of mem\n");
			exit(EXIT_FAILURE);
		}
		nc++;
	}
	if (verbose) {
		printf("%d Codes read\n", nc);
		fflush(stdout);
	}
	fclose(fp);
	/* we append a dummy for not defined nums */
	if ((codes=realloc(codes, (nc+1)*sizeof(code_t))) == 0) {
		fprintf(stderr, "Out of mem\n");
		exit(EXIT_FAILURE);
	}
	codes[nc].num=-1;
	codes[nc].code=strdup("");
	free(cf);
}
#endif

static void read_rzfile(char *rf) {
	int i;
	char line[BUFSIZ], *p, *op;
	FILE *fp;
	int from,to,z;

	if ((fp=fopen(rf, "r")) == 0) {
		fprintf(stderr, "Coudn't read '%s'\n", rf);
		exit(EXIT_FAILURE);
	}
#ifdef USE_DESTINATION
	maxnum = numlen==5 ? 40000 : 10000;
#endif		
	if ((numbers = calloc(maxnum+1, sizeof(int))) == 0) {
		fprintf(stderr, "Out of mem\n");
		exit(EXIT_FAILURE);
	}
	n=i=keylen=keydigs=0;
	if (verbose)
		printf("Reading %s\n", rf);

	while (fgets(line, BUFSIZ, fp)) {
		if (verbose && (n % 1000) == 0) {
			printf("%d\r", n);
			fflush(stdout);
		}
		if (!*line)
			break;
		if (strlen(line)>40)
			fprintf(stderr, "Possible junk in line %d", n);
		from = strtoul(line, &p, 10);
		if (p-line > keydigs)
			keydigs=p-line;
		p++;
		op = p;
		to = strtoul(p, &p, 10);
		if (p-op > keydigs)
			keydigs=p-op;
		p++;
		z = strtoul(p, &p, 10);
		if (z > 127) {
			fprintf(stderr, "Something is wrong with this file (line %d)\n", n);
			exit(EXIT_FAILURE);
		}
		if ((zones = realloc(zones, (n+1)*3*sizeof(int))) == 0) {
			fprintf(stderr, "Out of mem\n");
			exit(EXIT_FAILURE);
		}
		zones[n][0]=from;
		zones[n][1]=to;
		zones[n][2]=z;
		if (from > keylen)
			keylen=from;
		if (to > keylen)
			keylen=to;
#ifdef USE_DESTINATION
		if(to>maxnum) {
			maxnum=to;  
			if ((numbers = realloc(numbers,(maxnum+1)*sizeof(int))) == 0) {
				fprintf(stderr, "Out of mem\n");
				exit(EXIT_FAILURE);
			}
		}
#endif			
		numbers[to]++;
		n++;
	}
	free(rf);
#ifdef USE_DESTINATION
	maxnum=keylen<maxnum?keylen:maxnum;
#endif	
}

/* get the 256 top used nums in table */
static void make_table() {
	int i, j, k;
	tablelen = 0;
	if (verbose)
		printf("%d\nSorting\n", n);
	nn = maxnum;
	if (keylen < nn)
		nn = keylen;
	for (j=0; j<256; j++) {
		int max = 0;
		k = -1;
		for (i=0; i<=nn; i++) {
			if (numbers[i] > max) {
				k = i;
				max = numbers[i];
			}
		}
		if (k == -1) {
			nn = j;
			break;
		}
		numbers[k]=0;
		table[j] = k;
		if (k > tablelen)
			tablelen = k;
	}
	free(numbers);
	if (nn > 256)
		nn = 256;
}


#ifndef USE_DESTINATION

static int comp_func(const void *p1, const void *p2) {
	return ((code_t*)p1)->num - ((code_t*)p2)->num;
}

/* insert the code of from into value */
static void insert_code(datum *value, int ifrom) {
	int l;
	code_t *cp, key;
	key.num = ifrom;
	cp = bsearch(&key, codes, nc, sizeof(code_t), comp_func);
	if (!cp)
		cp = &codes[nc];

	l = strlen(cp->code) + 1;
	if ((value->dptr = realloc(value->dptr, value->dsize+l)) == 0) {
		fprintf(stderr, "Out of mem\n");
		exit(EXIT_FAILURE);
	}
	memmove(value->dptr+l, value->dptr, value->dsize);
	value->dsize += l;
	memcpy(value->dptr, cp->code, l);
	*cp->code = '\0'; /* invalidate */
	return;
}

static void write_remaining_codes(_DB db) {
	int i,l;
	UL kul;
	US kus;
	datum key, value;
	char *val;
	if (verbose)
		printf("Writing remaining\n");
	
	for (i=0 ; i< nc; i++)
		if (codes[i].num > 0 && *codes[i].code) {
			if (verbose && (i % 1000) == 0) {
				printf("%d\r", i);
				fflush(stdout);
			}
			
			if (keylen == 4) {
				kul = (UL)codes[i].num;
				key.dptr = (char*)&kul;
			}
			else {
				kus = (US)codes[i].num;
				key.dptr = (char*)&kus;
			}
			key.dsize = keylen;
			val = malloc(l=(strlen(codes[i].code)+3));
			strcpy(val, codes[i].code);
			val[l-1] = val[l-2] = '\0'; /* count=0 */
			value.dptr = val;
			value.dsize = l;
			if(STORE(db, key, value)) {
				fprintf(stderr, "Error storing key '%d' - %s\n",codes[i].num,GET_ERR);
				exit(EXIT_FAILURE); 
			}
			free(val);
		}
}
#endif

static void write_db(char * df) {
	_DB db;
	datum key, value;
	UL ul, kul;
	US us, kus;
	int ofrom;
	int vlen;
	US count;
	char *val, *p;
	int i, j;
	char version[80];

#ifndef USE_DESTINATION
	if (country)
		qsort(codes, nc, sizeof(code_t), comp_func);
#endif		
	if (verbose)
		printf("Writing\n");
	if((db=OPEN(df,WRITE)) == 0) {
		fprintf(stderr, "Can't create '%s' - %s\n", df, GET_ERR);
		exit(EXIT_FAILURE);
	}
	/* tablelen .. len of table entries */
	/* keylen = keysize */
	if (maxnum > keylen)
		keylen=maxnum;
	keylen = keylen > 0xffff ? 4 : 2;
	tablelen = tablelen > 0xffff ? 4 : tablelen > 0xff ? 2 : 1;

	/* write version & table */
	key.dptr = "vErSiO";
	key.dsize = 7;
	/* version of zone.c must be not smaller than dataversion */
#ifndef USE_DESTINATION
	sprintf(version,"V1.23 K%c C%c N%d T%d O%d L%d A%d",
		keylen==2?'S':'L',tablelen==1?'C':tablelen==2?'S':'L',
		nn,n, ortszone, numlen?numlen:keydigs, country);
#else		
	sprintf(version,"V1.20 K%c C%c N%d T%d O%d L%d",
		keylen==2?'S':'L',tablelen==1?'C':tablelen==2?'S':'L',
		nn,n, ortszone, numlen?numlen:keydigs);
#endif		
	value.dptr = version;
	value.dsize = strlen(version)+1;
	if(STORE(db, key, value)) {
		fprintf(stderr, "Error storing version key - %s\n", GET_ERR);
		exit(EXIT_FAILURE);
	}

	if ((p = val = calloc(nn, tablelen)) == 0) {
		fprintf(stderr, "Out of mem\n");
		exit(EXIT_FAILURE);
	}
	key.dptr = "_tAbLe";
	key.dsize = 7;
	for (i=0; i<nn; i++)
		if(tablelen==1)
			*p++ = (UC)table[i];
		else if(tablelen == 2)
			*((US*)p)++ = (US)table[i];
		else
			*((UL*)p)++ = (UL)table[i];
	value.dptr = val;
	value.dsize = nn*tablelen;
	if(STORE(db, key, value)) {
		fprintf(stderr, "Error storing table key - %s\n", GET_ERR);
		exit(EXIT_FAILURE);
	}
	free(val);

	/* and write data */
	val = malloc(2); /* size of count */
	vlen = 2;
	ofrom = -1;
	count = 0;
	for (i=0; i<n; i++) {
		bool found = false;
		UC uc;

		if (verbose && (i % 1000) == 0) {
			printf("%d\r", i);
			fflush(stdout);
		}
		if (ofrom != -1 && ofrom != zones[i][0]) {
		    *((US*)val) = count;
			value.dptr = val;
			value.dsize = vlen;
#ifndef USE_DESTINATION
			if (country)
				insert_code(&value, ofrom);
#endif				
			if(STORE(db, key, value)) {
				fprintf(stderr, "Error storing key '%d' - %s\n",ofrom,GET_ERR);
				exit(EXIT_FAILURE);
			}
			free(value.dptr); /* the realloced val */
		}
		if (ofrom != zones[i][0]) {
			count = 0;
			val = malloc(2); /* size */
			vlen = 2;
			/* set up key */
			ofrom = zones[i][0];
			if (keylen == 4) {
				kul = (UL)ofrom;
				key.dptr = (char*)&kul;
			}
			else {
				kus = (US)ofrom;
				key.dptr = (char*)&kus;
			}
			key.dsize = keylen;
		}
		count++;
		for (j=0; j<nn; j++)
			if(table[j] == zones[i][1]) {
				found = true;
				val = realloc(val, vlen+2);
				uc = (UC)zones[i][2];
				val[vlen++] = uc;
				uc = (UC)j;
				val[vlen++] = uc;
				break;
			}
		if (!found) {
			val = realloc(val, vlen+1+keylen);
			zones[i][2] |= 128;
			uc = (UC)zones[i][2];
			val[vlen++] = uc;
			if(keylen == 2) {
				us = (US)zones[i][1];
				*((US*)(&val[vlen])) = us;
			}
			else {
				ul = (UL)zones[i][1];
				*((UL*)(&val[vlen])) = ul;
			}
			vlen+=keylen;
		}
	}
	if(verbose)
		printf("%d\n", i);
	/* write last */
    *((US*)val) = count;
	value.dptr = val;
	value.dsize = vlen;
#ifndef USE_DESTINATION
	if (country)
		insert_code(&value, ofrom);
#endif		
	STORE(db, key, value);
	free(value.dptr);
#ifndef USE_DESTINATION
	if (country)
		write_remaining_codes(db);
#endif		
	CLOSE(db);
	free(zones);
	free(df);
#ifndef USE_DESTINATION
	for (i = 0; i <= nc; i++)
		free(codes[i].code);
	free(codes);
#endif	
}

int main (int argc, char *argv[])
{
	char *df=0;
	char *rf=0;
#ifndef USE_DESTINATION
	char *cf=0;
#endif	
	int c;

	if (argc < 2)
		usage(argv);
#ifdef USE_DESTINATION
	while ( (c=getopt(argc, argv, "vVr:d:o:l:")) != EOF) {
#else	
	while ( (c=getopt(argc, argv, "vVr:d:c:o:l:a:")) != EOF) {
#endif	
		switch (c) {
			case 'v' : verbose = true; break;
			case 'V' : printf("%s: V%s Db=%s\n",
				basename(argv[0]), progversion, dbv); exit(1);
			case 'd' : df = strdup(optarg); break;
			case 'r' : rf = strdup(optarg); break;
#ifndef USE_DESTINATION
			case 'c' : cf = strdup(optarg); break;
			case 'a' : country = atoi(optarg); break;
#endif			
			case 'o' : ortszone = atoi(optarg); break;
			case 'l' : numlen = atoi(optarg); break;
		}
	}
#ifndef USE_DESTINATION
	read_codefile(cf);
#endif	
	read_rzfile(rf);
	make_table();
	write_db(df);

	/* Uff this got longer as I thought,
	   C is a real low level language -
	   now it's clear, why I prefer Perl
	*/
	return(EXIT_SUCCESS);
}

/* $Id: isdnlog.c,v 1.12 1997/05/25 19:40:58 luethje Exp $
 *
 * ISDN accounting for isdn4linux. (log-module)
 *
 * Copyright 1995, 1997 by Andreas Kool (akool@Kool.f.EUnet.de)
 *                     and Stefan Luethje (luethje@sl-gw.lake.de)
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
 *
 * $Log: isdnlog.c,v $
 * Revision 1.12  1997/05/25 19:40:58  luethje
 * isdnlog:  close all files and open again after kill -HUP
 * isdnrep:  support vbox version 2.0
 * isdnconf: changes by Roderich Schupp <roderich@syntec.m.EUnet.de>
 * conffile: ignore spaces at the end of a line
 *
 * Revision 1.11  1997/05/09 23:30:47  luethje
 * isdnlog: new switch -O
 * isdnrep: new format %S
 * bugfix in handle_runfiles()
 *
 * Revision 1.10  1997/05/06 22:13:26  luethje
 * bugfixes in HTML-Code of the isdnrep
 *
 * Revision 1.9  1997/05/04 20:19:47  luethje
 * README completed
 * isdnrep finished
 * interval-bug fixed
 *
 * Revision 1.8  1997/04/17 20:09:32  luethje
 * patch of Ingo Schneider
 *
 * Revision 1.7  1997/04/08 21:56:48  luethje
 * Create the file isdn.conf
 * some bug fixes for pid and lock file
 * make the prefix of the code in `isdn.conf' variable
 *
 * Revision 1.6  1997/04/08 00:02:14  luethje
 * Bugfix: isdnlog is running again ;-)
 * isdnlog creates now a file like /var/lock/LCK..isdnctrl0
 * README completed
 * Added some values (countrycode, areacode, lock dir and lock file) to
 * the global menu
 *
 */

#define _ISDNLOG_C_

#include <linux/limits.h>

#include "isdnlog.h"
#ifdef POSTGRES
#include "postgres.h"
#endif

#define FD_SET_MAX(desc, set, max) { if (desc > max) max=desc; FD_SET(desc,set); }

/*****************************************************************************/

 /* Letzte Exit-Nummer: 47 */

/*****************************************************************************/

static void loop(void);
static void init_variables(int argc, char* argv[]);
static int  set_options(int argc, char* argv[]);
static void hup_handler(int isig);
static void exit_on_signal(int Sign);
int print_in_modules(const char *fmt, ...);
static int read_param_file(char *FileName);

/*****************************************************************************/

static char     usage[]   = "%s: usage: %s [ -%s ] file\n";
#ifdef Q931
static char     options[] = "av:sp:x:m:l:rt:c:C:w:SVTDPMh:nW:H:f:bL:NqFA:2:O:";
#else
static char     options[] = "av:sp:x:m:l:rt:c:C:w:SVTDPMh:nW:H:f:bL:NFA:2:O:";
#endif
static char     msg1[]    = "%s: Can't open %s (%s)\n";
static char    *ptty = NULL;
static section *opt_dat = NULL;

/*****************************************************************************/

static void exit_on_signal(int Sign)
{
  print_msg(PRT_NORMAL, "Got signal %d\n", Sign);

  Exit(7);
} /* exit_on_signal */

/*****************************************************************************/

static void hup_handler(int isig)
{
  print_msg(PRT_INFO, "re-reading %s\n", CONFFILE);

  discardconfig();
  if (readconfig(myname) != 0)
  	Exit(41);

	if (fprot != NULL && tmpout != NULL && *tmpout != '\0')
	{
		fclose(fprot);

		if ((fprot = fopen(tmpout,"a")) == NULL)
 	 	{
 	 		print_msg(PRT_ERR,"Can not open file `%s': %s!\n",tmpout, strerror(errno));
			Exit(46);
		}
	}

	if (fout != NULL && outfile != NULL && *outfile != '\0')
	{
		fclose(fout);

		if ((fout = fopen(outfile,"a")) == NULL)
 	 	{
 	 		print_msg(PRT_ERR,"Can not open file `%s': %s!\n",outfile, strerror(errno));
			Exit(47);
		}
	}

  signal(SIGHUP, hup_handler);
} /* hup_handler */

/*****************************************************************************/

static void loop(void)
{
  auto fd_set readmask;
  auto fd_set exceptmask;
  auto int    maxdesc = 0;
  auto int    Cnt;
  auto int    len;
  auto int    queuenumber;
  auto int    NewSocket;
  auto int    NewClient = 0;
  auto struct sockaddr incoming;
  auto int    Interval;


  if (xinfo) {
    first_descr++;

    if ((NewSocket = listening(port)) >= 0) {
      if (add_socket(&sockets, NewSocket))
	Exit(11);
    }
    else
      Exit(17);
  }
  else
    if (add_socket(&sockets, -1))
      Exit(11);

  while (1) {
    if (replay) {

      if (trace)
        dotrace();

      if (!morectrl(0))
        break;
    }
    else {
      FD_ZERO(&readmask);
      FD_ZERO(&exceptmask);

      queuenumber = socket_size(sockets);

      if (NewClient) {
        /* Damit sich der neue Client anmelden kann, ohne
           das was anderes dazwischen funkt ... */
        if (sockets[NewClient].descriptor >= 0)
	        FD_SET_MAX(sockets[NewClient].descriptor, &readmask, maxdesc);

      	NewClient = 0;
      }
      else {
        for (Cnt = 0; Cnt < queuenumber; Cnt++)
          if (sockets[Cnt].descriptor >= 0)
            FD_SET_MAX(sockets[Cnt].descriptor, &readmask, maxdesc);

        for (Cnt = first_descr; Cnt < queuenumber; Cnt++)
          if (sockets[Cnt].descriptor >= 0)
            FD_SET_MAX(sockets[Cnt].descriptor, &exceptmask, maxdesc);
      } /* else */

      if (newcps && ((ifo[0].u & ISDN_USAGE_MASK) + (ifo[1].u & ISDN_USAGE_MASK)))
      	Interval = wakeup; /* AK:06-Jun-96 */
      else
      	Interval = 0;

      while ((Cnt = select(maxdesc+1, &readmask, NULL, &exceptmask, Get_Interval(Interval))) < 0 && (errno == EINTR));

      if ((Cnt < 0) && (errno != EINTR)) { /* kill -HUP ausgeschlossen */
        print_msg(PRT_DEBUG_CS, "Error select: %s\n", strerror(errno));
        Exit(12);
      } /* if */

      processrate();

      if (!Cnt) /* Abbruch durch Timeout -> Programm starten */
        Start_Interval();

      now();

      for (Cnt = first_descr; Cnt < socket_size(sockets); Cnt++) {
        if (FD_ISSET(sockets[Cnt].descriptor, &exceptmask)) {
          if (sockets[Cnt].fp == NULL) {
            disconnect_client(Cnt);
            break;
          }
          else {
            int        event    = sockets[Cnt].call_event;
            int        cur_call = sockets[Cnt].chan;
            info_args *infoarg  = sockets[Cnt].info_arg;

            Ring(NULL, NULL, Cnt, 0);

            if (infoarg->flag & RING_LOOP && call[cur_call].cur_event == event)
              Start_Process(cur_call, infoarg, event);

            break;
          } /* else */
        }
        else if (FD_ISSET(sockets[Cnt].descriptor, &readmask))
          if (sockets[Cnt].fp == NULL) {
            eval_message(Cnt);
            /* Arbeite immer nur ein Client ab, du weisst nicht, ob der
               naechste noch lebt */
            break;
          }
          else
            Print_Cmd_Output(Cnt);
      } /* for */

      if (xinfo && FD_ISSET(sockets[IN_PORT].descriptor, &readmask)) {
        len = sizeof(incoming);

        if ((NewSocket = accept(sockets[IN_PORT].descriptor, &incoming, &len)) == -1)
          print_msg(PRT_DEBUG_CS, "Error accept: %s\n", strerror(errno));
        else {
          if (add_socket(&sockets, NewSocket))
            Exit(13);

      	  queuenumber = socket_size(sockets);

          sockets[queuenumber - 1].f_hostname = GetHostByAddr(&incoming);
          sockets[queuenumber - 1].waitstatus = WF_ACC;

          NewClient = queuenumber - 1;
        } /* else */
      }
      else if (FD_ISSET(sockets[ISDNINFO].descriptor, &readmask))
        moreinfo();
      else if (FD_ISSET(sockets[ISDNCTRL].descriptor, &readmask))
        (void)morectrl(0);
      else if (FD_ISSET(sockets[ISDNCTRL2].descriptor, &readmask))
        (void)morectrl(1);
    } /* else */
  } /* while */
} /* loop */

/*****************************************************************************/

int print_in_modules(const char *fmt, ...)
{
  auto va_list ap;
  auto char    String[LONG_STRING_SIZE];


  va_start(ap, fmt);
  (void)vsnprintf(String, LONG_STRING_SIZE, fmt, ap);
  va_end(ap);

  return print_msg(PRT_ERR, "%s", String);
} /* print_in_modules */

/*****************************************************************************/

static void init_variables(int argc, char* argv[])
{
  flog     = NULL;   /* /var/adm/isdn.log          */
  fcons    = NULL;   /* /dev/ttyX      (or stderr) */
  fprot    = NULL;   /* /tmp/isdnctrl0 	 	   */
  isdnctrl = NULL;
  fout     = NULL;

  first_descr = FIRST_DESCR;
  message = PRT_NORMAL | PRT_WARN | PRT_ERR | PRT_INFO;
  syslogmessage = PRT_NOTHING;
  xinfo = 0;
  sound = 0;
  trace = 0;
  isdaemon = 0;
  imon = 0;
  port = 0;
  wakeup = 1;
  fullhour = 0;
  tty_dv = 0;
  net_dv = 0;
  inf_dv = 0;
  settime = 0;
  replay = 0;
  replaydev = 0;
  verbose = 0;
  synclog = 0;
  any = 1;
  stdoutput = 0;
  allflags = 0;
  newcps = 0;
  chans = 2;
  hupctrl = 0;
  hup1 = hup2 = 0;
  bilingual = 0;
  mcalls = MAX_CALLS_IN_QUEUE;
  xlog = MAX_PRINTS_IN_QUEUE;
  outfile = NULL;

  sockets = NULL;
  known = NULL;

  opt_dat = NULL;
  newline = 1;
  width = 0;
  watchdog = 0;
  use_new_config = 1;
#ifdef Q931
  q931dmp = 0;
#endif
  CityWeekend = 0;

  sprintf(mlabel, "%%s%s  %%s%%s", "%e.%b %T %I");
  amtsholung = NULL;
  dual = 0;

  myname = argv[0];
  myshortname = basename(myname);
} /* init_variables */

/*****************************************************************************/

#ifdef Q931
static void traceoptions()
{
  q931dmp++;
  use_new_config = 0;
  replay++;
  port = 20012;
} /* traceoptions */
#endif

/*****************************************************************************/

int set_options(int argc, char* argv[])
{
  register int   c;
  register char *p;
  auto     int   defaultmsg = PRT_NORMAL | PRT_WARN | PRT_ERR | PRT_INFO;
  auto     int   newmessage = 0;


  if (!message)
    message = defaultmsg;

#ifdef Q931
  if (!strcmp(myshortname, "trace"))
    traceoptions();
#endif

  while ((c = getopt(argc, argv, options)) != EOF)
    switch (c) {
      case 'V' : print_version(myshortname);
      	       	 exit(0);
		 break;

      case 'v' : verbose = strtol(optarg, NIL, 0);
      	       	 break;

      case 's' : synclog++;
      	       	 break;

      case 'p' : port = strtol(optarg, NIL, 0);
      	       	 break;

      case 'm' : newmessage = strtol(optarg, NIL, 0);

      	       	 if (!newmessage) {
                   newmessage = defaultmsg;
                   printf("%s: WARNING: \"-m\" Option now requires numeric Argument\n", myshortname);
                 } /* if */
      	       	 break;

      case 'l' : syslogmessage = strtol(optarg, NIL, 0);

               	 if (!syslogmessage) {
                   syslogmessage = defaultmsg;
                   printf("%s: WARNING: \"-l\" Option requires numeric Argument\n", myshortname);
                 } /* if */
                 break;

      case 'x' : xinfo = strtol(optarg, NIL, 0);

               	 if (!xinfo)
                   xinfo = defaultmsg;
      	       	 break;

      case 'r' : replay++;
      	       	 break;

      case 't' : settime = strtol(optarg, NIL, 0); /* 1=once, 2=ever */
      	       	 break;

      case 'C' : ptty = strdup(optarg);
                 break;

      case 'M' : imon++;
      	       	 break;

      case 'S' : sound++;
      	       	 break;

      case 'w' : wakeup = strtol(optarg, NIL, 0);
      	       	 break;

      case 'D' : isdaemon++;
      	       	 break;

      case 'T' : trace++;
      	       	 break;

      case 'a' : any = 0;
      	       	 break;

      case 'P' : stdoutput = PRT_LOG;
      	       	 break;

      case 'h' : hupctrl++;

      	       	 if ((p = strchr(optarg, ':'))) {
                   *p = 0;
                   hup1 = atoi(optarg);
                   hup2 = atoi(p + 1);
      	       	 }
                 else
                   printf("%s: WARNING: \"-h\" Option requires two Arguments\n", myshortname);
      	       	 break;

      case 'b' : bilingual++;
      	       	 break;

      case 'c' : mcalls = strtol(optarg, NIL, 0);
      	       	 break;

      case 'L' : xlog = strtol(optarg, NIL, 0);
      	       	 break;

      case 'f' : read_param_file(optarg);
      	       	 break;

      case 'n' : newline = 0;
      	       	 break;

      case 'W' : width = strtol(optarg, NIL, 0);
      	       	 break;

      case 'H' : watchdog = strtol(optarg, NIL, 0);
      	       	 break;

      case 'N' : use_new_config = 0;
      	       	 break;

#ifdef Q931
      case 'q' : traceoptions();
      	       	 break;
#endif

      case 'F' : CityWeekend++;
      	       	 break;

      case 'A' : amtsholung = strdup(optarg);
      	       	 break;

      case '2' : dual = strtol(optarg, NIL, 0);
      	       	 break;

      case 'O' : outfile = strdup(optarg);
      	       	 break;

      case '?' : printf(usage, myshortname, myshortname, options);
	         exit(1);
    } /* switch */

  if (newmessage)
    message = newmessage;

  if (trace && isdaemon) {
    printf("%s","Can not trace daemonized!\n");
    exit(20);
  } /* if */

  if (stdoutput && isdaemon) {
    printf("%s","Can not write to stdout as daemon!\n");
    exit(21);
  } /* if */

  if (isdaemon) {
    if (syslogmessage == -1)
      syslogmessage = defaultmsg;

    switch (fork()) {
      case -1 : print_msg(PRT_ERR,"%s","Can not fork()!\n");
                Exit(18);
                break;
      case 0  : break;

      default : _exit(0);
    }

    /* Wenn message nicht explixit gesetzt wurde, dann gibt es beim daemon auch
       kein Output auf der Console/ttyx                                   */

    if (!newmessage && ptty == NULL)
      message = 0;
  } /* if */

  allflags = syslogmessage | message | xinfo | stdoutput;

  return optind;
} /* set_options */

/*****************************************************************************/

static int read_param_file(char *FileName)
{
  register char    *p;
  auto 	   section *SPtr;
  auto 	   entry   *Ptr;


	if (opt_dat != NULL)
	{
    print_msg(PRT_ERR, "Only one option file allowed (file %s)!\n", FileName);
		return -1;
	}

	if ((SPtr = opt_dat = read_file(NULL, FileName, 0)) == NULL)
		return -1;

	while (SPtr != NULL)
	{
		if (!strcmp(SPtr->name,CONF_SEC_OPT))
		{
			Ptr = SPtr->entries;

			while (Ptr != NULL)
			{
				if (!strcmp(Ptr->name,CONF_ENT_DEV))
					isdnctrl = Ptr->value;
				else
				if (!strcmp(Ptr->name,CONF_ENT_LOG))
					verbose = (int)strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_FLUSH))
					synclog = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_PORT))
					port = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_STDOUT))
					message = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_SYSLOG))
					syslogmessage = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_XISDN))
				{
					xinfo = strtol(Ptr->value, NIL, 0);
				}
				else
				if (!strcmp(Ptr->name,CONF_ENT_TIME))
					settime = (int)strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_CON))
					ptty = Ptr->value;
				else
				if (!strcmp(Ptr->name,CONF_ENT_START))
					sound = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_THRU))
					wakeup = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_DAEMON))
					isdaemon = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_MON))
					imon = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_PIPE))
					stdoutput = toupper(*(Ptr->value)) == 'Y'?PRT_LOG:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_MON))
					imon = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_HANGUP)) {
				  hupctrl++;
      	       	 		  if ((p = strchr(Ptr->value, ':'))) {
                 		    *p = 0;
                 		    hup1 = atoi(Ptr->value);
                 		    hup2 = atoi(p + 1);
      	       	 		  } /* if */
				}
				else
				if (!strcmp(Ptr->name,CONF_ENT_BI))
					bilingual = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_CALLS))
					mcalls = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_XLOG))
					xlog = strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_NL))
					newline = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_WIDTH))
					width = (int)strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_CW))
					CityWeekend = toupper(*(Ptr->value)) == 'Y'?1:0;
				else
				if (!strcmp(Ptr->name,CONF_ENT_DUAL))
					dual = (int)strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_AMT))
                                       amtsholung = strdup(Ptr->value);
				else
#ifdef Q931
				if (!strcmp(Ptr->name,CONF_ENT_Q931))
				{
					if(toupper(*(Ptr->value)) == 'Y')
						traceoptions();
				}
				else
#endif
				if (!strcmp(Ptr->name,CONF_ENT_WD))
					watchdog = (int)strtol(Ptr->value, NIL, 0);
				else
				if (!strcmp(Ptr->name,CONF_ENT_OUTFILE))
					outfile = Ptr->value;
				else
					print_msg(PRT_ERR,"Error: Invalid entry `%s'!\n",Ptr->name);

				Ptr = Ptr->next;
			}
		}
		else
			print_msg(PRT_ERR,"Error: Invalid section `%s'!\n",SPtr->name);

		SPtr = SPtr->next;
	}

	return 0;
}

/*****************************************************************************/

static void restoreCharge()
{
  register int   i, j, n = 0, nx = max(knowns, 1);
  auto 	   FILE *f;
  auto 	   char  fn[BUFSIZ];

  typedef struct {
    char    who[16];
    int	    day;
    double  charge;
    double  scharge;
    int	    month;
    double  online;
    double  sonline;
    double  bytes;
    double  sbytes;
  } CHARGE;

  auto     CHARGE *charge;



  if ((charge = (CHARGE *)malloc(sizeof(CHARGE) * nx)) != (CHARGE *)NULL) {

    sprintf(fn, "%s/%s", confdir(), CHARGEFILE);

    if ((f = fopen(fn, "r")) != (FILE *)NULL) {

      while (fscanf(f, "%s %d %lg %lg %d %lg %lg %lg %lg\n",
        charge[n].who, &charge[n].day, &charge[n].charge, &charge[n].scharge,
        &charge[n].month, &charge[n].online, &charge[n].sonline,
        &charge[n].bytes, &charge[n].sbytes) == 9) {

        n++;

        if (n == nx) {
          nx++;

  	  if ((charge = (CHARGE *)realloc((void *)charge, sizeof(CHARGE) * nx)) == (CHARGE *)NULL) {
      	    fclose(f);
            return;
          } /* if */
        } /* if */

      } /* while */

      if (n) {
        for (i = 0; i < knowns; i++)
          for (j = 0; j < n; j++)
            if (!strcmp(known[i]->who, charge[j].who)) {
      	      known[i]->day     = charge[j].day;
      	      known[i]->charge  = charge[j].charge;
      	      known[i]->scharge = charge[j].scharge;
      	      known[i]->month	= charge[j].month;
      	      known[i]->online	= charge[j].online;
      	      known[i]->sonline	= charge[j].sonline;
      	      known[i]->bytes	= charge[j].bytes;
      	      known[i]->sbytes	= charge[j].sbytes;

              break;
            } /* if */
      } /* if */

      fclose(f);
    } /* if */

    free(charge);
  } /* if */
} /* restoreCharge */

/*****************************************************************************/

int main(int argc, char *argv[], char *envp[])
{
  register char  *p;
  register int 	  i, res = 0;
  auto 	   int    lastarg;
	auto     char   rlogfile[PATH_MAX];
	auto     char **devices = NULL;
#ifdef TESTCENTER
  extern   void   test_center(char*);
#endif


  if (getuid() != 0) {
    fprintf(stderr,"Can only run as root!\n");
    exit(35);
  } /* if */

  set_print_fct_for_lib(print_in_modules);
  init_variables(argc, argv);

  lastarg = set_options(argc,argv);

#ifdef TESTCENTER
    test_center(argv[lastarg]);
#endif

	if (outfile != NULL)
	{
		if (!message)
		{
 	   print_msg(PRT_ERR,"Can not set outfile `%s', when -m is not set!\n",outfile);
 	   Exit(44);
		}
		else
		{
			if ((fout = fopen(outfile,"w")) == NULL)
			{
 	 			print_msg(PRT_ERR,"Can not open file `%s': %s!\n",outfile, strerror(errno));
 	  		Exit(45);
			}
		}
	}

  if (lastarg < argc)
    isdnctrl = argv[lastarg];

  if (isdnctrl == NULL) {
    print_msg(PRT_ERR,"There is no valid input device!\n");
    Exit(31);
  }
  else {
    p = strrchr(isdnctrl, C_SLASH);

    if (add_socket(&sockets, -1) ||  /* reserviert fuer isdnctrl */
        add_socket(&sockets, -1) ||  /* reserviert fuer isdnctrl2 */
        add_socket(&sockets, -1)   ) /* reserviert fuer isdninfo */
      Exit(19);

    if (replay) {
      verbose = 0;
      synclog = 0;
      settime = 0;
      xinfo = 0;
      *isdnctrl2 = 0;
    }
    else {
      if (strcmp(isdnctrl, "-") && dual) {
        strcpy(isdnctrl2, isdnctrl);
      	p = strrchr(isdnctrl2, 0) - 1;

        if (!isdigit(*p)) /* "/dev/isdnctrl" */
          strcat(++p, "0");

        if (isdigit(*(p - 1)))
          p--;

        i = atoi(p);

        sprintf(p, "%d", i + 2);
      }
      else
        *isdnctrl2 = 0;

    } /* else */


    openlog(myshortname, LOG_NDELAY, LOG_DAEMON);

    if (xinfo && read_user_access())
      Exit(22);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP,  hup_handler);
    signal(SIGINT,  exit_on_signal);
    signal(SIGTERM, exit_on_signal);
    signal(SIGALRM, exit_on_signal);
    signal(SIGQUIT, exit_on_signal);
    signal(SIGILL,  exit_on_signal);

    if (!(allflags & PRT_DEBUG_GENERAL))
      signal(SIGSEGV, exit_on_signal);

    sockets[ISDNCTRL].descriptor = !strcmp(isdnctrl, "-") ? fileno(stdin) : open(isdnctrl, O_RDONLY | O_NONBLOCK);
    if (*isdnctrl2)
      sockets[ISDNCTRL2].descriptor = open(isdnctrl2, O_RDONLY | O_NONBLOCK);

    if ((sockets[ISDNCTRL].descriptor >= 0) && (!*isdnctrl2 || (sockets[ISDNCTRL2].descriptor >= 0))) {

      if (ptty != NULL)
        fcons = fopen(ptty, "a");

      if ((ptty == NULL) || (fcons != (FILE *)NULL)) {
        if (verbose) {
          if ((p = strrchr(isdnctrl, '/')))
            p++;
          else
            p = argv[lastarg];

          sprintf(tmpout, "%s/%s", TMPDIR, p);
        } /* if */

        if (!verbose || ((fprot = fopen(tmpout, "a")) != (FILE *)NULL)) {

          for (i = 0; i < MAXCHAN; i++)
	    clearchan(i, 1);

#ifdef Q931
          if (q931dmp) {
  	    mymsns         = 3;
  	    mycountry      = "+49";
  	    myarea   	     = "6408";
        currency   	   = NULL;
        dual	         = 1;
  	    chargemax  	   = 0.0;
  	    connectmax 	   = 0.0;
  	    bytemax        = 0.0;
  	    connectmaxmode = 0;
  	    bytemaxmode    = 0;
  	    knowns     	   = retnum = 0;
  	    known      	   = (KNOWN **)NULL;
  	    start_procs.infoargs = NULL;
  	    start_procs.flags    = 0;

	    setDefaults();
          }
          else
#endif
          {
            if (readconfig(myshortname) < 0)
              Exit(30);

            restoreCharge();
          } /* if */

    			if (replay)
					{
						sprintf(rlogfile, "%s.rep", logfile);
			      logfile = rlogfile;
					}
					else
          {
          	append_element(&devices,isdnctrl);

            switch (i = handle_runfiles(myshortname,devices,START_PROG)) {
              case  0 : break;

              case -1 : print_msg(PRT_ERR,"Can not open pid/lock file: %s!\n", strerror(errno));
            	          Exit(36);

              default : Exit(37);
            } /* switch */
          } /* if */

          if (replay || ((sockets[ISDNINFO].descriptor = open(INFO, O_RDONLY | O_NONBLOCK)) >= 0)) {

            now();

#ifdef Q931
      	    if (!q931dmp)
#endif
    	      print_msg(PRT_NORMAL, "%s Version %s loaded\n", myshortname, VERSION);

#ifdef POSTGRES
            dbOpen();
#endif
            loop();

            if (sockets[ISDNINFO].descriptor >= 0)
              close(sockets[ISDNINFO].descriptor);
	  }
          else {
            print_msg(PRT_ERR, msg1, myshortname, INFO, strerror(errno));
            res = 7;
          } /* else */

          if (verbose)
            fclose(fprot);
        }
        else {
          print_msg(PRT_ERR, msg1, myshortname, tmpout, strerror(errno));
          res = 5;
        } /* else */

        if (ptty != NULL)
          fclose(fcons);
      }
      else {
        print_msg(PRT_ERR, msg1, myshortname, ptty, strerror(errno));
        res = 3;
      } /* else */

      if (strcmp(isdnctrl, "-"))
        close(sockets[ISDNCTRL].descriptor);
      if (*isdnctrl2)
        close(sockets[ISDNCTRL2].descriptor);
    }
    else {
      print_msg(PRT_ERR, msg1, myshortname, isdnctrl, strerror(errno));
      res = 2;
    } /* else */

#ifdef Q931
    if (!q931dmp)
#endif
      print_msg(PRT_NORMAL, "%s Version %s exiting\n", myshortname, VERSION);

  } /* else */

  Exit(res);
  return(res);
} /* main */

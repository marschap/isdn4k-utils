AUTOMAKE_OPTIONS=foreign no-dependencies
bin_SCRIPTS=vboxmail vboxplay
EXTRA_DIST= vboxmail.in vboxplay.in
MAINTAINERCLEANFILES=Makefile.in 
DISTCLEANFILES=vboxmail vboxplay

%: %.in
	MANDATE=`grep CHECKIN $< | awk '{print $4}'`    
	sed \
		-e "s#\@MANDATE\@#${MANDATE}#g" \
		-e "s#\@I4LVERSION\@#@VERSION@#g" \
        	-e "s#\@BINDIR\@#@bindir@#g" \
        	-e "s#\@SBINDIR\@#@sbindir@#g" \
        	-e "s#\@VERSION\@#@VERSION@#g" \
        	-e "s#\@VERDATE\@#@VERDATE@#g" \
        	-e "s#\@PACKAGE\@#@PACKAGE@#g" \
        	-e "s#\@SPOOLDIR\@#@SPOOLDIR@#g" \
        	-e "s#\@I4LCONFDIR\@#@I4LCONFDIR@#g" \
        	-e "s#\@LOGFILEDIR\@#@LOGFILEDIR@#g" \
        	-e "s#\@PIDFILEDIR\@#@RUNDIR@#g" \
        	-e "s#\@LCKFILEDIR\@#@LOCKDIR@#g" \
        	-e "s#\@MAILCMD\@#/usr/bin/mail#g" \
		< $< > $@

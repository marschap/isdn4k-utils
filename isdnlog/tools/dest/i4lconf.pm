=pod
$Id: i4lconf.pm,v 1.1 2003/07/25 21:23:15 tobiasb Exp $
Copyright 2002..2003 Tobias Becker <i4l-projects@talypso.de>
License terms of isdnlog apply.

locate_countryfile() returns the location of the system's countryfile.
This file is used by pp_rate for the preparation of rate-files und by
makedest for the generation of the destination database.  Usually the
location is specified with the COUNTRYFILE keyword in the ISDNLOG
section of /etc/isdn/isdn.conf.  If this fails, locate_countryfile()
will look for a file named country.dat or country-de.dat in some
usual directories.  If this fails too, an empty string will be returned.
Please note, that locate_countryfile() does not prove whether the
returned file is a valid countryfile or not.
=cut

package i4lconf;
use strict;
use vars qw($VERSION @ISA @EXPORT);
$VERSION=1.01;
require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(locate_countryfile);

sub locate_countryfile(;$) {
    my $countryfile = "";

    # Step 0 - check filename from caller if submitted
    if ( defined($_[0]) ) {
        $countryfile = $_[0];
        if ( -e $countryfile and -f _ and -r _ and -s _ ) {
            return $countryfile;
        }
    }
    
    # Step 1 - /etc/isdn/isdn.conf
    if ( open(CONF, "</etc/isdn/isdn.conf") ) {
        my ($line, $section, $keyword, $content);
	while ($line = <CONF>) {
	    chomp $line;
	    $line =~ s/(\s*[^\\]|^)\#.*//;
	    next unless ($line);
	    if ( $line =~ /\s*\[([^\]]+)\]/ ) {
	        $section = uc($1);
		next;
            }
	    next unless ($section eq "ISDNLOG");
	    if ( $line =~ /^\s*(\S+)\s*=\s*(.*)\s*$/ ) {
	        $keyword = uc($1);
		$content = $2;
		if ($keyword eq "COUNTRYFILE") {
		    $countryfile = $content;
		    last;
		}
	    }
 	}
	close(CONF);
    }
    if ( -e $countryfile and -f _ and -r _ and -s _ ) {
        return $countryfile;
    }
    
    # step 2 - searching
    $countryfile = "";
    my @dirs = ( '/usr/lib/isdn',	# default installation
                 '/usr/share/isdn',     # seen on debian
		 '/usr/local/lib/isdn', # my choose
		 '../..'                # the isdnlog directory of the i4l-CVS
	       );
    my @names = ( 'country.dat',	# canonical name since 2000
                  'country-de.dat'      # former name, still used in source
		);  
    my ($name, $dir);
    foreach $dir (@dirs) {
        foreach $name (@names) {
	    $countryfile = "$dir/$name";
            if ( -e $countryfile and -f _ and -r _ and -s _ ) {
                return $countryfile;
            }
	}    
    }

    # nothing found at all
    $countryfile = "";
    return $countryfile;
}					# end of sub locate_countryfile

1;

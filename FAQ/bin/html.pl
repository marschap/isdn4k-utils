#!/usr/bin/perl
#
# Creates HTML version from Latin-1 FAQ
#
# (c) 1996 Volker Götz <volker@oops.franken.de> (V 1.0)
# modified 08-Nov-96 Bernhard Hailer <dl4mhk@lrz.uni-muenchen.de>
# modified 11-Nov-96 Matthias Heßler <hessler@wi-inf.uni-essen.de>
# 
# $Id: html.pl,v 1.3 1997/02/23 01:20:55 hessler Exp $


# 
# variables
#

$maincount = 0;
$subcount  = 0;
$qcount    = 0;

$state     = "title";
$buffer    = "-";

$script    = "FALSE";


#
# Get file names from prompt
#

$in = $ARGV[0];
die "Usage: GenHTML.pl infile [outfile] [headtempfile] [tailtempfile]\n" unless length ($in) gt 0;

$out = $ARGV[1];
$out = "$in.html" unless length ($out) gt 0;

$head = $ARGV[2];
$head = "head.tmp" unless length ($head) gt 0;

$tail = $ARGV[3];
$tail = "tail.tmp" unless length ($tail) gt 0;


#
# Open files
#

open(IN,"<$in") || die "Can not open $in for input!\n";

open(TAIL,">$tail") || die "Can not open $tail for output!\n";
open(HEAD,">$head") || die "Can not open $head for output!\n";


#
# Create FAQ
#

print ("Creating html temp files...\n");

print HEAD "<HTML>\n";

while($line = <IN>) {

    chop($line);                               # kill \n character (newline)
    $line = &latin2html ($line);               # convert Umlaut's

    if ($state eq "title") {                   # pre text of FAQ
	print HEAD "<HEAD>\n";
	print HEAD "<TITLE>$inputfile</TITLE>\n";
	print HEAD "</HEAD>\n\n";
	print HEAD "<BODY>\n";
	print HEAD "<PRE>\n";
	print HEAD "$line\n";
	$state = "begin";
    } 

    elsif ($line =~ /^!1/) {                   # main headline
	if ($state eq "begin") {
	    print HEAD "</PRE>\n\n<HR>\n<P>\n\n<H1>$'</H1>\n\n";
	    print TAIL "</PRE>\n\n<HR>\n<P>\n\n<H1>$'</H1>\n\n";
	    $state = "!1";
	}
	else { 
	    print "ERROR: too many !1 or !1 improper located:\n";
	    print "$line\n";
	}
    }      
    
    elsif ($state eq "begin") {                # body of FAQ begins
	print HEAD "$line\n";
    }
    
    elsif ($line =~ /^!old/) { 
        # DUMMY; sonst erscheint das !old in der HTML-Version.
        # Hier später die !old-Auswertung einfügen.
    }

    elsif ($line =~ /^!2/) {                   # bigger headlines
        $maincount++;
        $subcount = 0;
        $qcount   = 0;
	if ($state eq "!1") {                  # first headline needs no </OL>
	    print HEAD "\n\n<H2>$maincount $'</H2>\n\n<OL>\n";
	    print TAIL "\n\n<H2>$maincount $'</H2>\n\n<OL>\n";
	}
	else {
	    print HEAD "</OL>\n<P>\n\n<H2>$maincount $'</H2>\n\n<OL>\n";
	    print TAIL "</OL>\n<P>\n<BR><BR>\n<H2>$maincount $'</H2>\n\n<OL>\n";
	}
	$state    = "!2";
    }
    
    elsif ($line =~ /^!3/) {                   # smaller headlines
	$subcount++;
        $qcount   = 0;
	print HEAD "</OL>\n<P>\n\n<H3>$maincount.$subcount $'</H3>\n\n<OL>\n";
	print TAIL "</OL>\n<P>\n<BR><BR>\n<H3>$maincount.$subcount $'</H3>\n\n<OL>\n";
	$state = "!3";
    } 
    
    elsif ($line =~ /^Q: /) {                  # questions
	
	# index part
	$qcount++;
	if ($state eq "question") {
	    print HEAD "$'\n";                 # first line was already printed
	    print TAIL "$'\n";
	}
	else {
	    print HEAD "<LI><A HREF=\"#$maincount.$subcount.$qcount\">$'\n";
	    print TAIL "<BR><BR>\n" unless $maincount == 0;
	    print TAIL "<A NAME=\"$maincount.$subcount.$qcount\"></A>\n";
	    print TAIL "<LI><B>$'\n";
	}
	$state = "question";
    } 

    elsif ($line =~ /^A: /) {                  # answers
	if ($state eq "question") {
	    print HEAD "</A>\n";               # stop link in question line
	    print TAIL "</B>\n<BR>\n<BR>\n\n$'\n";
	    $state = "answer";
	}
    } 

    elsif ($line =~ /^!verbon/) {              # script mode on
	if ($script eq "TRUE") {
	    print "ERROR: !verbon is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "$line\n";
        }
	else {
	    $script = "TRUE"; 
	    print TAIL "<PRE>\n";
	}
    }

    elsif ($line =~ /^!verboff/) {             # script mode off
	if ($script eq "TRUE") {
	    $script = "FALSE";
	    print TAIL "</PRE><BR>\n";
	}
	else {
	    print "ERROR: missing !verbon ($state $maincount.$subcount.$qcount):\n"; 
            print "$line\n";
	}
    }
    
    elsif ($line =~ /^!commenton/) {           # comment mode on
	if ($state eq "comment") {
	    print "ERROR: !commenton is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "$line\n";
        }
	else {
	    $buffer = $state;
	    $state = "comment";
	    print TAIL "<P>\n";
	}
    }

    elsif ($line =~ /^!commentoff/) {          # comment mode off
	if ($state eq "comment") {
	    $state = $buffer;
	}
	else {
	    print "ERROR: missing !commenton ($state $maincount.$subcount.$qcount):\n"; 
            print "$line\n";
	}
    }
    
    elsif ($state eq "question") {             # do while in question mode
	print HEAD "$line\n";
	print TAIL "$line\n";
    }
    
    elsif ($state eq "answer") {               # do while in answer mode
	if (length ($line) > 0) {
	    print TAIL "$line\n";
	}
	else {
	    print TAIL "<P>\n";
	    # $old = 0;
	}
    }
    
    elsif ($script eq "TRUE") {
	if ($state eq "question") {
	    print HEAD "$line\n";
	    print TAIL "$line\n";
	}	    
	elsif ($state eq "answer") {
	    print TAIL "$line\n";
	}
    }

    elsif ($state eq "comment") {
	print TAIL "$line\n";
    }

    else {
	print TAIL "$line\n";
    }
}
		
print HEAD "</OL>\n";
print TAIL "</OL>\n";
print TAIL "<HR>\n";
print TAIL "&copy; 1995,1996 ";
print TAIL "<A HREF=\"mailto:dl4mhk\@lrz.uni-muenchen.de?cc=hessler\@wi-inf.uni-essen.de\?subject=i4l-faq\">Das FAQ-Team</A>\n";
print TAIL "</BODY>\n</HTML>\n";


#
# Close files
#

close (IN);
close (HEAD);
close (TAIL);


#
# Write output file (concat head and tail)
#

print ("Writing html file...\n");

open(OUT,">$out") || die "Can not open $out for output!\n";

open(HEAD,"<$head") || die "Can not reopen $head for input!\n";
while (<HEAD>) { print OUT; }
close (HEAD);

open(TAIL,"<$tail") || die "Can not reopen $tail for input!\n";
while (<TAIL>) { print OUT; }
close (TAIL);

close (OUT);


#
# Cleanup
#

unlink ($head) || die "Could not delete temp file $head!\n";
unlink ($tail) || die "Could not delete temp file $head!\n";


#
# subroutines
#

sub latin2html
{
    local ($line) = @_;

    # native codes
    $line =~ s/&/&amp;/g;
    $line =~ s/</&lt;/g;
    $line =~ s/>/&gt;/g;

    $line =~ s/ä/&auml;/g;
    $line =~ s/ö/&ouml;/g;
    $line =~ s/ü/&uuml;/g;
    $line =~ s/Ä/&Auml;/g;
    $line =~ s/Ö/&Ouml;/g;
    $line =~ s/Ü/&Uuml;/g;
    $line =~ s/ß/&szlig;/g;

    # special codes
    $line =~ s|!br|<BR>|g;
    while ($line =~ /!link /) {
	my $pos1= 0;
	my $pos2= 0;
	my $url= "";

	$pos1= index($line, "!link ");
	$pos2= index($line, ";", $pos1);
	if ($pos2 == -1) {
	    $pos2= length($line);
	}
	$url= substr($line, $pos1+length("!link "),
		     $pos2-$pos1-length("!link "));
	$line = substr($line, 0, $pos1)
	    . "<A HREF=\"$url\" TARGET=_top>$url</A>"
		. substr($line, $pos2+1);
    }

    # Hervorhebungen
    $line =~ s|!b1|<B>|g;
    $line =~ s|!b0|</B>|g;
    $line =~ s|!i1|<I>|g;
    $line =~ s|!i0|</I>|g;
    $line =~ s|!u1|<U>|g;
    $line =~ s|!u0|</U>|g;

    return $line;
}

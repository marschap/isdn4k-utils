#!/usr/bin/perl
#
# Marks all Questions in FAQ as old.
#
# (c) 1996 Matthias Heﬂler <hessler@wi-inf.uni-essen.de>
# 
# $Id: old.pl,v 1.1 1997/02/19 19:54:52 hailer Exp $

#
# Get file names from prompt
#

$in = $ARGV[0];
die "Usage: old.pl infile [outfile]\n" unless length ($in) gt 0;

$out = $ARGV[1];
$out = "$in" unless length ($out) gt 0;


#
# Open files
#

if ("$in" eq "$out") {
    if( -e "$in.orig") {
	unlink( "$in.orig") || die "Can not remove $in.orig!\n";
    }
    rename( "$in", "$in.orig") || die "Can not rename $in to $in.orig!\n";
    $in .= ".orig";
}

open(IN,"<$in") || die "Can not open $in for input!\n";
open(OUT,">$out") || die "Can not open $out for output!\n";


#
# Updating FAQ
#

print "Marking all FAQ questions as old...\n";

$old = 0;

while($line = <IN>) {

    chop($line);  # kill \n character (newline)

    if ($line =~ /^!old/) {     # remove all "!old"
	# Do not output line...
    }
    
    elsif ($line =~ /^Q: /) {      # questions
	print OUT "!old\n";

	print OUT "$line\n";
    } 

    else {
	print OUT "$line\n";
    } 

}
		

#
# Close files
#

close (IN);
close (OUT);


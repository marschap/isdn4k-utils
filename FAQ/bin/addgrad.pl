#!/usr/bin/perl
#
# $Id: addgrad.pl,v 1.1 1997/02/19 19:54:51 hailer Exp $

($infile, $outfile) = @ARGV;

$infile = "i4l-faq.diff" until $infile;
$outfile = "$infile.ext" until $outfile;

open(IN,"<$infile") || die "cannot open $infile";
open(OUT,">$outfile") || die "cannot open $outfile"; 

while(<IN>) {

    if (/^\+/) {
        if ( /^\+\+/ ) {
        print OUT "$_"; 
        }
        else {            
            print OUT "+°";
            print OUT "$'";
        }
    }
    else {
       print OUT "$_";
    }
}

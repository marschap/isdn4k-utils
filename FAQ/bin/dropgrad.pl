#!/usr/bin/perl
#
# $Id: dropgrad.pl,v 1.1 1997/02/19 19:54:51 hailer Exp $

($infile, $outfile) = @ARGV;

$infile = "de-i4l-faq" until $inputfile;
$outfile = "$infile.out" until $outfile;

open(IN,"<$infile") || die "cannot open $infile";
open(OUT,">$outfile") || die "cannot open $outfile"; 

while(<IN>) {

    if (/^\°/) {
        print OUT "$'";
    }
    else {
       print OUT "$_";
    }
}

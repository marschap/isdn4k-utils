#!/usr/bin/perl
#
# Creates 7 bit ASCII text from Latin-1 FAQ
#
# (c) 1996 Volker Götz <volker@oops.franken.de>
# modified 1996/1997 by
#   Bernhard Hailer <dl4mhk@lrz.uni-muenchen.de>
#   Matthias Heßler <hessler@wi-inf.uni-essen.de>
# 
# $Id: 7bit.pl,v 1.3 1997/02/23 01:20:51 hessler Exp $


#
# Variables
#

# Unbuffered print's
$| = 1;

$margin = 6;
$bar = "| ";
$barlen = length ($bar);
# maxlen gibt an, wieviele Zeichen max. pro Zeile ausgegeben werden
# (margin geht extra...)
$maxlen = 79 - $margin;

# Für die Datenausgabe benötigt...
$actline = "";
$commentline = "";
$lastnumber = "";

# Für Änderungsstrich (default: Änderungsstrich!)
$old = 0;

# Fragenzaehler
$maincount = 0;
$subcount  = 0;
$qcount    = 0;

# Statusvariable
$script    = "FALSE";
$state = "begin";
$buffer = "-";


#
# Get file names from prompt
#

$in = $ARGV[0];
die "Usage: 7bit.pl infile [outfile] [headtempfile] [tailtempfile]\n" unless defined ($in);

$out = $ARGV[1];
$out = "$in.asc" unless defined ($out);


#
# Open files
#

open(IN,"<$in") || die "Can not open $in for input!\n";

open(OUT,">$out") || die "Can not open $out for output!\n";


#
# Create FAQ - TOC
#

print ("Creating TOC:\n");

while($line = <IN>) {

    chop($line);  # kill \n character (newline)

    if ($line =~ s/^!1//g) {                           # main headline
	if ($state eq "begin") {
	    $line = &latin2asc ($line);
	    output_newline ("\n\n".$line);
	    output_newline ("#" x length ($line));
	    output_newline ("");
	    $state = "!1";
	}
	else { 
	    print "ERROR: too many !1 or !1 improper located:\n";
	    print "!1$line\n";
	}
    }      
    
    elsif ($state eq "begin") {               # Vorspann...
	$line = &latin2asc ($line);
	output_newline ($line);
    }
    
    elsif ($line =~ s/^!2//g) {                           # bigger headlines
        $maincount++;
        $subcount = 0;
        $qcount   = 0;
	flush_output ();
	$line = &latin2asc ($line);
	output_newline ("\n\n$maincount $line");
	output_newline ("=" x (length($line)+length($maincount)+1));
	output_newline ("");
	$state = "!2";
    }
    
    elsif ($line =~ s/^!3//g) {                           # smaller headlines
	$subcount++;
        $qcount   = 0;
	flush_output ();
	$line = &latin2asc ($line);
	output_newline ("\n$maincount.$subcount $line");
	output_newline ("-" x (length($line)+length($maincount)+length($subcount)+2));
	output_newline ("");
	$state = "!3";
    } 
    
    elsif ($line =~ s/^!old//g) {     # old mode on
	if ($old eq 0) {
	    flush_output();
	    $old = 1;
	}
	else {
	    print "ERROR: more than 1 !old ($state $maincount.$subcount.$qcount):\n"; 
            print "!old$line\n";
	}
    }
    
    elsif ($line =~ s/^Q: //g) {      # questions

	$qcount++;

	# für jede Frage einen Punkt...
	print ".";

	# index part
	if ($state ne "question") {       # first line
	    flush_output ();

	    # delete leading/trailing whitespace
	    $line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	    nice_output ( $line, "$qcount. ");
	}
	else {
	    print "ERROR: \"Q: \" is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "Q: $line\n";
	}
	$state = "question";
    } 

    elsif ($line =~ s/^A: //g) {       # answers
	if ($state eq "question") {
	    flush_output();
	    $old = 0; # Kann wieder zurückschalten, da keine Ausgabe mehr...
	    $state = "answer";
	}
	else {
	    print "ERROR: \"A: \" is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "A: $line\n";
	}
    } 

    elsif ($line =~ s/^!verbon//g) {   # script mode on
	if ($state ne "script") {
	    flush_output ();
	    $buffer = $state;
	    $state = "script"; 
        }
	else {
	    print "ERROR: !verbon is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "!verbon$line\n";
	}
    }

    elsif ($line =~ s/^!verboff//g) {  # script mode off
	if ($state eq "script") {
	    flush_output ();
	    $state = $buffer;
	}
	else {
	    print "ERROR: missing !verbon ($state $maincount.$subcount.$qcount):\n"; 
            print "!verboff$line\n";
	}
    }
    
    elsif ($state eq "script") {       # script mode ("!verbon")
	if ($buffer eq "question") {
	    # leave spaces/margin alone => keine Formattierung
	    ugly_output ($line);
	}	    
	elsif ($buffer eq "answer") {
	    # keine Ausgabe in der TOC
	}
    }

    elsif ($state eq "question") {     # do while in question mode
	# delete leading/trailing whitespace
	$line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	if (length ($line) > 0) {
	    nice_output ( $line);
	}
    }
    
}

flush_output();

#
# Reopen Inputfile
#

close (IN);
print "\n";
open(IN,"<$in") || die "Can not reopen $in for input!\n";


#
# Create main part of FAQ
#

print ("Writing questions:\n");

$maincount= 0;
$subcount = 0;
$qcount   = 0;

$state = "begin";
$buffer = "-";

while($line = <IN>) {

    chop($line);  # kill \n character (newline)

    if ($line =~ s/^!1//g) {                           # main headline
	if ($state eq "begin") {
	    $line = &latin2asc ($line);
	    output_newline ("");
	    output_newline ("\n\n$line");
	    output_newline ("#" x length ($line));
	    output_newline ("");
	    $state = "!1";
	}
	else { 
	    print "ERROR: too many !1 or !1 improper located:\n";
	    print "!1$line\n";
	}
    }      
    
    elsif ($state eq "begin") {               # Vorspann...
	# wurde schon ausgegeben => überspringen!
    }
    
    elsif ($line =~ s/^!2//g) {                           # bigger headlines
        $maincount++;
        $subcount = 0;
        $qcount   = 0;
	flush_output ();
	$line = &latin2asc ($line);
	output_newline ("");
	output_newline ("\n\n$maincount $line");
	output_newline ("=" x (length($line) + length($maincount) + 1));
	output_newline ("");
	$state = "!2";
    }
    
    elsif ($line =~ s/^!3//g) {                           # smaller headlines
        $subcount++;
        $qcount   = 0;
	flush_output ();
	$line = &latin2asc ($line);
	output_newline ("");
	output_newline ("\n\n$maincount.$subcount $line");
	output_newline ("-" x (length($line) + length($maincount) + length($subcount) + 2));
	output_newline ("");
	$state = "!3";
    } 
    
    elsif ($line =~ s/^!old//g) {     # old mode on
	if ($old eq 0) {
	    flush_output();
	    $old = 1;
	}
	else {
	    print "ERROR: more than 1 !old ($state $maincount.$subcount.$qcount):\n"; 
            print "!old$line\n";
	}
    }
    
    elsif ($line =~ s/^Q: //g) {      # questions

	$qcount++;

	# für jede Frage einen Punkt...
	print ".";

	# index part
	if ($state ne "question") {       # first line
	    flush_output ();

	    # delete leading/trailing whitespace
	    $line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	    if ($qcount != 1) {
		output_newline("");
	    }
	    nice_output ( $line, "$qcount. ");
	}
	else {
	    print "ERROR: \"Q: \" is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "Q: $line\n";
	}
	$state = "question";
    } 

    elsif ($line =~ s/^A: //g) {       # answers
	if ($state eq "question") {
	    # delete leading/trailing whitespace
	    $line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	    ugly_output("");
	    nice_output ( $line);
	    $state = "answer";
	}
	else {
	    print "ERROR: \"A: \" is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "A: $line\n";
	}
    } 

    elsif ($line =~ s/^!verbon//g) {   # script mode on
	if ($state ne "script") {
	    flush_output ();
	    $buffer = $state;
	    $state = "script"; 
        }
	else {
	    print "ERROR: !verbon is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "!verbon$line\n";
	}
    }

    elsif ($line =~ s/^!verboff//g) {  # script mode off
	if ($state eq "script") {
	    flush_output ();
	    $state = $buffer;
	}
	else {
	    print "ERROR: missing !verbon ($state $maincount.$subcount.$qcount):\n"; 
            print "!verboff$line\n";
	}
    }
    
    elsif ($line =~ s/^!commenton//g) { # comment mode on
	if ($state ne "comment") {
	    flush_output ();
	    output_newline ("");
	    $buffer = $state;
	    $state = "comment"; 
        }
	else {
	    print "ERROR: !commenton is already active ($state $maincount.$subcount.$qcount):\n"; 
            print "!commenton$line\n";
	}
    }

    elsif ($line =~ s/^!commentoff//g) { # comment mode off
	if ($state eq "comment") {
	    flush_output ();
	    $state = $buffer;
	}
	else {
	    print "ERROR: missing !commenton ($state $maincount.$subcount.$qcount):\n"; 
            print "!commentoff$line\n";
	}
    }
    
    elsif ($state eq "question") {     # do while in question mode
	# delete leading/trailing whitespace
	$line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	if (length ($line) > 0) {
	    nice_output ( $line);
	}
    }
    
    elsif ($state eq "answer") {       # do while in answer mode
	# delete leading/trailing whitespace
	$line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	if (length ($line) > 0) {
	    nice_output ( $line);
	}
	else {
	    flush_output ();
	    output_newline ("");
	    $old = 0;
	}
    }
    
    elsif ($state eq "script") {       # script mode ("!verbon")
	if ($buffer eq "question") {
	    # leave spaces/margin alone => keine Formattierung
	    ugly_output ($line);
	}	    
	elsif ($buffer eq "answer") {
	    # leave spaces/margin alone => keine Formattierung
	    ugly_output ($line);
	}
    }

    elsif ($state eq "comment") {
	# delete leading/trailing whitespace
	$line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	comment_output ($line);
    }

    elsif ($line =~ s/^<.*>//g) {                             # do if mark
	# leave spaces/margin alone
	# - and whatever the purpose of that part is after the change in
	# syntax... MatHes
	print "Skipped mark $&\n";
    }

    else {
	# delete leading/trailing whitespace
	$line =~ s/^[ \t]*(.*?)[ \t]*$/$1/g;
	if (length($line) > 0) {
	    comment_output ($line);
	}
	else {
	    flush_output();
	}
    }
}

flush_output();		
ugly_output ("-" x 75);
ugly_output ("(c) 1995,1996 Das FAQ-Team");

print "\n";


#
# Close files
#

close (IN);
close (OUT);


sub ugly_output
{ # Kein Wrapping, kein Rand (außer bei Änderungsstrich)
# Input Parameter: Textzeile, evtl. Nummerierung (_als String_)
    my ($line ) = @_;

    # Umlaute umwandeln
    $line = &latin2asc ($line);

# flush
    flush_output ();

# output...
    output_simple_line ($line);
}


sub comment_output
{ # Wrapping, kein Rand, kein Änderungsstrich
# Input Parameter: Textzeile, evtl. Numerierung (_als String_)
    my ($line) = @_;
    my $pos = 0;

    # Umlaute umwandeln _vor_ Wrapping
    $line = &latin2asc ($line);

    # Zeile anhängen mit korrektem Zwischenraum
    if (length ($line) > 0) {
	if (length ($commentline) > 0) {
	    if (substr ($commentline, length ($commentline)) ne "-") {
		# Leerzeichen zwischen Zeilen einfügen, falls kein Trennstrich
		$commentline .= " ";
	    }
	    else {
		# Trennstrich entfernen
		$commentline = substr ($commentline, 0, length ($commentline) -1);
	    }
	}
	# Zeile anhängen
	$commentline .= $line;
    }

    # Zeilenweise ausgeben
    while( length( $commentline) > $maxlen+$margin || $commentline =~ /\n/) {
	$pos = wrap_pos ($commentline, $maxlen+$margin);
	if ($pos > 0) {
	    output_newline (substr ($commentline, 0, $pos));
	    # +1 für Trennzeichen: Space bzw. "\n"
	    $commentline = substr ($commentline, $pos +1);
	}
	else {
	    # kein wrapping möglich, da line zu lange => einfach abschneiden!
	    # (zu kurze line muß aufgrund der while-Bedingung ja mindestens
	    # ein "\n" als Trennzeichen haben => schon oben behandelt)
	    output_newline (substr ($commentline, 0, $maxlen));
	    $commentline = substr ($commentline, $maxlen);
	}
    }

    # in commentline steht der Rest der Zeile...
}


sub nice_output
{ # Wrapping und Rand
# Input Parameter: Textzeile, evtl. Numerierung (_als String_)
    my ($line, $number ) = @_;
    $number = "" unless defined ($number);
    my $pos = 0;

    # Umlaute umwandeln _vor_ Wrapping
    $line = &latin2asc ($line);

    # flush actline, when new number comes in
    if (length ($number) > 0) {
	flush_output ();
	# Neue Nummer merken
	$lastnumber = $number;
    }

    # Zeile anhängen mit korrektem Zwischenraum
    if (length ($line) > 0) {
	if (length ($actline) >0) {
	    if (substr ($actline, length ($actline)) ne "-") {
		# Leerzeichen zwischen Zeilen einfügen, falls kein Trennstrich
		$actline .= " ";
	    }
	    else {
		# Trennstrich entfernen
		$actline= substr ($actline, 0, length ($actline) -1);
	    }
	}
	# Zeile anhängen
	$actline .= $line;
    }

    # Zeilenweise ausgeben
    while( length( $actline) > $maxlen || $actline =~ /\n/) {
	$pos = wrap_pos ($actline, $maxlen);
	if ($pos > 0) {
	    print OUT format_margin( $lastnumber);
	    print OUT substr ($actline, 0, $pos);
	    print OUT "\n";
	    # +1 für Trennzeichen: Space bzw. "\n"
	    $actline = substr ($actline, $pos +1);
	}
	else {
	    # kein wrapping möglich, da line zu lange => einfach abschneiden!
	    # (zu kurze line muß aufgrund der while-Bedingung ja mindestens
	    # ein "\n" als Trennzeichen haben => schon oben behandelt)
	    print OUT format_margin( $lastnumber);
	    print OUT substr ($actline, 0, $maxlen);
	    print OUT "\n";
	    $actline = substr ($actline, $maxlen);
	}
	$lastnumber = "";
    }

    # in $actline steht der Rest der Zeile...
}


sub flush_output
{
    # flush actline
    if (length ($actline) > 0) {
	print OUT format_margin( $lastnumber);
	print OUT $actline;
	print OUT "\n";
	$actline = "";
	$lastnumber = "";
    }

    # flush commentline
    if (length ($commentline) > 0) {
	print OUT $commentline;
	print OUT "\n";
	$commentline = "";
    }
}


sub output_newline
{ # Druckt eine Zeile (_ohne_ Änderungsstrich)
# Achtung: die Umwandlung der Umlaute (latin2asc) muß schon erledigt sein!
# Input Parameter: Textzeile
    my ($line) = @_;

    print OUT $line;
    print OUT "\n";
}


sub output_simple_line
{ # Gibt eine Textzeile aus, ggf. mit Änderungsstrich
# Input Parameter: Textzeile
    my ($line) = @_;

    flush_output();

    if ($old == 0) {
	print OUT $bar;
	if ($line =~ /^  /) {
	    print OUT substr ($line, $barlen);
	    print OUT "\n";
	}
	else {
	    print OUT $line;
	    print OUT "\n";
	}
    }
    else {
	print OUT $line;
	print OUT "\n";
    }
}


sub wrap_pos
{
# Input Parameter: Textzeile
    my ($line, $maxlen ) = @_;
    my $pos1 = 0;
    my $pos2 = 0;

    $pos1 = index( $line, "\n");
    if ($pos1 <= $maxlen && $pos1 >= 0) {  # !br/Zeilenumbrüche berücksichtigen
	return $pos1;
    }
    else {
	$pos2 = rindex( $line, " ", $maxlen); # wo word wrapping?
	if ($pos2 <= $maxlen && $pos2 >= 0) {
	    return $pos2;
	}
	else { # Kein wrapping möglich
	    return 0;
	}
    }
}


sub format_margin
{
    my( $number) = @_;   # Input Parameter: evtl. Nummerierung _als String_
    my $out = "";

    if (length ($number) > 0) {
	if ($old == 0) {
	    $out = $bar;
	    $out .= " " x ($margin - length ($number) -$barlen); # Margin
	    $out .= $number;
	}
	else {
	    $out = " " x ($margin - length ($number));     # Margin
	    $out .= $number;
	}
    }
    else {
	if ($old == 0) {
	    $out = $bar;
	    $out .= " " x ($margin -$barlen);   # Margin
	}
	else {
	    $out .= " " x $margin;     # Margin
	}
    }
    return $out;
}


sub latin2asc
{
    my ($line) = @_;

    # native codes
    $line =~ s/ä/ae/g;
    $line =~ s/ö/oe/g;
    $line =~ s/ü/ue/g;
    $line =~ s/Ä/Ae/g;
    $line =~ s/Ö/Oe/g;
    $line =~ s/Ü/Ue/g;
    $line =~ s/ß/ss/g;

    # !br als Umbruch verwenden
    $line =~ s|!br|\n|g;
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
	    . " $url "
		. substr($line, $pos2+1);
    }

    # Hervorhebungen
    $line =~ s|!b1|*|g;
    $line =~ s|!b0|*|g;
    $line =~ s|!i1|/|g;
    $line =~ s|!i0|/|g;
    $line =~ s|!u1|_|g;
    $line =~ s|!u0|_|g;

    return $line;
}

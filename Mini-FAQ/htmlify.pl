#!/usr/bin/perl

$lang = "german";

print'<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">' . "\n";
print'<html><head><title>English-German mini-FAQ for ISDN4Linux</title></head>';
print'<body text="#000000" link="#0000ff" vlink="#000080" bgcolor="#ffffff">';
print"<pre>\n";

open(INFILE, $ARGV[0]) || die("Couldn't open $ARGV[0]");

while($line=<INFILE>)
{
  $line =~ s/&/&amp;/g;
  $line =~ s/</&lt;/g;
  $line =~ s/>/&gt;/g;
  if($line =~ /http:/)
  {
    $line =~ s/(http:[^ \n]+)/<a href="\1">\1<\/a>/;
  }
  elsif($line =~ /ftp:/)
  {
    $line =~ s/(ftp:[^ \n]+)/<a href="\1">\1<\/a>/;
  }
  elsif($line =~ /www./)
  {
    $line =~ s/(www.[^ \n]+)/<a href="http:\/\/\1">\1<\/a>/;
  }
  $line =~ s/\&lt;([A-Za-z0-9_.-]+\@[a-zA-Z0-9.-]+)\&gt;/<a href="mailto:\1">&lt;\1&gt;<\/a>/g;
  if($line =~ /mini-faq f/i)
  {
    $line =~ s/^/<\/pre><h1>/;
    $line =~ s/\n/<\/h1><pre>/;
  }
  if($line =~ /^0/) {$lang = "english"};
  if($lang eq "german")
  {
    if($line =~ /^[0-9]+\./)
    {
      $line =~ s/^([0-9]+)\./<br><a href="#frage\1" name="frage\1">\1.<\/a><b>/;
      $header = "yes";
    }
    $line =~ s/(Frage ([0-9]+))/<a href="#frage\2">\1<\/a>/g;
  }
  else
  {
    if($line =~ /^[0-9]+\./)
    {
      $line =~ s/^([0-9]+)\./<br><a href="#question\1" name="question\1">\1.<\/a><b>/;
      $header = "yes";
    }
    $line =~ s/(question ([0-9]+))/<a href="#question\2">\1<\/a>/g;
  }
  $line =~ s/^ /<br> /;
  if($line =~ /^$/)
  {
    if($header eq "yes")
    {
      $line =~ s/^$/<\/b>/;
      $header = "no";
    }
    else
    {
      $line =~ s/^$//;
    }
  }
  print $line;
}

print"</pre></body></html>\n"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define TABSTOP 40

#define COUNT(array) sizeof(array)/sizeof(array[0])

void print_area(char *area) 
{
  char buf[BUFSIZ];
  char *s,*p;
  
  if (!*area) return;

  s=strncpy (buf,area,BUFSIZ);
  while (*s) {
    if (strlen(s)>TABSTOP-5) {
      for (p=s+TABSTOP-4; p>s; p--)
	if (*p==',') {
	  *(p++)='\0';
	  break;
	}
    } else p="";
    
    printf ("A:%s\n", s);
    s=p;
  }
}

void rprintf (char *name, char *fmt, ...) 
{
  char buf[BUFSIZ], *p;
  int len;
  va_list ap;
  
  va_start(ap, fmt);
  len=vsnprintf(buf, BUFSIZ, fmt, ap);
  p=buf+len;
  for (p=buf+len; len<TABSTOP;len++)
    *(p++)=' ';
  *p='\0';
  printf ("%s%s\n", buf, name);
}

void rate_sample(void) {

  char *Zone[][2] = {{ "", "" }, 
		     { "", "" },
		     { "", "+*" }};
  
  double Tarif[] = { 01.00,
		     22.00 };
  
  int z;
  
  printf ("\n");
  rprintf ("Sample", "P:00");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/0-24=%.2f(60)/60", Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1002(void) {

  char *Zone[][2] = {{ "Bundeslandzone", "" }, 
		     { "Österreichzone", "" },
		     { "special friends", "" },
		     { "Mobilnetz", "0663,0664,0676,0699" },
		     { "EasyInternet", "" },
		     { "Nachbarländer West", "+39,+41,+4175,+49" },
		     { "Nachbarländer Ost", "+36,+386,+420,+421" },
		     { "Europa 3, USA", "+1,+1787,+1809,+1907,+31,+32,+33,+352,+353,+44,+45,+46,+47" },
		     { "Europa 4", "+30,+34,+351,+358,+376,+377,+378,+379,+385,+387,+48" },
		     { "Europa 5", "+355,+356,+357,+359,+371,+372,+375,+380,+389,+40,+90" },
		     { "Europa 6", "+212,+213,+298,+350,+354,+370,+7,+972,+993,+995" },
		     { "Fernost, Südafrika", "+27,+61,+64,+65,+81,+82,+852" },
		     { "Welt 1", "+218,+262,+374,+56,+590,+592,+596,+994" },
		     { "Welt 2", "+1441,+244,+52,+55,+58,+593,+599,+60,+63,+670,+853,+86,+965,+966,+975,+98" },
		     { "Welt 3", "+1242,+1246,+1473,+1767,+1809,+20,+224,+231,+233,+234,+240,+241,+257,+264,+265,+266,+267,+269,+2978,+299,+501,+57,+592,+6723,+673,+674,+685,+687,+7,+856,+961,+962,+971,+973,+974,+977,+996" },
		     { "Welt 4", "+1264,+1345,+1876,+220,+222,+227,+228,+229,+230,+232,+236,+250,+251,+253,+254,+256,+258,+505,+506,+53,+54,+591,+595,+597,+598,+62,+675,+678,+679,+686,+689,+850,+886,+91,+94,+95,+960,+964,+968,+976" },
		     { "Welt 5", "+1268,+226,+243,+247,+682,+880,685" }};
  
  double Tarif[][2] = {{  1.00,  0.88 },
		       {  2.50,  1.00 },
		       {  1.80,  0.88 },
		       {  4.20,  2.88 },
		       {  1.00,  1.00 },
		       {  3.60,  3.40 },
		       {  3.70,  3.50 },
		       {  4.40,  4.00 },
		       {  5.60,  5.40 },
		       {  6.10,  5.90 },
		       {  8.00,  7.80 },
		       {  9.80,  9.60 },
		       { 11.00, 10.80 },
		       { 14.40, 14.20 },
		       { 18.70, 18.50 },
		       { 24.80, 24.60 },
		       { 28.00, 27.80 }};
  
  int z;
  
  printf ("\n");
  rprintf ("UTA", "P:02");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("Geschäftszeit", "T:*/0-24=%.2f(60)/60,%.2f(60)/1", Tarif[z][0],Tarif[z][0]);
    rprintf ("Freizeit", "T:*/0-24=%.2f(60)/60,%.2f(60)/1", Tarif[z][1],Tarif[z][1]);
    print_area(Zone[z][1]);
  }
}    

void rate_1005(void) {

  char *Zone[][2] = {{ "National", "" }, 
		     { "Mobilfunknetz", "0663,0664,0676,0699" },
		     { "Europa 1 / USA", "+1,+30,+31,+32,+33,+34,+352,+353,+358,+36,+377,+386,+39,+41,+4175,+420,+421,+44,+45,+46,+47,+49" },
		     { "Europa 2", "+212,+213,+216,+298,+350,+351,+354,+355,+356,+357,+359,+370,+371,+372,+376,+378,+381,+385,+387,+389,+40,+48,+7,+90,+972" },
		     { "Welt 1", "+61,+64,+65,+81 ,+82,+852" },
		     { "Welt 2", "+*" }};
  
  double Tarif[] = { 1.00, 4.00, 3.50, 6.00, 9.00, 19.00 };
  
  int z;
  

  printf ("\n");
  rprintf ("tele2", "P:05");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/0-24=%.2f(60)/60,%.2f(60)/1", Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_1012(void) {

  char *Zone[][2] = {{ "Nah", "" }, 
		     { "Fern", "" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
		     { "Deutschland spezial", "+49" },
		     { "Nachbarn", "+33,+36,+386,+39,+41,+4175,+420,+421" },
		     { "Europa 1 + USA", "+1,+31,+32,+34,+352,+353,+358,+44,+45,+46,+47" },
		     { "Europa 2", "+298,+356,+376,+377,+381,+385,+387" },
		     { "Europa 3", "+216,+30,+351,+355,+357,+359,+372,+378,+380,+389,+40,+48,+7" },
		     { "Welt 1", "+212,+213,+350,+354,+370,+371,+61,+64,+65,+7,+81,+82,+852,+90,+972" },
		     { "Welt 2", "+*" }};
  
  double Tarif[][2] = {{ 1.00, 0.90 },
		       { 2.00, 0.90 },
		       { 4.00, 3.70 },
		       { 2.90, 2.90 },
		       { 3.70, 3.70 },
		       { 4.40, 4.40 },
		       { 5.60, 5.60 },
		       { 6.10, 6.10 },
		       { 7.90, 7.90 },
		       { 24.90, 24.90 }};
		       
  
  int z;
  
  printf ("\n");
  rprintf ("1012", "P:12");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (z<3) {
      rprintf ("Tag", "T:1-5/8-18=%.2f(60)/60,%.2f(60)/1", Tarif[z][0], Tarif[z][0]);
      rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/60,%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
      rprintf ("Nacht", "T:E,H/0-24=%.2f(60)/60,%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
    } else {
      rprintf ("rund um die Uhr", "T:*/0-24=%.2f(60)/60,%.2f(60)/1", Tarif[z][0],Tarif[z][0]);
    }
    print_area(Zone[z][1]);
  }
}    

void rate_1069(void) {

  char *Zone[][2] = {{ "Festnetz", "" }, 
		      { "ONE Mobilfunknetz", "0699" },
		      { "andere Mobilfunknetze", "0663,0664,0676,699" },
		      { "EU und Nachbarländer", "+31,+32,+33,+34,+352,+353,+358,+36,+386,+39,+41,+4175,+420,+421,+44,+45,+46,+47,+49" },
		      { "Internationale Zone 2", "+1,+1787,+30,+351,+354,+356,+376,+377,+378,+379,+381,+385,+387,+389,+48,+61,+64,+81" },
		      { "Internationale Zone 3", "+212,+213,+216,+350,+355,+357,+359,+371,+372,+380,+40,+65,+7,+852,+90,+972" },
		      { "Internationale Zone 4", "+1242,+1441,+1809,+1876,+218,+233,+27,+299,+373,+374,+5,+52,+55,+56,+58,+60,+673,+685,+7,+82,+853,+886,+968,+971,+994,+995" },
		      { "Internationale Zone 5", "+*" }};
  
  double Tarif[] = { 01.00, 
		     02.00, 
		     04.00, 
		     03.50, 
		     05.50, 
		     08.00, 
		     15.00, 
		     22.00 }; 
  
  int z;
  
  printf ("\n");
  rprintf ("ONE","P:69");
  for (z=0; z<7; z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/0-24=%.2f(60)/30", Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_1001(void) {

  char *Name[] = { "Minimumtarif",
		   "Standartarif",
		   "Geschäftstarif 1",
		   "Geschäftstarif 2",
		   "Geschäftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "Regionalzone", "" }, 
		       { "Fernzone 1", "" },
		       { "Fernzone 2", "" },
		       { "Online-Tarif", "07189" },
		       { "Mobilfunk", "0663,0664,0676,0699" },
		       { "Ausland Zone 1", "+36,+386+389,+39,+41,+420,+49" },
		       { "Ausland Zone 2", "+212,+31,+32,+33,+34,+352,+353,+354,+357,+358,+375,+380,+385,+44,+45,+46,+47" },
		       { "Ausland Zone 3", "+298,+351,+7,+972" },
		       { "Ausland Zone 4", "+61,+886" },
		       { "Ausland Zone 5", "+27,+373,+64,+852," },
		       { "Ausland Zone 6", "+60,+81" },
		       { "Ausland Zone 7", "+86" },
		       { "Ausland Zone 8", "+65,+971" },
		       { "Ausland Zone 9", "+973" },
		       { "Ausland Zone 10", "+54" },
		       { "Ausland Zone 11", "+62,+82,+91" },
		       { "Ausland Zone 12", "" },
		       { "Ausland Zone 13", "" },
		       { "Ausland Zone 14", "+1" },
		       { "Ausland Zone 15", "+30,+48" },
		       { "Handvermittelter Verkehr", "" },
		       { "Grenznahverkehr", "" },
		       { "Teleinfo 04570", "04570" },
		       { "Teleinfo 04500 ", "04500" },
		       { "Telebusiness 04590", "04590" },
		       { "Teleinfo 04580", "04580" },
		       { "Businessline 0711x", "0711" },
		       { "Businessline 0713x", "0713" },
		       { "Businessline 0714x", "0714" },
		       { "Votingline 0717x", "0717" },
		       { "Freephone 0660", "0660" }};


  /* Einheiten in 72 sec */

  double Faktor [][4] = {{  1.25,  1.00,  0.66,  0.45 },  /* Regionalzone bis 50 km */
			   {  3.60,  2.88,  1.44,  1.00 },  /* Fernzone 1 bis 200 km */
			   {  4.80,  3.75,  1.80,  1.44 },  /* Fernzone 2 über 200 km */
			   {  0.60,  0.60,  0.20,  0.20 },  /* Online-Tarif */
			   {  6.00,  6.00,  3.75,  3.75 },  /* Mobilfunk */
			   {  6.00,  6.00,  5.00,  5.00 },  /* Ausland Zone 1 */
			   {  8.00,  8.00,  6.00,  6.00 },  /* Ausland Zone 2 */
			   { 11.00, 11.00,  9.00,  9.00 },  /* Ausland Zone 3 */
			   { 14.00, 14.00, 12.00, 12.00 },  /* Ausland Zone 4 */
			   { 17.00, 17.00, 15.00, 15.00 },  /* Ausland Zone 5 */
			   { 20.00, 20.00, 17.00, 17.00 },  /* Ausland Zone 6 */
			   { 23.00, 23.00, 20.00, 20.00 },  /* Ausland Zone 7 */
			   { 26.00, 26.00, 24.00, 24.00 },  /* Ausland Zone 8 */
			   { 30.00, 30.00, 28.00, 28.00 },  /* Ausland Zone 9 */
			   { 34.00, 34.00, 32.00, 32.00 },  /* Ausland Zone 10 */
			   { 39.00, 39.00, 36.00, 36.00 },  /* Ausland Zone 11 */
			   { 44.00, 44.00, 40.00, 40.00 },  /* Ausland Zone 12 */
			   { 59.00, 59.00, 53.00, 53.00 },  /* Ausland Zone 13 */
			   {  9.00,  9.00,  8.00,  8.00 },  /* Ausland Zone 14 */
			   {  8.00,  8.00, 10.00, 10.00 },  /* Ausland Zone 15 */
			   {  0.00,  0.00,  0.00,  0.00 },  /* Handvermittelter Verkehr */
			   {  4.00,  4.00,  3.00,  3.00 },  /* Grenznahverkehr */
			   {  6.67,  6.67,  6.67,  6.67 },  /* Teleinfo 04570 */
			   { 10.00, 10.00, 10.00, 10.00 },  /* Teleinfo 04500 */ 
			   { 16.00, 16.00, 16.00, 16.00 },  /* Telebusiness 04590 */
			   { 26.67, 26.67, 26.67, 26.67 },  /* Teleinfo 04580 */
			   {  1.25,  1.00,  0.66,  0.45 },  /* Businessline 0711x */
			   {  2.25,  2.25,  2.25,  2.25 },  /* Businessline 0713x */
			   {  4.80,  4.80,  4.80,  4.80 },  /* Businessline 0714x */
			   {  4.80,  4.80,  4.80,  4.80 },  /* Votingline 0717x */
			   {  0.00,  0.00,  0.00,  0.00 }}; /* Freephone 0660 */

  int t, z;
  char s[BUFSIZ];

  for (t=0; t<4; t++) {
    printf ("\n");
    rprintf ("Telekom Austria", "P:01,%d", t+1);
    sprintf (s, "%s (ATS %.3f / Impuls)", Name[t], Tarif[t]);
    rprintf (s, "C:Tarif:");
    for (z=0; z<COUNT(Zone); z++) {
      printf ("\n");
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (z==30) { /* Freephone */
	rprintf ("rund um die Uhr", "T:*/0-24=0/72");
      } else {
	if (Faktor[z][2]) rprintf ("Sparzeit",      "T:1-5/06-08,18-20=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	if (Faktor[z][0]) rprintf ("Tageszeit 1",   "T:1-5/08-12=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	if (Faktor[z][0]) rprintf ("Tageszeit 1",   "T:1-4/13-16=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	if (Faktor[z][1]) rprintf ("Tageszeit 2",   "T:1-4/12-13,16-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	if (Faktor[z][1]) rprintf ("Tageszeit 2",   "T:5/12-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	if (Faktor[z][2]) rprintf ("Sparzeit",      "T:E,H/06-20=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	if (Faktor[z][3]) rprintf ("Supersparzeit", "T:*/20-06=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][3]);
      }
      print_area(Zone[z][1]);
    }
  }    
}

void main (void)
{
  printf ("# created by rate-at.c\n\n");
  printf ("# The information herein was machine-generated,\n");
  printf ("# so do not contribute patches to this file.\n\n");
  printf ("# Please contact Michael Reinelt <reinelt@eunet.at>\n");
  printf ("# if you have any corrections or additions. Thanks!\n\n\n");
  printf ("V:1.0-Austria [02-Apr-1999]\n");

  rate_1005();
  rate_1012();
  rate_1069();
  rate_1001();

}

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define __USE_GNU /* isblank ... */
#include <ctype.h>

#define TABSTOP 35
#define TAGSTOP 75

#define COUNT(array) sizeof(array)/sizeof(array[0])
/*
  Adressen-URL: http://telecom.bmv.gv.at/deutch/contacts/konzess.html
*/

void sv_printf(char *s, char *n) {
  printf("S:%s\nN:%s\n\n",s,n);
}

void  write_services(void) {
  printf("\n# Service aka Sondernummern\n");
  sv_printf("Euro-Notruf","112");
  sv_printf("Feuerwehr","122");
  sv_printf("Polizei","133");
  sv_printf("Rettung","144");
  sv_printf("Bergrettung","140");
  sv_printf("�rztenotdienst","141");
  sv_printf("Xxxx","142");
  sv_printf("Arb�","123");
  sv_printf("�amtc","120");
  sv_printf("Internet","07189*,19411,19430,19440,019088333,194040");
  sv_printf("St�rung","111*");
  sv_printf("Auskuft AT,DE","11811");
  sv_printf("Auskuft Int","11812");
  sv_printf("Diverse Dienste","11820,15*,114,16*,190");
  sv_printf("EMS","02290414");
  sv_printf("Telegramm","022900");
  sv_printf("Diverse Ortstarif","02290*\t# Wildcard after special");
  printf("\n");
}

char *strip (char *s)
{
  char *p;

  while (isblank(*s)) s++;
  for (p=s; *p; p++)
    if (*p=='#' || *p=='\n') {
      *p='\0';
      break;
    }
  for (p--; p>s && isblank(*p); p--)
    *p='\0';
  return s;
}

void rprintf (char *fmt, char *name, ...)
{
  char buf[BUFSIZ], *p, *s;
  int len;
  va_list ap;
  int prefix, v;

  va_start(ap, name);
  len=vsnprintf(buf, BUFSIZ, fmt, ap);
  if (name && *name) {
    p=buf+len;
    for (p=buf+len; len<TABSTOP;len++)
      *(p++)=' ';
    *p='\0';
    s = buf;
    switch (*s) {
    case 'P':
      s+=2;
      while (isblank(*s)) s++;
      prefix = strtol(s, &s ,10);
      while (isblank(*s)) s++;
      printf("\n###########################\n");
      v=0;
      printf ("%s%s\n", buf, name);
      sprintf(buf,"%4d",1000+prefix);
      printf("B:%s\n",prefix<95?buf:"Kabel");
      printf("C:NR:%s\n",prefix<95?buf:"Kabel");
      return;
    case 'Z':
      printf("\n");
      break;
    }
    printf ("%s%s\n", buf, name);
  } else {
    printf ("%s\n", buf);
  }
}

static char* str2list (char **s)
{
  static char buffer[BUFSIZ];
  char *p=buffer;

  while (**s) {
    if (**s==',')
      break;
    else
      *p++=*(*s)++;
  }
  *p = '\0';
  return buffer;
}

void print_area(char *area)
{
  int   col;
  char *a;

  if (!*area)
    return;

  col = 0;
  while (1) {
    if (*(a=strip(str2list(&area)))) {
      if (*a=='+' && *(a+1)=='\0') {
	if (col) {
	  printf ("\n");
	  col=0;
	}
	rprintf ("A:+", "# alle �brigen L�nder" );
      } else {
	if (col+strlen(a) > TAGSTOP) {
	  printf ("\n");
	  col=0;
	}
	if (col)
	  col+=printf (", %s", a);
	else
	  col=printf ("A:%s", a);
      }
    }
    if (*area==',') {
      area++;
      continue;
    }
    break;
  }
  if (col)
    printf ("\n");
}

void rate_sample(void)
{

  char *Zone[][2] = {{ "", "" },
		     { "", "" },
		     { "", "+" }};

  double Tarif[] = { 01.00,
		     22.00 };

  int z;

  printf ("\n");
  rprintf ("P:00", "Sample" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/60", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1001_old(void)
{

  char *Name[] = { "Minimumtarif",
		   "Standardtarif",
		   "Gesch�ftstarif 1",
		   "Gesch�ftstarif 2",
		   "Gesch�ftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "FreePhone", "122,133,144,+43800" },
		     { "Regionalzone", "+43660" },
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "Online-Tarif", "+437189,19411,19430,19440" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, D�nemark,Finnland, Frankreich, Gro�britannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rum�nien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische F�deration, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Franz�sisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, S�dafrika" },
		     { "Ausland Zone 6", "Japan, S�dkorea, Malaysia, Niederl�ndische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, �thiopien, Benin, Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, T�rkei" },
		     { "Handvermittelter Verkehr", "Afghanistan, Amerikanisch-Samoa, Guantanamo, Marshallinseln, Midway-Inseln, Mikronesien, Palau, Pitcairn-Inseln, Tuvalu, Wake-Inseln" },
		     { "Grenznahverkehr", "" },
		     { "Teleinfo 04570", "+434570" },
		     { "Teleinfo 04500 ", "+434500" },
		     { "Telebusiness 04590", "+434590" },
		     { "Teleinfo 04580", "+434580" },
		     { "Businessline 0711x", "+43711" },
		     { "Businessline 0713x", "+43713" },
		     { "Businessline 0714x", "+43714" },
		     { "Votingline 0717x", "+43717" }};


  /* Einheiten in 72 sec */

  double Faktor [][4] = {{  0.00,  0.00,  0.00,  0.00 },  /* Freephone 0660 */
			 {  1.25,  1.00,  0.66,  0.45 },  /* Regionalzone bis 50 km */
			 {  3.60,  2.88,  1.44,  1.00 },  /* Fernzone 1 bis 200 km */
			 {  4.80,  3.75,  1.80,  1.44 },  /* Fernzone 2 �ber 200 km */
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
			 {  4.80,  4.80,  4.80,  4.80 }}; /* Votingline 0717x */

  int t, z;
  char s[BUFSIZ];

  for (t=0; t<4; t++) {
    rprintf ("P:01,%d", "Telekom Austria Alt" , t+1);
    rprintf ("G:-01.09.1999","#Valid til 00:00");
    rprintf ("C:Maintainer:", "Michael Reinelt <reinelt@eunet.at>" );
    rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, " );
    rprintf ("C:Zone:", "Fernzone 1 bis 200 Km, Fernzone 2 dar�ber." );
    rprintf ("C:Special:", "Variable Taktung, abh�ngig von Zone und Tageszeit." );
    rprintf ("C:Name:", "Telekom Austria" );
    rprintf ("C:Address:", "Postgasse 8, A-1010 Wien" );
    rprintf ("C:Homepage:", "http://www.telekom.at" );
    rprintf ("C:TarifURL:", "http://www.telekom.at/tarife/" );
    rprintf ("C:Telefon:", "0800 100 100" );
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name[t], Tarif[t]);
    rprintf ("# Tarif:", s );
    rprintf ("D:pta", "# Verzonung" );
    for (z=0; z<COUNT(Zone); z++) {
      rprintf ("Z:%d", Zone[z][0] , z);
      if (z==0) { /* Freephone */
	rprintf ("T:*/*=0/72", NULL );
      } else if (Faktor[z][0]) {
	if (Faktor[z][0]==Faktor[z][1]) {
	  rprintf ("T:W/08-18=%.3f/%.5g", "Tageszeit" , Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("T:W/08-12=%.3f/%.5g", "Tageszeit 1" , Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("T:1-4/13-16=%.3f/%.5g", "Tageszeit 1" , Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("T:1-4/12-13,16-18=%.3f/%.5g", "Tageszeit 2" , Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("T:5/12-18=%.3f/%.5g", "Tageszeit 2" , Tarif[t], 72.0/Faktor[z][1]);
	}
	if (Faktor[z][2]==Faktor[z][3]) {
	  rprintf ("T:W/18-8=%.3f/%.5g", "Sparzeit" , Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("T:E,H/*=%.3f/%.5g", "Sparzeit" , Tarif[t], 72.0/Faktor[z][2]);
	} else {
	  rprintf ("T:W/06-08,18-20=%.3f/%.5g", "Sparzeit" , Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("T:E,H/06-20=%.3f/%.5g", "Sparzeit" , Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("T:*/20-06=%.3f/%.5g", "Supersparzeit" , Tarif[t], 72.0/Faktor[z][3]);
	}
      }
      print_area(Zone[z][1]);
    }
  }
}


void rate_1001(void)
{

  char *Name[] = { "Minimumtarif",
		   "Standardtarif",
		   "Gesch�ftstarif 1",
		   "Gesch�ftstarif 2",
		   "Gesch�ftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "FreePhone", "112,122,133,141,142,144,+43800,+43801,+43802,+43803,+43804" },
		     { "Regionalzone", "111*,11820,15*,+437111,+437112,+437113,+437114,+43810,02290" },
		     { "�sterreichzone", "�sterreich" },
		     { "Mobilfunk 1", "+43663,+43664,+43676" },
		     { "Mobilfunk 2", "+43699" },
		     { "Online", "07189*,19411,19430,19440" },
		     { "AON Complete", "194040" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, D�nemark,Finnland, Frankreich, Gro�britannien, Nordirland, Irland, Jugoslawien, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rum�nien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische F�deration, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Franz�sisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, S�dafrika" },
		     { "Ausland Zone 6", "Japan, S�dkorea, Malaysia, Niederl�ndische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa (West), Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, �thiopien, Benin, Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, T�rkei" },
		     { "Handvermittelter Verkehr", "Afghanistan, Amerikanisch-Samoa, Guantanamo, Marshallinseln, Midway-Inseln, Mikronesien, Palau, Pitcairn-Inseln, Tuvalu, Wake-Inseln" },
		     { "Grenznahverkehr", "" },
		     { "Inmarsat-A", "Inmarsat A" },
		     { "Inmarsat-B/M", "Inmarsat B, Inmarsat M" },
		     { "Inmarsat-Mini-M", "Inmarsat Mini-M" },
		     { "Iridium 008816", "Iridium 008816" },
		     { "Iridium 008817", "Iridium 008817" },
		     { "EMSAT", "" },
		     { "0711-5,6,7", "+437115,+437116,+437117" },
		     { "0711-8,9,0", "+437118,+437119,+437110" }};


  /* Einheiten in 72 sec */

  double Faktor [][2] = {{  0.00,  0.00 },  /* FreePhone */
			 {  1.00,  0.45 },  /* Regionalzone */
			 {  2.88,  1.00 },  /* �sterreichzone */
			 {  5.00,  3.75 },  /* Mobilfunk 1 */
			 {  5.59,  4.45 },  /* Mobilfunk 2 */
			 {  0.60,  0.20 },  /* Online */
			 {  0.00,  0.00 },  /* AON Complete */
			 {  4.80,  4.00 },  /* Ausland Zone 1 */
			 {  6.00,  5.00 },  /* Ausland Zone 2 */
			 {  6.75,  6.00 },  /* Ausland Zone 3 */
			 { 10.00,  9.00 },  /* Ausland Zone 4 */
			 { 12.00, 11.00 },  /* Ausland Zone 5 */
			 { 15.00, 14.00 },  /* Ausland Zone 6 */
			 { 17.00, 15.00 },  /* Ausland Zone 7 */
			 { 20.00, 17.00 },  /* Ausland Zone 8 */
			 { 23.00, 20.00 },  /* Ausland Zone 9 */
			 { 24.00, 23.00 },  /* Ausland Zone 10 */
			 { 28.00, 26.00 },  /* Ausland Zone 11 */
			 { 30.00, 28.80 },  /* Ausland Zone 12 */
			 { 36.00, 34.00 },  /* Ausland Zone 13 */
			 {  6.75,  6.00 },  /* Ausland Zone 14 */
			 {  6.75,  6.75 },  /* Ausland Zone 15 */
			 {  0.00,  0.00 },  /* Handvermittelter Verkehr */
			 {  4.00,  3.00 },  /* Grenznahverkehr */
			 { 99.00, 99.00 },  /* Inmarsat-A */
			 { 67.00, 67.00 },  /* Inmarsat-B/M */
			 { 48.00, 48.00 },  /* Inmarsat-Mini-M */
			 {150.00,150.00 },  /* Iridium 008816 */
			 { 67.00, 67.00 },  /* Iridium 008817 */
			 { 48.00, 48.00 },  /* EMSAT */
			 {  2.25,  2.25 },  /* 0711-5,6,7 */
			 {  4.80,  4.80 }}; /* 0711-8,9,0 */

  int t, z;
  char s[BUFSIZ];
  char *gf[] = {"","Cost=240-198","Cost=288-198","Cost=468-198","Cost=1788-198"};
  char *gt[] = {"Grundgeb�hr ATS 198","Grundgeb�hr ATS 240","Grundgeb�hr ATS 288",
		"Grundgeb�hr ATS 468","Grundgeb�hr ATS 1788, Businesskunden"};
  for (t=0; t<5; t++) {
    sprintf(s,"Telekom Austria %s",Name[t]);
    rprintf ("P:01,%d", s , t+1);
    rprintf ("G:01.09.1999","#Valid from");
    rprintf ("C:Maintainer:", "Michael Reinelt <reinelt@eunet.at>" );
    rprintf ("C:TarifChanged:", "01.09.1999" );
    rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, alles andere ist �sterreichzone." );
    rprintf ("C:Special:", "Variable Taktung, abh�ngig von Zone und Tageszeit." );
    rprintf ("C:Name:", "Telekom Austria" );
    rprintf ("C:Address:", "Postgasse 8, A-1010 Wien" );
    rprintf ("C:Homepage:", "http://www.telekom.at" );
    rprintf ("C:TarifURL:", "http://www.telekom.at/tarife/" );
    rprintf ("C:Telefon:", "0800 100 100" );
    if (*gt[t])
      rprintf ("C:GT:", gt[t] );
    rprintf ("C:GF:", gf[t] );
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name[t], Tarif[t]);
    rprintf ("# Tarif:", s );
    rprintf ("D:1001", "# Verzonung" );
    for (z=0; z<COUNT(Zone); z++) {
      rprintf ("Z:%d", Zone[z][0] , z);
      if (z==0) { /* Freephone */
	rprintf ("T:*/*=0/72", NULL );
      } else if (Faktor[z][0]) {
	if (Faktor[z][0]==Faktor[z][1]) {
	  rprintf ("T:*/*=%.3f/%.5g", "0-24h" , Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("T:W/08-18=%.3f/%.5g", "Gesch�ftszeit" , Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("T:W/18-08=%.3f/%.5g", "Freizeit" , Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("T:E,H/*=%.3f/%.5g", "Freizeit" , Tarif[t], 72.0/Faktor[z][1]);
	}
      }
      else
        rprintf ("T:*/*=0/60","0-24h"); // AON Complete
      print_area(Zone[z][1]);
    }
  }
}


void rate_1002(void) {

  char *Zone[][2] = {
    { "�sterreich", "�sterreich" },
    { "Mobilnetz", "+43663,+43664,+43676,+43699" },
    { "EasyInternet", "019088333" },
    { "Nachbarl�nder West (Z1)", "Deutschland, Italien, Liechtenstein, Schweiz" },
    { "Nachbarl�nder Ost (Z2)", "Slowakei, Slowenien, Tschechien, Ungarn" },
    { "Europa 3, USA (Z3)", "Belgien, D�nemark, Frankreich, Gro�britannien, Irland, Amerikanische Jungferninseln, Kanada, Luxemburg, Niederlande, Norwegen, Puerto Rico, Schweden, Vereinigte Staaten (USA), Alaska, Hawaii" },
    { "Europa 4 (Z4)", "Andorra, Bosnien-Herzegowina, Finnland, Griechenland, Kanarische Inseln, Kroatien, Madeira, Monaco, Polen, Portugal, San Marino, Spanien, Vatikan" },
    { "Europa 5 (Z5)", "Albanien, Azoren, Belarus, Bulgarien, Estland, Lettland, Malta, Mazedonien, Rum�nien, T�rkei, Ukraine, Zypern" },
    { "Europa 6 (Z6)", "Algerien, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Litauen, Marokko, Ru�land, Tadschikistan, Turkmenistan, Usbekistan" },
    { "Fernost, S�dafrika (Z7)", "Australien, Hongkong, Japan, Kokosinseln, Neuseeland, Singapur, S�dafrika, S�dkorea, Weihnachtsinseln" },
    { "Welt 1 (Z8)", "Armenien, Aserbaidschan, Chile, Franz�sisch-Guayana, Guadeloupe, Libyen, Martinique, Reunion" },
    { "Welt 2 (Z9)", "Angola, Antarktis, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Kuwait, Macao, Malaysia, Mexiko, Saint Pierre und Miquelon, Moldau, Niederl�ndische Antillen, Philippinen, Saipan, Saudi-Arabien, Venezuela" },
    { "Welt 3 (Z10)", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Bahrain, Barbados, Belize, Botswana, Brunei, Burundi, Dominica, Dominikanische Republik, Elfenbeink�ste, Gabun, Ghana, Grenada, Gr�nland, Guinea, Guinea-Bissau, Guyana, Jordanien, Britische Jungferninseln, Kasachstan, Katar, Kirgisistan, Kolumbien, Komoren, Laos, Lesotho, Libanon, Liberia, Malawi, Mayotte, Namibia, Nauru, Nepal, Neukaledonien, Saint Kitts und Nevis, Nigeria, Norfolk-Inseln, Panama, Peru, Sambia, Sao Tome und Principe, Simbabwe, Saint Helena, Saint Lucia, Saint Vincent und die Grenadinen, Sudan, Swasiland, Syrien, Tansania, Tonga, Trinidad & Tobago, Turks- und Caicos-Inseln, Vereinigte Arabische Emirate, Samoa (West)" },
    { "Welt 4 (Z11)", "Anguilla, Argentinien, �thiopien, Benin, Bolivien, Caymaninseln, Costa Rica, Dschibuti, Fidschi, Franz�sisch-Polynesien, Gambia, Indien, Indonesien, Irak, Jamaica, Kenia, Kiribati, Kuba, Malediven, Mauretanien, Mauritius, Mongolei, Mosambik, Myanmar, Nicaragua, Niger, Niue, Nordkorea, Oman, Papua-Neuguinea, Paraguay, Ruanda, Sierra Leone, Sri Lanka, Surinam, Taiwan, Togo, Uganda, Uruguay, Vanuatu, Zentralafrikanische Republik" },
    { "Welt 5 (Z12)", "Afghanistan, Amerikanisch-Samoa, Antigua & Barbuda, Ascension, Bangladesch, Burkina Faso, Cook-Inseln, Diego Garcia, El Salvador, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Kamerun, Kongo, Madagaskar, Honduras, Jemen, Kambodscha, Kap Verde, Mali, Marshallinseln, Midway-Inseln, Mikronesien, Montserrat, Pakistan, Palau, Salomonen, Senegal, Seychellen, Somalia, Tokelau, Tschad, Tuvalu, Vietnam, Wake-Inseln, Wallis- und Futuna-Inseln, Zaire" }};

  double Tarif[][2] = {
    {  0.88,  0.88 }, /* 1.9. rundum die Uhr */
    {  3.90,  2.88 },
    {  0.53,  0.18 }, /* internet = TA-Online */
    {  2.50,  2.30 }, /* Nachb west */
    {  3.70,  3.50 },
    {  3.60,  3.40 }, /* Z 3 */
    {  5.60,  5.40 },
    {  5.60,  5.40 },
    {  8.00,  7.80 },
    {  9.80,  9.60 },
    { 11.00, 10.80 },
    { 14.40, 14.20 },
    { 18.70, 18.50 },
    { 24.80, 24.60 },
    { 28.00, 27.80 }};

  int z;

  rprintf ("P:02", "UTA" );
  rprintf ("C:Maintainer:", "Leopold Toetsch <lt@toetsch.at>" );
  rprintf ("C:TarifChanged:", "07.09.1999" );
  rprintf ("C:Homepage:", "http://www.utanet.at" );
  rprintf ("C:TarifURL:", "http://www.utanet.at/1/txt/0/12.html" );
  rprintf ("C:Name:", "UTA Telekom AG" );
  rprintf ("C:Address:", "Rooseveltplatz 2, A-1090 Wien" );
  rprintf ("C:Telefon:", "0800 882 882, 0800 800 800" );
  rprintf ("C:Telefax:", "0800 882 329" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
    else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Gesch�ftszeit" , Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/1", "Freizeit" , Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/1", "Freizeit" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1003(void) {

  char *Zone[][2] = {{ "Regionalzone", "" },
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "5 Freunde", "" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Europa spezial", "Belgien, Frankreich, Gro�britannien" },
		     { "Ausland Zone 1", "Czech Republic, Hungary, Italy, Slovakia, Slovenia, Switzerland" },
		     { "Ausland Zone 2", "Bosnia and Hercegovina, Canada, Croatia, Denmark, Finland, Luxembourg, Monaco, Netherlands, Norway, Poland, Puerto Rico, Spain, Sweden, United States of America, American Virgin Isles, Vatican City, Montenegro" },
		     { "Ausland Zone 3", "Andorra, Belarus, Bulgaria, Greece, Ireland, Malta, Macedonia, Romania, San Marino, Tunesia" },
		     { "Ausland Zone 4", "Albania, Cyprus, Israel, Portugal, Turkey, Ukraine" },
		     { "Ausland Zone 5", "Algeria, Antarctica, Australia, Weihnachtsinseln, Cocos Island, Estonia, F�r�er, Gibraltar, Japan, South Korea, Latvia, Moldova, Marocco, Russian Federation, South Africa, Tajikistan" },
		     { "Ausland Zone 6", "Azerbaijan, Chile, French Guyana, Georgia, Greenland, Guadaloupe, Hongkong, Iceland, Libya, Lithuania, Martinique, Mayotte, Mexico, Phillipines, Reunion, St. Pierre and Miquelon, Singapore, Thailand, Turkmenistan, Uganda, United Arab Emirates" },
		     { "Ausland Zone 7", "Angola, Argentina, Bahrain, Bhutan, Botswana, Brazil, Burundi, Comoros, Dominican Republic, Ecuador, Egypt, Gabon, Gambia, India, Indonesia, Iran, Jordan, Kuwait, Kyrgyzstan, Macau, Malawi, Malaysia, Namibia, Netherlands Antilles, New Zealand, Nigeria, Oman, Saipan, Saudi Arabia, Sri Lanka, Swaziland, Taiwan, Venezuela, Zambia, Zimbabwe" },
		     { "Ausland Zone 8", "Armenia, Aruba, Bahamas, Barbados, Belize, Bermuda, Brunei, Central African Republic, China, Colombia, Costa Rica, Cuba, Djibouti, Equatorial Guinea, Ghana, Guinea-Bissau, Guyana, Kazakhstan, Lesotho, Liberia, Mongolia, Nepal, Panama, Saint Helena, Sao Tome and Principa, Sudan, Syria, Tanzania, Trinidad & Tobago, Turks & Caicos Islands" },
		     { "Ausland Zone 9", "Ascension, Benin, Bolivia, Burkina Faso, Cameroon, Cape Verde, Chad, Elfenbeink�ste, El Salvador, Ethiopia, Fiji, French Polynesia, Guatemala, Haiti, Honduras, Iraq, Jamaica, Kenya, Kiribati, Laos, Lebanon, Maldives, Mali, Mauritania, Mauritius, Mozambique, Myanmar, Nauru, New Caledonia, Nicaragua, Niue, Norfolk Island, Pakistan, Papua New Guinea, Peru, Quatar, Rwanda, Saint Lucia, St. Vincent and the Grenadines, Samoa (West), Senegal, Seychelles Islands, Sierra Leone, Somalia, Suriname, Togo, Tonga, Uzbekistan, Vanuatu, Jemen, Zaire" },
		     { "Ausland Zone 10", "Afghanistan, American Samoa, Anguilla, Antigua & Barbuda, Bangladesh, British Virgin Isles, Cambodia, Cayman Islands, Congo, Cook Islands, Dominica, Eritrea, Falkland Islands, Grenada, Guam, Guantanamo, Nordkorea, Madagascar, Marshall Islands, Montserrat, Niger, Palau, Paraguay, Pitcairn Islands, Tuvalu, Uruguay, Vietnam, Wake Island" }};

  double Tarif[][2] = {{ 1.0,  1.0},
		       { 1.0,  1.0},
		       { 2.0,  2.0},
		       { 1.0,  1.0},
		       { 3.0,  3.0},
		       { 3.0,  3.0},
		       { 4.0,  4.0},
		       { 3.9,  3.5},
		       { 4.6,  4.3},
		       { 6.1,  5.8},
		       { 8.3,  6.9},
		       {10.9, 10.6},
		       {14.6, 14.3},
		       {22.9, 22.6},
		       {27.1, 25.6},
		       {34.1, 33.9},
		       {53.5, 49.2}};

  int z;

  rprintf ("P:03", "Multikom" );
  rprintf ("C:Name:", "1003 Multikom Austria Telekom" );
  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.multikom.at" );
  rprintf ("C:EMail:", "multikom@multikom.at" );
  rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, die Fernzone 1 bis " );
  rprintf ("C:Zone:", "200 Km alles andere ist Fernzone 2." );
  rprintf ("# Fixme:", "Verzonung nicht verifiziert" );
  rprintf ("D:pta", "# Verzonung" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z][0]);
    else {
      rprintf ("T:*/8-18=%.2f(60)/60/1", "Tag" , Tarif[z][0]);
      rprintf ("T:*/18-8=%.2f(60)/60/1", "Nacht" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1004_1(void) {
  char *Zone[][2] = {
    { "Regional", "" },
    { "Austria", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
  };
  double Tarif[][2] = {
    {0.65, 0.65}, /* W/N/B O/ST/K V/T */
    {0.90, 0.75},
    {2.50, 2.05}
  };
  int z;

  rprintf ("P:04,1", "Globalone" );
  rprintf ("C:Name:", "Globalone" );
  rprintf ("C:Address:", "Print Eugen-Str. 8-10, A-1040" );
  rprintf ("C:Homepage:", "http://www.globalone.at" );
  rprintf ("C:TarifURL:", "http://www.globalone.at/tele_assi_national.html" );
  rprintf ("C:EMail:", "info.at@globalone.at" );
  rprintf ("C:Zone:", "Regionalzone sind Gespr�che innerhalb W/N�/B, O�/St/K und S/T/V, " );
  rprintf ("C:Zone:", "Gespr�che zwischen diesen 3 Gebieten sind Fernzone " );
  rprintf ("D:1004", "# Verzonung" );
  rprintf ("# Fixme:", "Ausland nicht implemtiert" );
  rprintf ("C:Special:", "Ausland nicht implemtiert" );
  rprintf ("C:GT:", "Businesskunden ab ATS 5000" );
  rprintf ("C:GF:", "Ch >= 5000 ? 1 : -1" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]*1.2);
    else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Gesch�ftszeit" , Tarif[z][0]*1.2);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1]*1.2);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1]*1.2);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1004_2(void) {
  char *Zone[][2] = {
    { "Regional", "" },
    { "Austria", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
  };
  double Tarif[][2] = {
    {0.55, 0.55}, /* W/N/B O/ST/K V/T */
    {0.80, 0.70},
    {2.40, 1.95}
  };
  int z;

  rprintf ("P:04,2", "Globalone Bus." );
  rprintf ("C:Name:", "Globalone" );
  rprintf ("C:Address:", "" );
  rprintf ("C:TarifChanged:", "01.09.1999" );
  rprintf ("C:Homepage:", "http://www.globalone.at" );
  rprintf ("C:TarifURL:", "http://www.globalone.at/tele_assi_national.html" );
  rprintf ("C:Zone:", "Regionalzone sind Gespr�che innerhalb Wien/N�/B, O�/St/K und S/T/V, " );
  rprintf ("C:Zone:", "Gespr�che zwischen diesen 3 Gebieten sind Fernzone " );
  rprintf ("D:1004", "# Verzonung" );
  rprintf ("# Fixme:", "Ausland nicht implemtiert" );
  rprintf ("C:Special:", "Ausland nicht implemtiert" );
  rprintf ("C:Special:", "Wien Gesch�ftszeit 0.68+Mwst." );
  rprintf ("C:GT:", "Businesskunden ab ATS 5000" );
  rprintf ("C:GF:", "Ch >= 5000 ? 1 : -1" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]*1.2);
    else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Gesch�ftszeit" , Tarif[z][0]*1.2);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1]*1.2);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1]*1.2);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1005(void) {

  char *Zone[][2] = {{ "National", "�sterreich" },
		     { "Mobilfunknetz", "+43663,+43664,+43676,+43699" },
		     { "D-I-CH","Deutschland, Italien, Schweiz" },
		     { "Europa 1 / USA", "Belgien, D�nemark, Finnland, Frankreich, Griechenland, Grossbritannien, Irland, Liechtenstein, Luxemburg, Monaco, Niederlande, Nordirland, Norwegen, Schweden, Slowakei, Slowenien, Spanien,  Tschechien, Ungarn, USA, Hawaii, Kanada" },
		     { "Europa 2", "Albanien, Algerien, Andorra, Bosnien-Herzegowina, Bulgarien, Estland, F�r�er-Inseln, Gibraltar, Island, Israel, Jugoslawien, Kroatien, Lettland, Litauen, Mazedonien, Malta, Marokko, Polen, Portugal, Rum�nien, Russland, San Marino, Tunesien, T�rkei, Zypern" },
		     { "Welt 1", "Australien, Hongkong, Japan, Neuseeland, S�dkorea, Singapur" },
		     { "Welt 2", "Bermuda,Antigua und Barbuda,St. Lucia,Dominikanische Republik,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,Puerto Rico,Bahamas,Anguilla,Jamaika,St. Vincent und Grenadinen,Montserrat,St. Kitts und Nevis,Grenada,Trinidad und Tobago,Marianen (SaipanNord-),Kaimaninseln,Amerikanische Jungferninseln,Barbados,+"}
  };

  double Tarif[] = { 0.85, 3.90, 2.50, 3.50, 6.00, 9.00, 19.00 };

  int z;


  rprintf ("P:05", "tele2" );
  rprintf ("C:Name:", "Tele2 Telecommunications Services GmbH" );
  rprintf ("C:TarifChanged:", "27.10.1999" );
  /*  rprintf ("","C:Address:"); */
  rprintf ("C:Homepage:", "http://www.tele2.at" );
  rprintf ("C:TarifURL:", "http://www.tele2.at/ger/tarif.htm" );
  rprintf ("C:Telefon:", "0800 24 00 24" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1007(void) { /* Fairtel lt 24.08.99 */
  char *Zone[][2] = {{ "Nahzone", "" },
		     { "Fernzone", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "EU+Angrenzende","Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn,"
		       "Belgien, D�nemark, Finnland, Griechenland, Gro�britannien, Irland, Luxemburg, Niederlande, Norwegen, Portugal, Schweden, Spanien"
		     },
		     {"Welt","Kanada,Bermuda,Antigua und Barbuda,St. Lucia,Dominikanische Republik,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,USA,Puerto Rico,Anguilla,Bahamas,Jamaika,St. Vincent und Grenadinen,Montserrat,St. Kitts und Nevis,Grenada,Trinidad und Tobago,Hawaii,Marianen (SaipanNord-),Kaimaninseln,Amerikanische Jungferninseln,Barbados,+"}
  };
  double Tarif[] = { 0.82, 0.88, 3.60, 2.50, 21.12 };
  int z;

  rprintf ("P:07", "European Telecom Fairtel" );
  rprintf ("C:TarifChanged:", "14.09.1999" );
  rprintf ("C:Name:", "European Telecom" );
  rprintf ("C:Zone:", "Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der " );
  rprintf ("C:Zone:", "Nahzone sonst in der Fernzone." );
  rprintf ("C:Address:", "Florag. 7, A-1040 Wien" );
  rprintf ("C:Homepage:", "http://www.europeantelecom.at" );
  rprintf ("C:EMail:", "info@europeantelecom.at" );
  rprintf ("C:Telefon:", "0800 1007 1007" );
  rprintf ("D:1012", "# Verzonung" );
  rprintf ("# Tarif:", "Fairtel" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1007_old(void) { /* lt 24.08.99 */

  char *Name[] = { "1 Jahr feste Bindung",
		   "14-t�gig k�ndbar" };

  char *Zone[][2] = {{ "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, D�nemark,Finnland, Frankreich, Gro�britannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rum�nien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische F�deration, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Franz�sisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, S�dafrika" },
		     { "Ausland Zone 6", "Japan, S�dkorea, Malaysia, Niederl�ndische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa (West), Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, �thiopien, Benin, Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, T�rkei" },
		     { "Grenznahverkehr", "" }};


  double Tarif[2][19][4] = {{{ 2.376,  1.896, 0.948, 0.66},
			     { 3.168,  2.472, 1.188, 0.948},
			     { 3.96,  3.96,  2.472, 2.472},
			     { 3.96,  3.96,  3.30,  3.30},
			     { 5.28,  5.28,  3.96,  3.96},
			     { 7.26,  7.26,  5.94,  5.94},
			     { 9.24,  9.24,  7.92,  7.92},
			     {11.22, 11.22,  9.90,  9.90},
			     {13.20, 13.20, 11.22, 11.22},
			     {15.18, 15.18, 13.20, 13.20},
			     {17.16, 17.16, 15.84, 15.84},
			     {19.80, 19.80, 18.48, 18.48},
			     {22.44, 22.44, 21.12, 21.12},
			     {25.74, 25.74, 23.76, 23.76},
			     {29.04, 29.04, 26.40, 26.40},
			     {38.94, 38.94, 34.98, 34.98},
			     { 5.94,  5.94,  5.28,  5.28},
			     { 5.28,  5.28,  6.60,  6.60},
			     { 2.64,  2.64,  1.98,  1.98}},

			    {{ 2.532,  2.028, 1.020, 0.708 },
			     { 3.372,  2.640, 1.260, 1.020 },
			     { 4.224,  4.224,  2.640, 2.640 },
			     { 4.224,  4.224,  3.516, 3.516 },
			     { 5.628,  5.628,  4.224, 4.224 },
			     { 7.740,  7.740,  6.336, 6.336 },
			     { 9.852,  9.852,  8.448, 8.448 },
			     {11.964, 11.964, 10.560,10.560 },
			     {14.076, 14.076, 11.964,11.964 },
			     {16.188, 16.188, 14.076,14.076 },
			     {18.300, 18.300, 16.896,16.896 },
			     {21.120, 21.120, 19.716,19.716 },
			     {23.940, 23.940, 22.524,22.524 },
			     {27.456, 27.456, 25.344,25.344 },
			     {30.972, 30.972, 28.164,28.164 },
			     {41.532, 41.532, 37.308,37.308 },
			     { 6.336,  6.336,  5.628, 5.628 },
			     { 5.628,  5.628,  7.044, 7.044 },
			     { 2.820,  2.820,  2.112, 2.112 }}};

  int t,z;

  for (t=0; t<2; t++) {
    rprintf ("P:07,%d", "European Telecom" , t+1);
    rprintf ("# Fixme:", "Verzonung nicht verifiziert" );
    rprintf ("C:Name:", "European Telecom" );
    /*  rprintf ("","C:Address:"); */
    rprintf ("C:Homepage:", "http://www.europeantelecom.at" );
    rprintf ("C:Telefon:", "0800 1007 1007" );
    rprintf ("C:Zone:", "Die Fernzone 1 reicht bis 200 Km alles andere ist Fernzone 2." );
    rprintf ("C:Special", "12-monatige Bindungsdauer." );
    rprintf ("D:1007", "# Verzonung" );
    rprintf ("# Tarif:", Name[t] );
    for (z=0; z<COUNT(Zone); z++) {
      rprintf ("Z:%d", Zone[z][0] , z+1);
      if (Tarif[t][z][0]==Tarif[t][z][1]) {
	rprintf ("T:W/08-18=%.3f(60)/1", "Tageszeit" , Tarif[t][z][0]);
      } else {
	rprintf ("T:W/08-12=%.3f(60)/1", "Tageszeit 1" , Tarif[t][z][0]);
	rprintf ("T:1-4/13-16=%.3f(60)/1", "Tageszeit 1" , Tarif[t][z][0]);
	rprintf ("T:1-4/12-13,16-18=%.3f(60)/1", "Tageszeit 2" , Tarif[t][z][1]);
	rprintf ("T:5/12-18=%.3f(60)/1", "Tageszeit 2" , Tarif[t][z][1]);
      }
      if (Tarif[t][z][2]==Tarif[t][z][3]) {
	rprintf ("T:W/18-8=%.3f(60)/1", "Sparzeit" , Tarif[t][z][2]);
	rprintf ("T:E,H/*=%.3f(60)/1", "Sparzeit" , Tarif[t][z][2]);
      } else {
	rprintf ("T:W/06-08,18-20=%.3f(60)/1", "Sparzeit" , Tarif[t][z][2]);
	rprintf ("T:E,H/06-20=%.3f(60)/1", "Sparzeit" , Tarif[t][z][2]);
	rprintf ("T:*/20-06=%.3f(60)/1", "Supersparzeit" , Tarif[t][z][3]);
      }
      print_area(Zone[z][1]);
    }
  }
}


void rate_1008(void) {

  char *Zone[][2] = {
    { "�stereich", "�sterreich" },
    { "Mobil A1,max","+43664, +43676"},
    { "Mobil D,One", "+43663, +43699"},
    /* 2.5 */	{"Deutschland", "Deutschland"},
    /* 2.98 */	{"Gro�britannien ...", "Gro�britannien, Italien, Liechtenstein, Schweiz, USA"},
    /* 3.58 */	{"Belgien ...", "Belgien, D�nemark, Finnland, Frankreich, Luxemburg, Niederlande, Slowakische Republik, Slowenien, Spanien, Tschechische Republik, Ungarn, Alaska, Kanada, Paris, Hawaii, Irland, Italien Mobilfunk, Kanarische Inseln, Monaco, Norwegen, Schweden"},
    /* 4.78 */	{"Deutschland Mobilfunk ...", "Deutschland Mobilfunk, Schweiz Mobilfunk"},
    /* 5.62 */	{"Israel ...", "Israel, Kroatien, Polen, T�rkei, Albanien, Andorra, Australien, Weissrussland, Belgien Mobilfunk, Bosnien-Herzegowina, Bulgarien, D�nemark Mobilfunk, Estland, F�r�er-Inseln, Frankreich Mobilfunk, Griechenland, Gro�britannien Mobilfunk, Irland Mobilfunk, Amerikanische Jungferninseln, Lettland, Litauen, Malta, Mazedonien, Niederlande Mobilfunk, Norwegen Mobilfunk, Portugal, Puerto Rico, Rum�nien, San Marino, Schweden Mobilfunk, Jugoslawien, Spanien Mobilfunk, Tunesien, Ukraine, Vatikan, Zypern"},
    /* 6.58 */	{"Hongkong ...", "Hongkong, Japan, Algerien, Azoren, Georgien, Gibraltar, Island, Israel Mobilfunk, Marokko, Madeira, Palestina, Russische F�rderation (�stl.), Singapur, Tadschikistan, Turkmenistan, Usbekistan"},
    /* 9.46 */	{"Antarktis ...", "Antarktis, Armenien, Aserbaidschan, Australien Mobilfunk, Chile, Weihnachtsinseln, Kokosinseln, Hong Kong Mobilfunk, Japan Mobilfunk, Libyen, Neuseeland, S�dafrika"},
    /* 11.62 */	{"Bahamas ...", "Bahamas, Bermuda, Brasilien, China, Franz�sisch-Guayana, Guadeloupe, S�dkorea, Macao, Malaysia, Martinique / Franz. Antillen, Mayotte, Mexiko, Moldavien Moldau (Republik), Niederl. Antillen, Philippinen, Reunion, Marianen (SaipanNord-), Saudi Arabien, St. Pierre und Miquelon, Tokelan, Venezuela, Ver. Arabische Emirate"},
    /* 15.58 */	{"Argentinien ...", "Argentinien, �gypten, Angola, Aruba, Bhutan, Brasilien Mobilfunk, Brunei, China Mobilfunk, Dominikanische Republik, Ecuador, Ecuador Mobilfunk, Ghana, Iran, Jordanien, Katar, Kolumbien, Kuwait, St. Vincent und Grenadinen, Taiwan, Trinidad und Tobago"},
    /* 17.98 */	{"�gypten Mobilfunk ...", "�quatorial-Guinea, Bahrain, Barbados, Belize, Botsuana Botswana, Burundi, Gabun, Gr�nland, Guinea, Guinea-Bissau, Guyana, Haiti, Kirgistan, Kolumbien Mobilfunk, Lesotho, Libanon, Libanon Mobilfunk, Liberia, Malawi, Namibia, Panama, Panama Mobilfunk, Peru, Sambia, Tansania, Simbabwe, St. Helena, St. Lucia, Sao Tome und Principe, Sudan, Swasiland, Syrien, Turks- und Caicosinseln"},
    /* 20.38 */	{"Costa Rica ...", "Costa Rica, Indonesien, Irak, Elfenbeink�ste Cote de Ivoire, Jamaika, Kasachstan, Kenia, Laos, Malediven, Nauru, Nepal, Neukaledonien, Nicaragua, Nigeria, Niue-Inseln, Norfolkinseln, Oman, Ruanda, West-Samoa, Thailand, Tonga"},
    /* 21.58 */	{"�thiopien ...", "�thiopien, Argentinien Mobilfunk, Benin, Fidschi, Franz�sisch-Polynesien, Gambia, Indien, Kiribati, Komoren, Mauretanien, Mauritius, Mongolei, Mosambik, Myanmar Burma, Niger, Peru Mobilfunk, St. Kitts und Nevis, Suriname, Taiwan Mobilfunk, Togo, Uganda, Vanuatu, Zentralafrikanische Republik"},
    /* 24.88 */	{"Anguilla ...", "Anguilla, Antigua und Barbuda, Bolivien, Kaimaninseln, Diego Garcia, Dschibuti, El Salvador Mobilfunk, Nordkorea, Kuba, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay"},
    /* 27.58 */	{"Ascension ...", "Ascension, Bangladesch, Burkina Faso Obervolta, Kamerun, Cookinseln, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Honduras, Jemen (Arab. Republik), Kambodscha, Cape Verde, Kongo, Madagaskar, Mali, Montserrat, Pakistan, Salomonen, Senegal, Seyschellen, Somalia, Tschad, Vietnam, Zaire"},
    /* 46.62 */	{"Afghanistan ...", "Afghanistan, Amerikanisch-Samoa, Guantanamo Bay, Marshallinseln, Midway-Inseln, Mikronesien, Palau /Belau, Pitcairn Inseln, Tuvalu, Wake Inseln, Wallis und Futuna"},
  };

  double Tarif[] = {
    0.90,
    3.30, /* a1,max */
    3.60, /* one, D */
    2.50,  /* Deutschland ... */
    2.98,  /* Gro�britannien ... */
    3.58,  /* Belgien ... */
    4.78,  /* Deutschland Mobilfunk ... */
    5.62,  /* Israel ... */
    6.58,  /* Hongkong ... */
    9.46,  /* Antarktis ... */
    11.62,  /* Bahamas ... */
    15.58,  /* Argentinien ... */
    17.98,  /* �gypten Mobilfunk ... */
    20.38,  /* Costa Rica ... */
    21.58,  /* �thiopien ... */
    24.88,  /* Anguilla ... */
    27.58,  /* Ascension ... */
    46.62,  /* Afghanistan ... */

  };
  int z;

  rprintf ("P:08", "TELEforum" );
  rprintf ("C:GT:", "Freischaltgeb�hren ATS 480" );
  rprintf ("C:TarifChanged:", "15.09.1999" );
  rprintf ("C:GF:", "Cost = 480/12" );
  rprintf ("C:Name:", "TELEforum Telekommunikations GmbH" );
  rprintf ("C:Address:", "Sporg. 32, A-8010 Graz" );
  rprintf ("C:Telefon:", "0316 9166 " );
  rprintf ("C:Telefax:", "0316 9166-8 " );
  rprintf ("C:Homepage:", "http://www.teleforum.at" );
  rprintf ("C:TarifURL:", "http://www.teleforum.at/fpreise.htm" );
  rprintf ("C:EMail:", "office@teleforum.at" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1009(void) {

  char *Zone[][2] = {
    { "Regional",""},
    { "Fern", "" },
    { "Fern", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    /* 2.86 */	{"Deutschland", "Deutschland"},
    /* 3.46 */	{"USA", "USA"},
    /* 3.56 */	{"Frankreich ...", "Frankreich, Gro�britannien, Italien, Liechtenstein, Schweden, Schweiz"},
    /* 3.76 */	{"Belgien", "Belgien"},
    /* 3.86 */	{"Niederlande", "Niederlande"},
    /* 3.96 */	{"D�nemark ...", "D�nemark, Finnland, Kanada, Spanien, Ungarn"},
    /* 4.46 */	{"Irland ...", "Irland, Norwegen, Slowenien, Tschechische Republik"},
    /* 4.66 */	{"Monaco", "Monaco"},
    /* 4.76 */	{"Andorra ...", "Andorra, Bosnien-Herzegowina, Vatikan, Kroatien, Luxemburg, Puerto Rico, Amerikanische Jungferninseln"},
    /* 5.16 */	{"Malta ...", "Malta, San Marino, Slowakische Republik"},
    /* 5.36 */	{"Griechenland ...", "Griechenland, Polen"},
    /* 5.56 */	{"Jugoslawien", "Jugoslawien"},
    /* 5.86 */	{"Portugal", "Portugal"},
    /* 5.96 */	{"Albanien ...", "Albanien, Australien, Weissrussland, Bulgarien, Estland, Israel, Mazedonien, Zypern"},
    /* 6.36 */	{"Rum�nien ...", "Rum�nien, T�rkei, Ukraine"},
    /* 6.46 */	{"F�r�er-Inseln ...", "F�r�er-Inseln, Island"},
    /* 6.76 */	{"Tunesien", "Tunesien"},
    /* 6.96 */	{"Gibraltar ...", "Gibraltar, Lettland, Russische F�rderation (�stl.)"},
    /* 7.16 */	{"Antarktis ...", "Antarktis, Weihnachtsinseln, Kokosinseln, Hongkong, Japan, Litauen, Neuseeland, Singapur"},
    /* 7.46 */	{"Algerien", "Algerien"},
    /* 7.76 */	{"S�dkorea", "S�dkorea"},
    /* 7.96 */	{"Marokko", "Marokko"},
    /* 8.36 */	{"Malaysia ...", "Malaysia, Mexiko, Philippinen, St. Pierre und Miquelon, S�dafrika, Tadschikistan"},
    /* 8.96 */	{"Chile ...", "Chile, Georgien, Guadeloupe, Libyen, Moldavien Moldau (Republik), Marianen (SaipanNord-), Turkmenistan, Usbekistan"},
    /* 9.26 */	{"Aserbaidschan", "Aserbaidschan"},
    /* 9.56 */	{"Macao ...", "Macao, Martinique / Franz. Antillen, Mayotte, Niederl. Antillen"},
    /* 9.96 */	{"Armenien ...", "Armenien, Franz�sisch-Guayana"},
    /* 10.16 */	{"Amerikanisch-Samoa", "Amerikanisch-Samoa"},
    /* 10.76 */	{"Bermuda ...", "Bermuda, Dominikanische Republik, Reunion"},
    /* 11.36 */	{"China", "China"},
    /* 11.96 */	{"Angola ...", "Angola, Bahamas, Brasilien, Ecuador, Kolumbien, Panama, Taiwan, Thailand, Venezuela"},
    /* 13.16 */	{"�gypten ...", "�gypten, Brunei, Gr�nland, St. Vincent und Grenadinen, Trinidad und Tobago, Ver. Arabische Emirate"},
    /* 13.96 */	{"Barbados ...", "Barbados, Botsuana Botswana, Ghana, Jordanien"},
    /* 14.36 */	{"Aruba ...", "Aruba, Indien, Indonesien, Iran, Kirgistan, Kuwait, Libanon, Liberia, Peru, Katar, Sudan, Syrien, Tansania"},
    /* 14.96 */	{"Bhutan ...", "Bhutan, Gabun, Saudi Arabien, Senegal"},
    /* 15.56 */	{"Argentinien ...", "Argentinien, Costa Rica, Guinea, Guinea-Bissau, Kasachstan, Nordkorea, Kuba, Lesotho, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Norfolkinseln, Papua-Neuguinea, Paraguay, Sambia, West-Samoa, Simbabwe, Swasiland, Turks- und Caicosinseln"},
    /* 15.96 */	{"Bahrain ...", "Bahrain, Laos, Nigeria, St. Helena, St. Lucia"},
    /* 16.76 */	{"Ascension ...", "Ascension, Belize, Bolivien, Burundi, Dominica, El Salvador, Fidschi, Grenada, Guatemala, Guyana, Jamaika, Kenia, Malediven, Mauretanien, Mauritius, Montserrat, Nicaragua, Somalia, Uruguay, Vietnam"},
    /* 16.96 */	{"Komoren ...", "Komoren, Sao Tome und Principe"},
    /* 17.96 */	{"�quatorial-Guinea ...", "�quatorial-Guinea, �thiopien, Afghanistan, Anguilla, Antigua und Barbuda, Bangladesch, Benin, Burkina Faso Obervolta, Kaimaninseln, "
		 "Cookinseln, Elfenbeink�ste Cote de Ivoire, Dschibuti, Eritrea, Falklandinseln, Franz�sisch-Polynesien, Gambia, Guam, Guantanamo Bay, Haiti, Honduras, Irak, Jemen (Arab. Republik), "
		 "Kambodscha, Kamerun, Cape Verde, Kiribati, Kongo, Madagaskar, Mali, Marshallinseln, Mikronesien, Mongolei, Mosambik, Myanmar Burma, Niger, Niue-Inseln, Oman, Pakistan, Palau /Belau, "
		 "Ruanda, Salomonen, Seyschellen, Sierra Leone, Sri Lanka, St. Kitts und Nevis, Suriname, Togo, Tonga, Tschad, Tuvalu, Uganda, Vanuatu, Britische Jungferninseln, Wake Inseln, Zaire, Zentralafrikanische Republik"},

  };

  double Tarif[] = {
    5999.4, /* 99.99*60 no Z1*/
    1.46,
    1.46,
    3.98, /* Mobil */
    2.86,  /* Deutschland ... */
    3.46,  /* USA ... */
    3.56,  /* Frankreich ... */
    3.76,  /* Belgien ... */
    3.86,  /* Niederlande ... */
    3.96,  /* D�nemark ... */
    4.46,  /* Irland ... */
    4.66,  /* Monaco ... */
    4.76,  /* Andorra ... */
    5.16,  /* Malta ... */
    5.36,  /* Griechenland ... */
    5.56,  /* Serbien ... */
    5.86,  /* Portugal ... */
    5.96,  /* Albanien ... */
    6.36,  /* Rum�nien ... */
    6.46,  /* Faroer Inseln ... */
    6.76,  /* Tunesien ... */
    6.96,  /* Gibraltar ... */
    7.16,  /* Antarctica Terr. Davis, Maws ... */
    7.46,  /* Algerien ... */
    7.76,  /* Korea S�d ... */
    7.96,  /* Marokko ... */
    8.36,  /* Malaysia ... */
    8.96,  /* Chile ... */
    9.26,  /* Aserbaidschan ... */
    9.56,  /* Macao ... */
    9.96,  /* Armenien ... */
    10.16,  /* Samoa US ... */
    10.76,  /* Bermuda ... */
    11.36,  /* China ... */
    11.96,  /* Angola ... */
    13.16,  /* �gypten ... */
    13.96,  /* Barbados ... */
    14.36,  /* Aruba ... */
    14.96,  /* Bhutan ... */
    15.56,  /* Argentinien ... */
    15.96,  /* Bahrain ... */
    16.76,  /* Ascension Island ... */
    16.96,  /* Komoren ... */
    17.96  /* �quatorialguinea ... */

  };
  int z;

  rprintf ("P:09", "Vocalis" );
  rprintf ("D:pta", "# Verzonung" );
  printf ("# Verzonung nicht verifiziert\n");
  rprintf ("C:TarifChanged:", "01.09.1999" );
  rprintf ("C:Name:", "Vocalis Telekom-Dienste GmbH" );
  rprintf ("C:Special:", "34 g Gespr�chs-Herstellungsgeb�hr" );
  rprintf ("C:Homepage:", "http://www.vocalis.at" );
  rprintf ("C:TarifURL:", "http://www.vocalis.at/voc_tarife_frame.htm" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=0.34/0,%.2f(60)/1", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1011(void) {

  char *Zone[][2] = {{ "Bundesland", "" },
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     {"Deutschland", "Deutschland"},
		     {"Ausland 1", "Frankreich, Gro�britannien, Italien, Liechtenstein, Paraguay, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn, Vatikanstadt"},
		     {"Ausland 2", "Belgien, D�nemark, Finnland, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Papua-Neuguinea, San Marino, Schweden, Spanien, Vereinigte Staaten (USA)"},
		     {"Ausland 3", "Andorra, Bosnien-Herzegowina, Bulgarien, Kroatien, Malediven, Zypern"},
		     {"Ausland 4", "Wei�ru�land, F�r�er-Inseln, Griechenland, Jugoslawien, Britische Jungferninseln, Kanarische Inseln, Palau, Rum�nien, Tunesien, T�rkei, Ukraine"},
		     {"Ausland 5", "Albanien, Algerien, Antarktis, Australien, Estland, Lettland, Malawi, Malaysia, Marokko, Moldau"},
		     {"Ausland 6", "Gibraltar, Island, Israel"},
		     {"Ausland 7", "Litauen, Russische F�deration, Tadschikistan"},
		     {"Ausland 8", "Georgien, Hongkong, Japan, S�dkorea, Neuseeland, Singapur, Turkmenistan, Usbekistan"},
		     {"Ausland 9", "Armenien, Aserbaidschan, Weihnachtsinseln, Kokosinseln, Mauritius, Mexiko"},
		     {"Ausland 10", "Chile, Franz�sisch-Guayana, Guadeloupe, Libyen, Peru, Reunion, St. Pierre und Miquelon, S�dafrika"},
		     {"Ausland 11", "Brasilien, Niederl�ndische Antillen"},
		     {"Ausland 12", "Angola, Bermuda, Bhutan, China, Ecuador, Iran, Kuwait, Mali, Saudi-Arabien, Venezuela, Vereinigte Arabische Emirate"},
		     {"Ausland 13", "Bahamas, Dominikanische Republik, Madagaskar, �gypten"},
		     {"Ausland 14", "Aruba, Barbados, Belize, Botswana, Brunei, Burundi, El Salvador, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Katar, Kirgisistan, Kolumbien, Libanon, Martinique, Mayotte, Sao Tome und Principe, Saint Vincent und die Grenadinen, Taiwan, Trinidad und Tobago, �quatorial-Guinea"},
		     {"Ausland 15", "Argentinien, Bahrain, Benin, Cote d'Ivoire, Dominica, Grenada, Guinea, Kasachstan, Kenia, Komoren, Laos, Lesotho, Liberia, Mauretanien, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Sambia, Samoa (West), Simbabwe, St. Helena, Saint Kitts und Nevis, St. Lucia, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln"},
		     {"Ausland 16", "Mikronesien, �thiopien"},
		     {"Ausland 17", "Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Indonesien, Irak, Jamaika, Kiribati, Nicaragua, Niue, Oman, Panama, Philippinen, Ruanda, Amerikanisch-Samoa, Suriname, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik"},
		     {"Ausland 18", "Afghanistan, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso, Cook-Inseln, Caymaninseln, Dschibuti, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Honduras, Indien, Jemen, Kambodscha, Kamerun, Kap Verde, Kongo, Nordkorea, Kuba, Macao, Malta, Marshallinseln, Mazedonien, Niger, Pakistan, Salomonen, Senegal, Seychellen, Sierra Leone, Somalia, Sri Lanka, Tschad, Tuvalu, Uruguay, Vietnam, Wallis- und Futuna-Inseln, Zaire"}};

  double Tarif[][2] = {{ 0.90, 0.88 },
		       { 0.90, 0.88 },
		       { 3.36, 3.36 },
		       { 3.00, 3.00 },
		       { 3.60, 3.60 },
		       { 4.20, 4.20 },
		       { 4.80, 4.80 },
		       { 5.40, 5.40 },
		       { 6.00, 6.00 },
		       { 6.60, 6.60 },
		       { 7.20, 7.20 },
		       { 7.80, 7.80 },
		       { 8.40, 8.40 },
		       { 9.60, 9.60 },
		       { 12.00, 12.00 },
		       { 13.20, 13.20 },
		       { 14.40, 14.40 },
		       { 15.60, 15.60 },
		       { 18.00, 18.00 },
		       { 19.20, 19.20 },
		       { 20.40, 20.40 },
		       { 24.00, 24.00 }};

  int z;

  rprintf ("P:11", "RSL COM" );
  rprintf ("D:1012", "# Verzonung" );
  rprintf ("C:Name:", "RSL COM Austria AG" );
  rprintf ("C:Address:", "Marktstr. 3, A-7000 Eisenstadt" );
  rprintf ("C:Homepage:", "http://www.rslcom.at" );
  rprintf ("C:EMail:", "info@rslcom.at" );
  rprintf ("C:Telefon:", "0800 008000" );
  rprintf ("C:Zone:", "Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der " );
  rprintf ("C:Zone:", "Bundelandzone sonst in der Zone �sterreich." );
  rprintf ("C:Special:", "F�r TNC-Kunden nicht verwendbar" );
/*  rprintf ("C:GT:", "Grundgeb�hr ATS 69, bei Rechnung < 200 ATS." );
  rprintf ("C:GF:", "Cost = Ch >= 200 ? 0 : 69" ); ab 1.10 nix */

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_10elf(void) {

  char *Zone[][2] = {{ "Bundesland", "+43" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "TransAlp", "Deutschland, Italien, Schweiz" },
		     { "TransEuro1", "Andorra, Belgien, D�nemark, Deutschland Mobil, F�r�er Inseln, Finnland, Frankreich, Gibraltar, Gro�britannien, Irland, Island, Liechtenstein, Luxemburg, Monaco, Niederlande, Norwegen, Polen, San Marino, Schweden, Slowakei, Slowenien, Spanien (einschlie�lich Kanar. Inseln), Tschechische Rep., Ungarn, Zypern" },
		     { "TransEuro2","Albanien, Bosnien-Herzegowina, Bulgarien, Estland, Griechenland, Israel, Jugoslawien, Kroatien, Lettland, Libyen, Litauen, Malta, Marokko, Mazedonien, Moldau, Portugal (einschlie�lich Azoren und Madeira), Rum�nien, Ru�land, T�rkei, Ukraine, Wei�ru�land" },
		     { "TransAmerika","Kanada, Vereinigte Staaten"},
		     { "TransPazifik","Australien, Hong Kong, Japan, Korea (S�d), Malaysia, Neuseeland, Singapur"},
		     { "TransWelt","+"}};

  double Tarif[][2] = {{ 0.90, 0.90 },
		       { 4.00, 3.50 },
		       { 2.75, 2.75 },
		       { 3.50, 3.50 },
		       { 6.50, 6.50 },
		       { 3.00, 3.00 },
		       { 8.00, 8.00 },
		       { 20.00,20.00 }};
  int z;

  rprintf ("P:11,1", "TNC 10elf" );
  rprintf ("C:Name:", "Transnet Communications" );
//  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.10elf.at" );
  rprintf ("C:TarifURL:", "http://www.10elf.at/deutsch/festnetzi.htm" );
  rprintf ("C:EMail:", "info@10elf.at" );
//  rprintf ("C:Telefon:", "" );
  rprintf ("C:Special:", "F�r RSL-COM-Kunden nicht verwendbar" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1012(what) { /* 1012 == 1044 (Citykom) */

  char *Zone[][2] = {
    { "�sterreich", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    { "Deutschland spezial", "Deutschland" },
    { "Nachbarn", "Frankreich, Italien, Schweiz, Liechtenstein, Slowakei, Slowenien, Tschechien, Ungarn" },
    { "Europa 1 + USA", "Belgien, D�nemark, Finnland, Gro�britannien, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Schweden, Spanien, Vereinigte Staaten (USA)" },
    { "Europa 2", "Andorra, Bosnien-Herzegowina, F�r�er-Inseln, Jugoslawien, Kroatien, Malta, Monaco" },
    { "Europa 3", "Albanien, Bulgarien, Estland, Griechenland, Mazedonien, Polen, Portugal, Rum�nien, San Marino, Tunesien, Ukraine,Wei�ru�land, Zypern" },
    { "Welt 1", "Algerien, Australien, Gibraltar, Hongkong, Island, Israel, Japan, Lettland, Litauen, Marokko, Neuseeland, Russische F�deration, Singapur, S�dkorea, T�rkei" },
    { "Welt 2", "Bermuda,Antigua und Barbuda,St. Lucia,Dominikanische Republik,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,Puerto Rico,Anguilla,Bahamas,Jamaika,St. Vincent und Grenadinen,Montserrat,St. Kitts und Nevis,Grenada,Trinidad und Tobago,Hawaii,Marianen (SaipanNord-),Kaimaninseln,Amerikanische Jungferninseln,Barbados,+" }};

  double Tarif[] = { 0.90 ,
		     4.00 ,
		     2.90 ,
		     3.70 ,
		     4.40 ,
		     5.60 ,
		     6.10 ,
		     7.90 ,
		     24.90};


  int z;
  if(what==1012) {
    rprintf ("P:12", "1012" );
    rprintf ("C:TarifChanged:", "14.09.1999" );
    rprintf ("C:Name:", "1012 privat" );
    rprintf ("C:Address:", "Linzer Str. 221, A-1140 Wien" );
    rprintf ("C:Homepage:", "http://www.1012privat.at" );
    rprintf ("C:Telefon:", "0800 1012 1012" );
  }
  else {
    rprintf ("P:44", "Citykom" );
    rprintf ("C:TarifChanged:", "15.09.1999" );
    rprintf ("C:Name:", "Citykom Austria Telekommunikations GmbH" );
    rprintf ("C:Address:", "Alserbachstr. 144-16, A-1090 Wien" );
    rprintf ("C:Homepage:", "http://www.citykom.at" );
    rprintf ("C:EMail:", "j.schmolinsk@citykom.at" );
    rprintf ("C:Telefon:", "0800 201 301" );
    rprintf ("C:Special:", "Mit 1012privat verschmolzen" );
  }
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1012_old(void) {

  char *Zone[][2] = {{ "Nah", "" },
		     { "Fern", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Nachbarn", "Frankreich, Italien, Schweiz, Liechtenstein, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 1 + USA", "Belgien, D�nemark, Finnland, Gro�britannien, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Schweden, Spanien, Vereinigte Staaten (USA)" },
		     { "Europa 2", "Andorra, Bosnien-Herzegowina, F�r�er-Inseln, Jugoslawien, Kroatien, Malta, Monaco" },
		     { "Europa 3", "Albanien, Bulgarien, Estland, Griechenland, Mazedonien, Polen, Portugal, Rum�nien, San Marino, Tunesien, Ukraine,Wei�ru�land, Zypern" },
		     { "Welt 1", "Algerien, Australien, Gibraltar, Hongkong, Island, Israel, Japan, Lettland, Litauen, Marokko, Neuseeland, Russische F�deration, Singapur, S�dkorea, T�rkei" },
		     { "Welt 2", "+" }};

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

  rprintf ("P:12", "1012" );
  rprintf ("C:Name:", "1012 privat" );
  rprintf ("C:Address:", "Linzer Str. 221, A-1140 Wien" );
  rprintf ("C:Homepage:", "http://www.1012privat.at" );
  rprintf ("C:Telefon:", "0800 1012 1012" );
  rprintf ("C:Zone:", "Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der " );
  rprintf ("C:Zone:", "Nahzone sonst in der Fernzone." );
  /*  rprintf ("","C:Special"); */

  rprintf ("D:1012", "# Verzonung" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (z<3) {
      rprintf ("T:W/8-18=%.2f(60)/60/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/60/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/60/1", "Weekend" , Tarif[z][1]);
    } else {
      rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z][0]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1013(void) {
  rprintf ("P:13", "NETnet" );
  rprintf ("C:Name:", "NETnet Telekommunikation GmbH" );
  rprintf ("C:Special:", "Nur Businesskunden" );
  rprintf ("C:Address:", "Inkustr. 2-7, A-3400 Klosterneuburg" );
  rprintf ("C:Homepage:", "http://www.netnet.at" );
  rprintf ("C:EMail:", "customercare@netnet.co.at" );
}

void rate_1014(void) {

  char *Zone[][2] = {
    { "Nah", "" },
    { "Fern", "�sterreich" },
  };
  double Tarif[][2] = {
    { 1.50, 0.60}, /* Fixme: */
    { 1.50, 0.60},
  };
  int z;

  rprintf ("P:14", "MCN" );
  rprintf ("D:pta", "# Verzonung" );
  rprintf ("C:Name:", "Millenium Communication Network GmbH" );
  rprintf ("C:Address:", "Handelskai 94 - 96, A-1200 Wien" );
  rprintf ("C:Homepage:", "http://www.mcn-tower.com" );
  rprintf ("C:Telefon:", "0800 1014 1014" );
  rprintf ("C:Telefax:", "01 207 1099" );
  rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, alles andere ist �sterreichzone." );
  rprintf ("C:Special:", "Kostenlose MCN-W�hlbox" );
  rprintf ("# Fixme:", "Ausland nicht implemtiert" );
  rprintf ("C:Special:", "Ausland nicht implemtiert" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Tag" , Tarif[z][0], Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1], Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1], Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1024(void) {

  char *Zone[][2] = {{ "RundUm-Wien", "" },
		     { "RundUm-Regional", "" },
		     { "Ganzland", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "WeltWeit 1", "Deutschland, Schweiz" },
		     { "WeltWeit 2", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Norwegen, Schweden, Slowenien, Slowakei, Spanien, Tschechien, Ungarn, Vatikan, Vereinigte Staaten (USA), Puerto Rico, Alaska, Hawaii, Kanada" },
		     { "WeltWeit 3", "Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Griechenland, Island, Kroatien, Lettland, Litauen, Malta, Mazedonien, Polen, Portugal, Rum�nien, Serbien, Zypern, Australien, F�r�er, Israel, Neuseeland, Amerikanische Jungferninseln" },
		     { "WeltWeit 4", "Albanien, Weihnachtsinseln, Gibraltar, Kokosinseln, Ru�land, San Marino, T�rkei, Tunesien, Ukraine, Wei�ru�land, Algerien, Hongkong, Japan, S�dkorea, Marokko, Philippinen, Singapur" },
		     { "WeltWeit 5", "�gypten, S�dafrika, Aserbaidschan, Armenien, China, Georgien, Libyen, Malaysia, Moldau, Tadschikistan, Taiwan, Turkmenistan, Usbekistan, Argentinien, Brasilien, Chile, Costa Rica, Dominikanische Republik, Ecuador, El Salvador, Guatemala, Kolumbien, Mexiko, Nicaragua, Panama, Peru, Venezuela" },
		     { "WeltWeit 6", "Angola, Aruba, Bahamas, Bahrain, Barbados, Bermuda, Bhutan, Bolivien, Botswana, Brunei, Burundi, Ghana, Gr�nland, Guadeloupe, Guam, Guinea, Martinique, Franz�sisch-Guayana, Honduras, Indien, Indonesien, Iran, Jordanien, Kasachstan, Kirgisistan, Kuba, Kuwait, Lesotho, Libanon, Liberia, Macao, Malawi, Mayotte, Namibia, Niederl�ndische Antillen, Nigeria, Paraguay, Reunion, Saipan, Saudi-Arabien, St. Lucia, St. Pierre und Miquelon, Saint Vincent und die Grenadinen, Sudan, Syrien, Swasiland, Tansania, Thailand, Trinidad und Tobago, Uruguay, Vereinigte Arabische Emirate" },
		     { "WeltWeit 7", "Antigua und Barbuda,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,Anguilla,Jamaika,Montserrat,St. Kitts und Nevis,Grenada,Kaimaninseln,+" }};

  double Tarif[][2] = {{ 0.98, 0.88},
		       { 1.28, 1.18},
		       { 1.98, 1.18},
		       { 3.98, 3.98},
		       { 2.98, 2.98}, /* W1 */
		       { 3.98, 3.98},
		       { 5.98, 5.98},
		       { 7.38, 7.38},
		       {12.98,12.98},
		       {14.98,14.98},
		       {19.98,19.98}};

  int z;

  rprintf ("P:24", "Telepassport" );
  rprintf ("C:Maintainer:", "Leopold Toetsch <lt@toetsch.at>" );
  rprintf ("C:Name:", "TelePassport Telekommunikationsdiensleitungen GmbH" );
  rprintf ("C:Address:", "Prinz-Eugen-Str. 10, A 1040 Wien" );
  rprintf ("C:Homepage:", "http://www.telepassport.at" );
  rprintf ("C:Hotline:", "0800 80 1024" );
  rprintf ("C:EMail:", "info@telepassport.at" );
  rprintf ("C:Telefon:", "01 506 44-100" );
  rprintf ("C:Telefax:", "01 506 44-199" );
  rprintf ("C:Zone:", "Vorwahlen 1XXX und 2XXX, sind in der Zone RundUm-Wien, andere Vorwahlen "
	   "mit gleicher Anfangsziffer liegen in der Zone RundUm-Regional, der Rest ist "
	   "Ganzland.");
  rprintf ("D:1024", "# Verzonung" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=%.2f(60)/1", "Tag" , Tarif[z][0], Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1], Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1], Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1029_privat(void) {

  char *Name =  "easy.Talk privat" ;

  double Einheit = 67;

  char *Zone[][2] = {{ "Regionalzone", "" },
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "Mobilfunk A1, max", "+43663,+43664,+43676" },
		     { "Mobilfunk One", "+43699" },
		     { "Deutschland", "Deutschland, Liechtenstein, Schweiz, Italien" },
		     { "International 1", "Frankreich, Grossbritannien, Slowakei, Slowenien, Tschechische Republik, Ungarn" },
		     { "International 2", "Andorra, Belgien, Bosnien-Herzegowina, D�nemark, Finnland, Irland, Kanada, Kroatien, Luxemburg, Malta, Monaco, Niederlande, Norwegen, Polen, Schweden, Spanien, USA, Vatikan, Zypern" },
		     { "International 3-1","Albanien,Belarus,Bulgarien,Mazedonien,Rum�nien,San Marino,Serbien,Ukraine"},
		     { "International 3-2","Estland, F�r�er Inseln, Gibraltar, Griechenland, Island, Israel,Portugal, Puerto Rico, Russland, Tunesien, T�rkei"},
		     { "International 4","Angola, Bermuda, Brasilien, Chile, Franz. Guyana, Guadeloupe, Hong Kong, Japan, Libyen, Macao, Madeira, Malaysia, Martinique, Mayotte, Mexico, Moldawien, Neu Seeland, Niederl. Antillen, Philippinen, Reunion, Saipan, Singapur, St. Pierre und Miquelon, S�dafrika, S�dkorea, Venezuela"},
		     { "International 5","�gypten, Aruba, Bahamas, Barbados, Belize, Bhutan, Botswana, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Gr�nland, Guyana, Iran, Jordanien, Kirgisistan, Kolumbien, Kuweit, Libanon, Panama, Saudiarabien, St. Vincent und Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate"},
		     { "International 6","Argentinien, �thiopien, Bahrain, Benin, Costa Rica, Elfenbeink�ste, Franz�sisch-Polynesien, Gambia, Guinea, Guinea-Bissau, Jamaica, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mozambique, Namibia, Nauru, Nepal, Neu Kaledonien, Nicaragua, Nigeria, Niue, Norfolk Inseln, Oman, Peru, Ruanda, Sambia, Samoa West, Simbabwe, St. Helena, St. Lucia, Sao Tome und Principe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks & Caicos Islands, Uganda, Vanuatu, Zentralafrikanische Republik"},
		     { "International 7","Anguilla, Antigua & Barbuda, Ascension, Bangla Desh, Bolivien, Burkina Faso, Myanmar/Burma, Cayman Inseln, Dominica, Dschibuti, El Salvador, Eritrea, Falkland Inseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Irak, Jemen, Jungfern Inseln britisch, Kambodscha, Kamerun, Kap Verde, Kongo, Kuba, Mali, Mauritius, Mongolei, Montserrat, Niger, Nordkorea, Pakistan, Papua Neuguinea, Paraguay, Salomonen, Senegal, Seychellen, Sierra Leone, Somalia, Sri Lanka, St. Kitts & Nevis, Tschad, Uruguay, Vietnam, Zaire"},
		     { "International 8","Algerien, Georgien, Lettland, Litauen, Marokko, Tadschikistan, Turkmenistan, Usbekistan"},
		     { "International 9","Armenien, Aserbaidschan, Christmas Island, Kokos Inseln"},

		     { "International 10", "Hawaii,Amerikanische Jungferninseln,+" }}; /* Afghanistan, Cook Inseln, Diego Garcia, Guantanamo, Madagaskar, Marshall Inseln, Micronesia, Midway Inseln, Palau, Samoa Ost, Tuvalu, Wake, Wallis/Futuna */


  double Tarif[][4] = {{ 0.87, 0.84, 0.57, 0.39}, /* Fixme: 1.9. */
		       { 1.52, 1.52, 1.21, 0.87},/* Fixme: 1.9. */
		       { 1.52, 1.52, 1.51, 1.26},/* Fixme: 1.9. */
		       { 3.96, 3.96, 3.25, 3.25}, /* A1/max */
		       { 4.46, 4.46, 3.14, 3.14}, /* mobil */
		       { 2.60, 2.60, 2.60, 2.60}, /* DE */
		       { 4.00, 4.00, 4.00, 4.00}, /* int 1 */
		       { 5.25, 5.25, 5.25, 5.25}, /* int 2 */
		       { 7.0, 7.0, 5.25, 5.25 }, /* int 3 +spar */
		       { 7.0, 7.0, 7.00, 7.00 }, /* int 3 -2 */
		       { 13.10, 13.10, 13.10, 13.10 }, /* int 4 */
		       { 17.50, 17.50, 17.50, 17.50 }, /* int 5 */
		       { 20.10, 20.10, 20.10, 20.10 }, /* int 6 */
		       { 24.50, 24.50, 24.50, 24.50 }, /* int 7 */
		       {  8.90,  8.90,  7.90,  7.90 }, /* int 8 */
		       { 10.50, 10.50, 10.50, 10.50 }, /* int 9 */
		       { 40.14, 40.14, 40.14, 40.14}, /* int 10 */
		       { 0.00, 0.00, 0.00, 0.00}};


  int z;
  char s[BUFSIZ];

  rprintf ("P:29,1", "CyberTron easy.Talk privat" );

  rprintf ("C:Name:", "Cybertron EDV- NetzwerkbetriebsgmbH" );
  rprintf ("C:Address:", "Davidstr. 79, A-1100 Wien" );
  rprintf ("C:Homepage:", "http://www.cybertron.at" );
  rprintf ("C:TarifURL:", "http://www.cybertron.at/html/easy.prod_1.html" );
  rprintf ("C:Telefon:", "0800 400 400, 1029 1029" );
  rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, die Fernzone 1 bis " );
  rprintf ("C:Zone:", "200 Km alles andere ist Fernzone 2." );
  rprintf ("C:Special:", "Variable Taktung." );

  rprintf ("D:pta", "# Verzonung" );
  sprintf (s, "%s (ATS %.3f pro Einheit)", Name, Einheit);
  rprintf ("# Tarif:", s );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]!=0.0) { /* Tarif bekannt? */
      if (Tarif[z][0]==Tarif[z][1] && Tarif[z][0]==Tarif[z][2] && Tarif[z][0]==Tarif[z][3]) {
	rprintf ("T:*/*=%.2f(60)/%.0f", "0-24h" , Tarif[z][0], Einheit/Tarif[z][0]);
      } else {
	if (Tarif[z][0]==Tarif[z][1]) {
	  rprintf ("T:W/8-18=%.2f(60)/%.0f", "Tageszeit" , Tarif[z][0], Einheit/Tarif[z][0]);
	} else {
	  rprintf ("T:W/8-12=%.2f(60)/%.0f", "Tageszeit 1" , Tarif[z][0], Einheit/Tarif[z][0]);
	  rprintf ("T:1-4/13-16=%.2f(60)/%.0f", "Tageszeit 1" , Tarif[z][0], Einheit/Tarif[z][0]);
	  rprintf ("T:1-4/12-13,16-18=%.2f(60)/%.0f", "Tageszeit 2" , Tarif[z][1], Einheit/Tarif[z][1]);
	  rprintf ("T:5/12-18=%.2f(60)/%.0f", "Tageszeit 2" , Tarif[z][1], Einheit/Tarif[z][1]);
	}
	if (Tarif[z][2]==Tarif[z][3]) {
	  rprintf ("T:*/*=%.2f(60)/%.0f", "Sparzeit" , Tarif[z][2], Einheit/Tarif[z][2]);
	} else {
	  rprintf ("T:W/06-08,18-20=%.2f(60)/%.0f", "Sparzeit" , Tarif[z][2], Einheit/Tarif[z][2]);
	  rprintf ("T:E,H/06-20=%.2f(60)/%.0f", "Sparzeit" , Tarif[z][2], Einheit/Tarif[z][2]);
	  rprintf ("T:*/20-06=%.2f(60)/%.0f", "Supersparzeit" , Tarif[z][3], Einheit/Tarif[z][3]);
	}
      }
    }
    print_area(Zone[z][1]);
  }
}

void rate_1029_business(void) {

  char *Zone[][2] = {{ "Regionalzone", "" },
		     { "Fernzone", "�sterreich" },
		     { "Mobilfunk A1, max", "+43663,+43664,+43676" },
		     { "Mobilfunk One", "+43699" },
		     /* 3.24 */	{"Deutschland", "Deutschland"},
		     /* 3.48 */	{"Liechtenstein ...", "Liechtenstein, Schweiz"},
		     /* 3.60 */	{"Italien ...", "Italien, Ungarn"},
		     /* 3.96 */	{"Frankreich ...", "Frankreich, Gro�britannien, Gro�britannien Mobilfunk, Kanada, Slowakische Republik, Slowenien, Tschechische Republik"},
		     /* 4.20 */	{"Spanien ...", "Spanien, USA"},
		     /* 4.32 */	{"Belgien ...", "Belgien, Niederlande, Norwegen, Schweden"},
		     /* 4.68 */	{"D�nemark ...", "D�nemark, Finnland, Irland, Luxemburg, Malta"},
		     /* 4.74 */	{"Monaco ...", "Monaco, Vatikan"},
		     /* 4.80 */	{"Bosnien-Herzegowina", "Bosnien-Herzegowina"},
		     /* 4.86 */	{"Polen", "Polen"},
		     /* 5.04 */	{"Griechenland ...", "Griechenland, Kroatien"},
		     /* 5.16 */	{"Andorra", "Andorra"},
		     /* 5.22 */	{"Amerikanische Jungferninseln", "Amerikanische Jungferninseln"},
		     /* 5.28 */	{"Puerto Rico", "Puerto Rico"},
		     /* 5.52 */	{"Jugoslawien", "Jugoslawien"},
		     /* 5.94 */	{"Bulgarien", "Bulgarien"},
		     /* 6.06 */	{"Mazedonien", "Mazedonien"},
		     /* 6.18 */	{"Albanien ...", "Albanien, Weissrussland, Rum�nien, San Marino, Tunesien, T�rkei, Ukraine"},
		     /* 6.24 */	{"Israel", "Israel"},
		     /* 6.30 */	{"Gibraltar ...", "Gibraltar, Portugal"},
		     /* 6.36 */	{"Australien ...", "Australien, Estland, F�r�er-Inseln, Lettland, Litauen"},
		     /* 6.38 */	{"Russische F�rderation (�stl.) ...", "Russische F�rderation (�stl.), Moskau, St. Petersburg"},
		     /* 6.42 */	{"Island", "Island"},
		     /* 7.56 */	{"Hongkong ...", "Hongkong, Japan, Japan Mobilfunk, Tokyo"},
		     /* 7.80 */	{"Algerien ...", "Algerien, Marokko"},
		     /* 7.98 */	{"Madeira", "Madeira"},
		     /* 8.04 */	{"Weihnachtsinseln ...", "Weihnachtsinseln, Kokosinseln"},
		     /* 8.16 */	{"Singapur", "Singapur"},
		     /* 8.52 */	{"Georgien ...", "Georgien, Tadschikistan, Turkmenistan, Usbekistan"},
		     /* 9.48 */	{"S�dafrika", "S�dafrika"},
		     /* 9.54 */	{"Libyen ...", "Libyen, Neuseeland"},
		     /* 9.66 */	{"Chile ...", "Chile, Martinique / Franz. Antillen, Moldavien Moldau (Republik), St. Pierre und Miquelon"},
		     /* 9.72 */	{"Franz�sisch-Guayana ...", "Franz�sisch-Guayana, Guadeloupe, Mayotte, Reunion"},
		     /* 9.96 */	{"Armenien", "Armenien"},
		     /* 10.44 */	{"Aserbaidschan", "Aserbaidschan"},
		     /* 11.16 */	{"Malaysia ...", "Malaysia, Niederl. Antillen, Philippinen, S�dkorea"},
		     /* 11.28 */	{"Marianen (SaipanNord-)", "Marianen (SaipanNord-)"},
		     /* 11.76 */	{"Brasilien", "Brasilien"},
		     /* 12.78 */	{"Bermuda ...", "Bermuda, Macao, Mexiko, Venezuela"},
		     /* 12.84 */	{"China", "China"},
		     /* 12.96 */	{"Angola ...", "Angola, Bhutan, Ecuador, Iran, Saudi Arabien"},
		     /* 13.32 */	{"�gypten", "�gypten"},
		     /* 14.40 */	{"Aruba ...", "Aruba, Bahamas, Brunei, Dominikanische Republik, Ghana, Jordanien, Kolumbien, Kolumbien Mobilfunk, Kuwait, St. Vincent und Grenadinen, Trinidad und Tobago"},
		     /* 14.52 */	{"Panama ...", "Panama, Ver. Arabische Emirate"},
		     /* 14.58 */	{"Barbados ...", "Barbados, Botsuana Botswana, Gr�nland"},
		     /* 14.64 */	{"Belize ...", "Belize, Kirgistan"},
		     /* 15.84 */	{"Libanon", "Libanon"},
		     /* 15.96 */	{"Gabun", "Gabun"},
		     /* 16.56 */	{"Bahrain ...", "Bahrain, Katar, Lesotho, Liberia, Namibia, Peru, Sambia, Simbabwe, St. Helena, St. Lucia, Sudan, Swasiland, Tansania, Turks- und Caicosinseln"},
		     /* 16.68 */	{"Nigeria ...", "Nigeria, Samoa (West)"},
		     /* 16.74 */	{"Burundi ...", "Burundi, Elfenbeink�ste Cote de Ivoire, Guinea, Kasachstan, Laos, Nauru, Neukaledonien"},
		     /* 16.92 */	{"Nepal ...", "Nepal, Syrien"},
		     /* 17.28 */	{"Guyana", "Guyana"},
		     /* 18.12 */	{"Guinea-Bissau ...", "Guinea-Bissau, Sao Tome und Principe"},
		     /* 18.30 */	{"Nicaragua", "Nicaragua"},
		     /* 18.56 */	{"Malawi", "Malawi"},
		     /* 18.66 */	{"Argentinien ...", "Argentinien, Benin, Costa Rica, Franz�sisch-Polynesien, Gambia, Jamaika, Kenia, Malediven, Niue-Inseln, Oman, Ruanda, Taiwan, Thailand, Uganda, Vanuatu"},
		     /* 18.74 */	{"Norfolkinseln", "Norfolkinseln"},
		     /* 18.78 */	{"Wallis und Futuna", "Wallis und Futuna"},
		     /* 18.90 */	{"Fidschi", "Fidschi"},
		     /* 18.96 */	{"�thiopien ...", "�thiopien, Irak, Kiribati, Komoren, Mauretanien, Mongolei, Mosambik, Suriname"},
		     /* 19.14 */	{"Tonga ...", "Tonga, Zentralafrikanische Republik"},
		     /* 19.44 */	{"Togo", "Togo"},
		     /* 20.28 */	{"Myanmar Burma", "Myanmar Burma"},
		     /* 21.36 */	{"Bolivien ...", "Bolivien, Kaimaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Kuba, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay"},
		     /* 21.66 */	{"Mauritius ...", "Mauritius, Nordkorea"},
		     /* 24.06 */	{"Anguilla ...", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso Obervolta, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen (Arab. Republik), Kambodscha, Kamerun, Cape Verde, Kongo, Mali, Montserrat, Pakistan, Salomonen, Senegal, Seyschellen, Somalia, St. Kitts und Nevis, Tschad, Vietnam, Zaire"},
		     /* 29.28 */	{"Wake Inseln", "Wake Inseln"},
		     /* 30.60 */	{"Diego Garcia", "Diego Garcia"},
		     /* 32.16 */	{"Cookinseln ...", "Cookinseln, Madagaskar"},
		     /* 40.14 */	{"Afghanistan ...", "Afghanistan, Guantanamo, Marshallinseln, Mikronesien, Midway-Inseln, Palau /Belau, Amerikanisch-Samoa"},

  };


  double Tarif[] =
  { 0.80, /* Business regional 0.67,1,3.08,3.28+20% */
    1.20,
    3.70,
    3.99, /* One */
    3.24, /* Deutschland ... */
    3.48, /* Liechtenstein ... */
    3.60, /* Italien ... */
    3.96, /* Frankreich ... */
    4.20, /* Spanien ... */
    4.32, /* Belgien ... */
    4.68, /* D�nemark ... */
    4.74, /* Monaco ... */
    4.80, /* Bosnien-Herzegovina ... */
    4.86, /* Polen ... */
    5.04, /* Griechenland ... */
    5.16, /* Andorra ... */
    5.22, /* Jungfern Inseln USA ... */
    5.28, /* Puerto Rico ... */
    5.52, /* Serbien/Montenegro ... */
    5.94, /* Bulgarien ... */
    6.06, /* Mazedonien ... */
    6.18, /* Albanien ... */
    6.24, /* Israel ... */
    6.30, /* Gibraltar ... */
    6.36, /* Australien ... */
    6.38, /* Russland ... */
    6.42, /* Island ... */
    7.56, /* Hongkong ... */
    7.80, /* Algerien ... */
    7.98, /* Madeira ... */
    8.04, /* Christmas Insel ... */
    8.16, /* Singapur ... */
    8.52, /* Georgien ... */
    9.48, /* S�dafrika ... */
    9.54, /* Libyen ... */
    9.66, /* Chile ... */
    9.72, /* Franz. Guyana ... */
    9.96, /* Armenien ... */
    10.4, /* Aserbaidschan ... */
    11.16, /* Malaysia ... */
    11.28, /* Saipan ... */
    11.76, /* Brasilien ... */
    12.78, /* Bermudas ... */
    12.84, /* China ... */
    12.96, /* Angola ... */
    13.32, /* �gypten ... */
    14.40, /* Aruba ... */
    14.52, /* Panama ... */
    14.58, /* Barbados ... */
    14.64, /* Belize ... */
    15.84, /* Libanon ... */
    15.96, /* Gabun ... */
    16.56, /* Bahrain ... */
    16.68, /* Nigeria ... */
    16.74, /* Burundi ... */
    16.92, /* Nepal ... */
    17.28, /* Guyana ... */
    18.12, /* Guinea-Bissau ... */
    18.30, /* Nicaragua ... */
    18.56, /* Malawi ... */
    18.66, /* Argentinien ... */
    18.74, /* Norfolk Inseln ... */
    18.78, /* Wallis/Futuna ... */
    18.90, /* Fidschi ... */
    18.96, /* �thiopien ... */
    19.14, /* Tonga ... */
    19.44, /* Togo ... */
    20.28, /* Burma/Myanmar ... */
    21.36, /* Bolivien ... */
    21.66, /* Mauritius ... */
    24.06, /* Anguilla ... */
    29.28, /* Wake ... */
    30.60, /* Diego Garcia ... */
    32.16, /* Cook Inseln ... */
    40.14 /* Afghanistan ... */
  };
  int  z;

  rprintf ("P:29,2", "CyberTron easy.Talk business" );
  rprintf ("C:TarifChanged:", "10.09.1999" );

  rprintf ("C:Name:", "Cybertron - austrian digital telecom" );
  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.cybertron.at" );
  rprintf ("C:TarifURL:", "http://www.cybertron.at/html/easy.prod_0.html" );
  rprintf ("C:Telefon:", "0800 400 400, 1029 1029" );
  rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 100 Km, die Fernzone " );
  rprintf ("C:Zone:", "dar�ber - Die Regionalzone ist nicht implementiert" );

  rprintf ("#Fixme: +-100Km nicht implemtiert", "# Verzonung" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}

void rate_1044_old(void) {

  char *Zone[][2] = {{ "Regional", "" },
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland", "+" }};

  double Tarif[][2] = {{ 0.864, 0.864 },
		       { 1.728, 0.864 },
		       { 5.04, 5.04 },
		       { 5999.40, 5999.40 }};

  int z;

  rprintf ("P:44", "Citykom" );

  rprintf ("C:TarifChanged:", "01.09.1999" );
  rprintf ("C:Name:", "Citykom Austria Telekommunikations GmbH" );
  rprintf ("C:Address:", "Alserbachstr. 144-16, A-1090 Wien" );
  rprintf ("C:Homepage:", "http://www.citykom.at" );
  rprintf ("C:EMail:", "j.schmolinsk@citykom.at" );
  rprintf ("C:Telefon:", "0800 201 301" );
  rprintf ("C:Zone:", "Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der " );
  rprintf ("C:Zone:", "Regionalzone sonst in der �sterreichzone." );
  /*  rprintf ("","C:Special:"); */

  rprintf ("D:1012", "# Verzonung" );
  rprintf ("# Fixme:", "Ausland nicht implemtiert" );
  rprintf ("C:Special:", "Ausland nicht implemtiert" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif [z][0]!=0.0) { /* Tarif bekannt? */
      if (Tarif[z][0]==Tarif[z][1]) {
	rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z][0]);
      } else {
	rprintf ("T:W/8-18=%.2f(60)/1", "Tag" , Tarif[z][0]);
	rprintf ("T:W/18-8=%.2f(60)/1", "Nacht" , Tarif[z][1]);
	rprintf ("T:E,H/*=%.2f(60)/1", "Weekend" , Tarif[z][1]);
      }
    }
    print_area(Zone[z][1]);
  }
}

void rate_1049(void) {

  char *Zone[][2] = {{ "Regionalzone", ""},
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     /* 3.00 */	{"Deutschland", "Deutschland"},
		     /* 3.60 */	{"Italien ...", "Italien, Liechtenstein, Schweiz"},
		     /* 3.96 */	{"D�nemark ...", "D�nemark, Frankreich, Finnland, Gro�britannien, Niederlande, Schweden, Ungarn"},
		     /* 4.08 */	{"Belgien", "Belgien"},
		     /* 4.20 */	{"Kanada ...", "Kanada, Norwegen, Slowenien, USA"},
		     /* 4.56 */	{"Spanien", "Spanien"},
		     /* 4.68 */	{"Irland ...", "Irland, Luxemburg, Slowakische Republik, Tschechische Republik"},
		     /* 4.80 */	{"Polen", "Polen"},
		     /* 5.28 */	{"Amerikanische Jungferninseln ...", "Amerikanische Jungferninseln, Andorra, Azoren, Griechenland, Monaco, Malta, Jugoslawien, Ukraine"},
		     /* 5.52 */	{"Kroatien", "Kroatien"},
		     /* 5.64 */	{"Portugal", "Portugal"},
		     /* 5.88 */	{"Albanien ...", "Albanien, Bosnien-Herzegowina, Bulgarien, Rum�nien, Zypern (Griechisch)"},
		     /* 6.00 */	{"Puerto Rico ...", "Puerto Rico, San Marino"},
		     /* 6.24 */	{"Australien ...", "Australien, T�rkei"},
		     /* 6.60 */	{"Algerien ...", "Algerien, Estland, Mazedonien"},
		     /* 7.08 */	{"Island ...", "Island, Israel, Palestina"},
		     /* 7.20 */	{"Gibraltar ...", "Gibraltar, Lettland, Litauen, Russische F�rderation (�stl.), Tadschikistan, Weissrussland"},
		     /* 7.44 */	{"Weihnachtsinseln ...", "Weihnachtsinseln, Tunesien"},
		     /* 7.92 */	{"F�r�er-Inseln ...", "F�r�er-Inseln, Kanarische Inseln, Madeira, Marokko"},
		     /* 8.28 */	{"Georgien ...", "Georgien, Japan, Singapur"},
		     /* 9.00 */	{"Hongkong ...", "Hongkong, Usbekistan"},
		     /* 9.24 */	{"Armenien ...", "Armenien, Aserbaidschan, Antarktis"},
		     /* 9.48 */	{"Libyen ...", "Libyen, Neuseeland"},
		     /* 9.60 */	{"Chile ...", "Chile, Martinique / Franz. Antillen"},
		     /* 10.20 */	{"Moldavien Moldau (Republik) ...", "Moldavien Moldau (Republik), S�dafrika"},
		     /* 10.68 */	{"Guadeloupe ...", "Guadeloupe, Niederl. Antillen, S�dkorea, Reunion, Zypern (T�rkisch)"},
		     /* 10.92 */	{"Franz�sisch-Guayana", "Franz�sisch-Guayana"},
		     /* 11.40 */	{"Turkmenistan", "Turkmenistan"},
		     /* 11.88 */	{"Malaysia", "Malaysia"},
		     /* 12.60 */	{"�gypten ...", "�gypten, Angola, Brasilien, Bermuda, Bhutan"},
		     /* 13.08 */	{"China ...", "China, Venezuela"},
		     /* 13.80 */	{"Dominikanische Republik ...", "Dominikanische Republik, Ecuador"},
		     /* 14.28 */	{"Macao ...", "Macao, Philippinen, Taiwan"},
		     /* 14.40 */	{"Aruba ...", "Aruba, Bahamas, Bolivien, Ver. Arabische Emirate"},
		     /* 14.64 */	{"Jordanien", "Jordanien"},
		     /* 15.00 */	{"Barbados ...", "Barbados, Belize, El Salvador, Ghana, Gr�nland, Mayotte"},
		     /* 15.24 */	{"Saudi Arabien", "Saudi Arabien"},
		     /* 15.48 */	{"Iran ...", "Iran, Kuwait, Kirgistan, St. Vincent und Grenadinen"},
		     /* 15.60 */	{"Argentinien", "Argentinien"},
		     /* 15.84 */	{"Liberia", "Liberia"},
		     /* 16.20 */	{"Burkina Faso Obervolta ...", "Burkina Faso Obervolta, Burundi, Trinidad und Tobago"},
		     /* 16.44 */	{"Libanon", "Libanon"},
		     /* 16.68 */	{"Diego Garcia ...", "Diego Garcia, Gabun, Guinea, Peru, Panama, Swasiland, Syrien, Tansania"},
		     /* 17.28 */	{"Kasachstan ...", "Kasachstan"},
		     /* 17.40 */	{"Bahrain ...", "Bahrain, Benin, Kaimaninseln"},
		     /* 17.88 */	{"Dominica ...", "Dominica, Kolumbien, Malawi, St. Pierre und Miquelon, Sudan"},
		     /* 18.00 */	{"�quatorial-Guinea ...", "�quatorial-Guinea, Elfenbeink�ste Cote de Ivoire, Franz�sisch-Polynesien"},
		     /* 19.08 */	{"Fidschi ...", "Fidschi, Guyana, Komoren, Kenia, Namibia, Nauru, Nepal, Nigeria, Neukaledonien, Sao Tome und Principe, St. Helena, St. Kitts und Nevis, St. Lucia, Simbabwe, West-Samoa"},
		     /* 19.20 */	{"Ascension ...", "Ascension, Britische Jungferninseln, Brunei"},
		     /* 19.56 */	{"Botsuana Botswana", "Botsuana Botswana"},
		     /* 19.80 */	{"�thiopien ...", "�thiopien, Anguilla, Gambia, Grenada, Norfolkinseln, Sambia"},
		     /* 20.28 */	{"Cookinseln ...", "Cookinseln, Costa Rica, Dschibuti, Guam, Guatemala, Guinea-Bissau, Kiribati, Cape Verde, Laos, Lesotho, Mikronesien, Mongolei, Montserrat, Mosambik, Nicaragua, Oman, Papua-Neuguinea, Paraguay, Sri Lanka, Thailand, Tonga, Uganda, Uruguay"},
		     /* 21.48 */	{"Honduras ...", "Honduras, Indien, Indonesien, Irak, Jamaika, Mauritius, Malediven, Mauretanien, Niger, Pakistan, Palau /Belau, Ruanda, Somalia, Amerikanisch-Samoa, Sierra Leone, Togo, Tuvalu, Vanuatu"},
		     /* 21.60 */	{"Antigua und Barbuda", "Antigua und Barbuda"},
		     /* 22.20 */	{"Eritrea", "Eritrea"},
		     /* 22.68 */	{"Niue-Inseln ...", "Niue-Inseln, Jemen (Arab. Republik), Senegal, Seyschellen, Wallis und Futuna, Zentralafrikanische Republik"},
		     /* 23.40 */	{"Afghanistan ...", "Afghanistan, Falklandinseln, Kuba, Madagaskar"},
		     /* 23.88 */	{"Haiti ...", "Haiti, Kambodscha, Kamerun, Kongo, Nordkorea, Mali, Marshallinseln, Myanmar Burma, Pitcairn Inseln, Salomonen, Suriname, Tschad, Vietnam"},
		     /* 35.88 */	{"Wake Inseln", "Wake Inseln"},
  };

  double Tarif[] = { 0.96, /* W,N,B 50 km */
		     1.74,
		     3.60,
		     3.00,  /* Deutschland ... */
		     3.60,  /* Italien ... */
		     3.96,  /* D�nemark ... */
		     4.08,  /* Belgien ... */
		     4.20,  /* Kanada ... */
		     4.56,  /* Spanien ... */
		     4.68,  /* Irland ... */
		     4.80,  /* Polen ... */
		     5.28,  /* Amerik. Jungferninseln ... */
		     5.52,  /* Kroatien ... */
		     5.64,  /* Portugal ... */
		     5.88,  /* Albanien ... */
		     6.00,  /* Puerto Rico  ... */
		     6.24,  /* Australien ... */
		     6.60,  /* Algerien ... */
		     7.08,  /* Island ... */
		     7.20,  /* Gibraltar ... */
		     7.44,  /* Austral. Weihnachtsinseln ... */
		     7.92,  /* F�r�er - Inseln ... */
		     8.28,  /* Georgien ... */
		     9.00,  /* Hongkong ... */
		     9.24,  /* Armenien ... */
		     9.48,  /* Libyen ... */
		     9.60,  /* Chile ... */
		     10.20,  /* Moldawien ... */
		     10.68,  /* Guadeloupe ... */
		     10.92,  /* Franz. Guyana ... */
		     11.40,  /* Turkmenistan ... */
		     11.88,  /* Malaysia ... */
		     12.60,  /* �gypten ... */
		     13.08,  /* China ... */
		     13.80,  /* Dominik. Republik  ... */
		     14.28,  /* Macau ... */
		     14.40,  /* Aruba ... */
		     14.64,  /* Jordanien ... */
		     15.00,  /* Barbados ... */
		     15.24,  /* Saudi Arabien  ... */
		     15.48,  /* Iran ... */
		     15.60,  /* Argentinien ... */
		     15.84,  /* Liberia ... */
		     16.20,  /* Burkina Faso ... */
		     16.44,  /* Libanon ... */
		     16.68,  /* Diego Garcia  ... */
		     17.28,  /* Kasachstan ... */
		     17.40,  /* Bahrain ... */
		     17.88,  /* Dominica ... */
		     18.00,  /* �quat. - Guinea ... */
		     19.08,  /* Fidschi ... */
		     19.20,  /* Ascension ... */
		     19.56,  /* Botswana ... */
		     19.80,  /* �thiopien ... */
		     20.28,  /* Cook - Inseln ... */
		     21.48,  /* Honduras ... */
		     21.60,  /* Antigua ... */
		     22.20,  /* Eritrea ... */
		     22.68,  /* Niue (Savage - Inseln) ... */
		     23.40,  /* Afghanistan ... */
		     23.88,  /* Haiti ... */
		     35.88,  /* Wake - Inseln ... */
  };

  int z;

  rprintf ("P:49", "Pegasus Telekom" );
  rprintf ("C:TarifChanged:", "01.09.1999" );
  rprintf ("D:1001", "# Verzonung" );
  rprintf ("C:Name:", "Pegasus Telekom Netzwerkdienste AG" );
  rprintf ("C:Address:", "Serrravag. 10-12, A-1140 Wien" );
  rprintf ("C:Homepage:", "http://www.ptag.net" );
  rprintf ("C:TarifURL:", "http://www.ptag.net/F_Tarife.htm" );
  rprintf ("C:Telefon:", "01 9150 0" );
  rprintf ("#", "Fixme: Verzonung nicht verifiziert." );
  //  rprintf ("Bei einem Rechnungswert unter ATS 500 netto, werden ATS 50 zus�tzlich verrechnet.","C:GT:");
  //  rprintf ("Cost = Ch < 500 ? 50 : 0","C:GF:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/1", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1066(void) {

  char *Zone[][2] = {{ "Regionalzone", "" },
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Euro 1", "Deutschland, Frankreich, Gro�britannien, Nordirland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Euro 2", "Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, D�nemark, Estland, Finnland, Griechenland, Irland, Jugoslawien, Kroatien, Luxemburg, Monaco, Niederlande, Norwegen, Polen, Portugal, Rum�nien, San Marino, Schweden, Spanien, Tunesien, T�rkei, Ukraine, Vatikanstadt, Zypern" },
		     { "Euro City", "Amsterdam, Br�ssel, Frankfurt, London, Luxemburg City, Mailand, Paris, Z�rich" },
		     { "Welt 1", "Hawaii, Amerikanische Jungferninseln, Puerto Rico, Vereinigte Staaten (USA), Kanada" },
		     { "Welt 2", "Albanien, Antarktis, Armenien, Aserbaidschan, Australien, Bahamas, Chile, Weihnachtsinseln, Kokosinseln, Dominikanische Republik, Georgien, Gibraltar, Guadeloupe, Franz�sisch-Guayana, Hongkong, Island, Israel, Japan, Lettland, Libyen, Litauen, Mazedonien, Malaysia, Malta, Marokko, Martinique, Mayotte, Moldau, Neuseeland, Niederl�ndische Antillen, Philippinen, Russische F�deration, Reunion, St. Pierre und Miquelon, Singapur, S�dafrika, S�dkorea, Saipan" },
		     { "Welt 3", "�gypten, Angola, Argentinien, Aruba, Ascension, Bahrain, Barbados, Belize, Benin, Bermuda, Bhutan, Bolivien, Botsuana, Brasilien, Brunei, Burkina Faso, Burundi, Caymaninseln, China, Costa Rica, Dschibuti, Dominica, Ecuador, El Salvador, F�r�er-Inseln, Falklandinseln, Fidschi, Franz�sisch-Polynesien, Gabun, Gambia, Ghana, Grenada, Gr�nland, Guatemala, Haiti, Honduras, Indien, Indonesien, Iran, Jamaika, Jordanien, Katar, Kenia, Kirgisistan, Kolumbien, Nordkorea, Kongo, Kuba, Kuwait, Laos, Lesotho, Libanon, Liberia, Macau, Malawi, Malediven, Marshallinseln, Mauretanien, Mauritius, Mexiko, Montserrat, Nepal, Neukaledonien, Nicaragua, Nigeria, Norfolk-Inseln, Panama, Paraguay, Peru, Ruanda, Sambia, Saudi-Arabien, Salomonen, Simbabwe, Somalia, St. Helena, Saint Kitts und Nevis, St. Lucia, Saint Vincent und die Grenadinen, Sudan, Syrien, Tadschikistan, Tansania, Taiwan, Thailand, Trinidad und Tobago, Turkmenistan, Uruguay, Usbekistan, Vanuatu, Venezuela, Vereinigte Arabische Emirate" },
		     { "Welt 4", "Afghanistan, Algerien, Anguilla, Antigua und Barbuda, �quatorial-Guinea, �thiopien, Bangladesch, Cook-Inseln, Cote d'Ivoire, Diego Garcia, Eritrea, Guam, Guantanamo, Guinea, Guinea-Bissau, Guyana, Irak, Jemen, Kambodscha, Kamerun, Kap Verde, Kasachstan, Kiribati, Komoren, Madagaskar, Mali, Midway-Inseln, Mikronesien, Mongolei, Mosambik, Myanmar, Namibia, Nauru, Niger, Niue, Oman, Pakistan, Palau, Papua-Neuguinea, Pitcairn-Inseln, Sao Tome und Principe, Amerikanisch-Samoa, Samoa (West), Senegal, Seychellen, Sierra Leone, Sri Lanka, Suriname, Swasiland, Togo, Tonga, Tschad, Turks- und Caicos-Inseln, Tuvalu, Uganda, Vietnam, Wake-Inseln, Zaire, Zentralafrikanische Republik" }};

  double Tarif[][2] = {{ 0.90, 0.80},
		       { 0.90, 0.80 },
		       { 4.00, 3.60 },
		       { 4.00, 3.66 },
		       { 5.56, 5.56 },
		       { 3.76, 3.46 },
		       { 6.66, 6.66 },
		       {11.66,11.66 },
		       {19.66,19.66 },
		       {26.66,26.66 }};

  int z;

  rprintf ("P:66", "MIT 1066" );

  rprintf ("C:Name:", "M.I.T Multi Media Informations Technologies GesmbH" );
  rprintf ("C:Address:", "Modecenterstrr. 14/5, A-1030 Wien" );
  rprintf ("C:Homepage:", "http://www.mit.co.at" );
  rprintf ("C:Telefon:", "01 79040 1500" );
  rprintf ("C:Zone:", "Die Regionalzone geht bis zu einer Entfernung von 50 Km, alles andere ist " );
  rprintf ("C:Zone:", "�sterreichzone." );
  rprintf ("C:Special:", "0.30 ATS Mindestentgelt pro Verbindung" );
  rprintf ("D:1066", "# Verzonung" );
  rprintf ("# Fixme", "Verzonung -50/+50 nicht verifiziert" );
  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=0.30|%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=0.30|%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=0.30|%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=0.30|%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1067(void) {

  char *Zone[][2] = {{ "Festnetz", "�sterreich" },
		     { "max.box", "+4367622" },
		     { "max.online", "+436762323" },
		     { "max.mobil", "+43676" },
		     { "andere Mobilfunknetze", "+43663,+43664,+43699" },
		     { "Deutschland", "Deutschland" },
		     { "Nachbarl�nder", "Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "EU", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Griechenland, Irland, Luxemburg, Niederlande, Portugal, Spanien, Schweden, Nordirland" },
		     { "Weltzone 1", "Andorra, Australien, Bahamas, Bosnien-Herzegowina, Kanada, Kroatien, F�r�er-Inseln, Gibraltar, Island, Japan, Lettland, Malta, Mazedonien, Monaco, Neuseeland, Norwegen, Polen, Puerto Rico, San Marino, Singapur, USA, Vatikan, Jugoslawien" },
		     { "Weltzone 2", "Albanien, Algerien, Belarus, Bulgarien, Zypern, Estland, Hong Kong, Litauen, Marokko, Oman, Rum�nien, Ru�land, Tunesien, T�rkei, Ukraine, American Virgin Isles" },
		     { "Weltzone 3", "Bermuda, Brasilien, Brunei, Chile, Christmas Island, Cocos Island, Dominikanische Republik, Georgien, Ghana, Gr�nland, Israel, Jamaica, Kasachstan, S�dkorea, Kirgisistan, Libyen, Macau, Malaysia, Mexiko, S�dafrika, Taiwan, Tadschikistan" },
		     { "Weltzone 4", "Antigua und Barbuda,St. Lucia,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,Anguilla,St. Vincent und Grenadinen,Montserrat,St. Kitts und Nevis,Grenada,Trinidad und Tobago,Hawaii,Marianen (SaipanNord-),Kaimaninseln,Barbados,+" },
		     { "Iridium 8816", "Iridium 008816"},
		     { "Iridium 8817", "Iridium 008817"}};

  double Tarif[] = { 00.88,
		     01.00,
		     01.00,
		     02.70,
		     03.90,
		     02.50,
		     03.30,
		     03.50,
		     05.50,
		     09.70,
		     15.00,
		     22.00,
		     122.00,
		     62.00 };

  int z;

  rprintf ("P:67", "max.plus" );
  rprintf ("C:Maintainer:", "Michael Reinelt <reinelt@eunet.at>" );
  rprintf ("C:Name:", "max.mobil Telekommunikation Service GmbH" );
  rprintf ("C:Address:", "Postfach 333, A-1031 Wien");
  rprintf ("C:Homepage:", "http://www.maxmobil.at" );
  rprintf ("C:Telefon:", "0676 2010" );
  rprintf ("C:Special:", "Nur f�r max - Vertragskunden." );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/30", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1069(void) {

  char *Zone[][2] = {{ "Festnetz", "�sterreich" },
		     { "One Mobilfunknetz", "+43699" },
		     { "andere Mobilfunknetze", "+43663,+43664,+43676" },
		     { "EU und Nachbarl�nder", "Belgien, D�nemark, Deutschland, Finnland, Frankreich, Gro�britannien, Irland, Italien, Liechtenstein, Luxemburg, Niederlande, Norwegen, Schweden, Schweiz, Slowakei, Slowenien, Spanien, Tschechien, Ungarn" },
		     { "Internationale Zone 2", "Andorra, Australien, Bosnien-Herzegowina, Griechenland, Island, Japan, Jugoslawien, Kanada, Kroatien, Malta, Monaco, Neuseeland, Polen, Portugal, Puerto Rico, Mazedonien, San Marino, Vereinigte Staaten (USA), Vatikanstadt" },
		     { "Internationale Zone 3", "Albanien, Algerien, Bulgarien, Estland, Gibraltar, Hongkong, Israel, Lettland,Litauen, Marokko, Rum�nien, Ru�land, Singapur, Tunesien, T�rkei, Ukraine, Wei�ru�land, Zypern" },
		     { "Internationale Zone 4", "Aserbaidschan, Amerikanisch-Samoa, Armenien, Bahamas, Bermuda, Brasilien, Brunei, Chile, Dominikanische Republik, Falklandinseln, Georgien, Ghana, Gr�nland, Jamaika, Kasachstan, Kokosinseln, Libyen, Macao, Mexico, Malaysia, Moldau, Oman, S�dkorea, S�dafrika, Tadschikistan,Taiwan, Usbekistan, Venezuela, Vereinigte Arabische Emirate, Weihnachtsinseln" },
		     { "Internationale Zone 5", "Antigua und Barbuda,St. Lucia,Dominica,Britische Jungferninseln,Turks- und Caicosinseln,Anguilla,St. Vincent und Grenadinen,Montserrat,St. Kitts und Nevis,Grenada,Trinidad und Tobago,Hawaii,Marianen (SaipanNord-),Kaimaninseln,Amerikanische Jungferninseln,Barbados,+" }};

  double Tarif[] = { 01.00,
		     02.00,
		     04.00,
		     03.50,
		     05.50,
		     08.00,
		     15.00,
		     22.00 };

  int z;

  rprintf ("P:69", "ONE" );

  rprintf ("C:Name:", "Connect Austria Gesellschaft f�r Telekommunikation GmbH" );
  rprintf ("C:Address:", "Br�nner Stra�e 52, A-1210 Wien " );
  rprintf ("C:Homepage:", "http://www.one.at" );
  rprintf ("C:Telefon:", "0800 1069 3000" );
  /*  rprintf ("","C:Zone:");
      rprintf ("C:Special:", "" ); */

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/30", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_librotel(void) {
  char *Zone[][2] = {
    { "�sterreich", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    { "Internet", "0800400600"},
    /* 2.50 */	{"Deutschland", "Deutschland"},
    /* 3.50 */	{"Italien ...", "Italien, Amerikanische Jungferninseln, Liechtenstein, Schweiz, Slowakische Republik, Slowenien, Tschechische Republik, Ungarn, USA"},
    /* 4.00 */	{"Andorra ...", "Andorra, Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Luxemburg, Monaco, Niederlande, Norwegen, Schweden, Vatikan"},
    /* 5.25 */	{"Australien ...", "Australien, Bosnien-Herzegowina, Kanada, Kroatien, Malta, Polen, Spanien, Zypern"},
    /* 7.00 */	{"Albanien ...", "Albanien, Bulgarien, Estland, F�r�er-Inseln, Gibraltar, Griechenland, Island, Israel, Mazedonien, Portugal, Puerto Rico, Rum�nien, Russische F�rderation (�stl.), San Marino, Jugoslawien, Tunesien, T�rkei, Ukraine, Weissrussland"},
    /* 8.90 */	{"Algerien ...", "Algerien, Georgien, Lettland, Litauen, Marokko, Tadschikistan, Turkmenistan, Usbekistan"},
    /* 10.50 */	{"Armenien ...", "Armenien, Aserbaidschan, Weihnachtsinseln, Kokosinseln"},
    /* 12.50 */	{"Angola ...", "Angola, Bermuda, Brasilien, Chile, Franz�sisch-Guayana, Guadeloupe, Hongkong, Japan, Libyen, Macao, Madeira, Malaysia, Martinique / Franz. Antillen, Mayotte, Mexiko, Moldavien Moldau (Republik), Neuseeland, Niederl. Antillen, Philippinen, Reunion, Marianen (SaipanNord-), Singapur, St. Pierre und Miquelon, S�dafrika, S�dkorea, Venezuela"},
    /* 17.50 */	{"�gypten ...", "�gypten, Aruba, Bahamas, Barbados, Belize, Bhutan, Botsuana Botswana, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Gr�nland, Guyana, Iran, Jordanien, Kirgistan, Kolumbien, Kuwait, Libanon, Panama, Saudi Arabien, St. Vincent und Grenadinen, Trinidad und Tobago, Ver. Arabische Emirate"},
    /* 19.50 */	{"Argentinien ...", "Argentinien, �thiopien, Bahrain, Benin, Costa Rica, Elfenbeink�ste Cote de Ivoire, Fidschi, Franz�sisch-Polynesien, Gambia, Guinea, Guinea-Bissau, Jamaika, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mosambik, Namibia, Nauru, Nepal, Neukaledonien, Nicaragua, Nigeria, Niue-Inseln, Norfolkinseln, Oman, Peru, Ruanda, Sambia, West-Samoa, Simbabwe, St. Helena, St. Lucia, Sao Tome und Principe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks- und Caicosinseln, Uganda, Vanuatu, Zentralafrikanische Republik"},
    /* 24.50 */	{"Anguilla ...", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso Obervolta, Myanmar Burma, Kaimaninseln, Dominica, Dschibuti, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Irak, Jemen (Arab. Republik), Britische Jungferninseln, Kambodscha, Kamerun, Cape Verde, Kongo, Kuba, Mali, Mauritius, Mongolei, Montserrat, Niger, Nordkorea, Pakistan, Papua-Neuguinea, Paraguay, Salomonen, Senegal, Seyschellen, Sierra Leone, Somalia, Sri Lanka, St. Kitts und Nevis, Tschad, Uruguay, Vietnam, Zaire"},
    /* 40.00 */	{"Cookinseln ...", "Cookinseln, Diego Garcia, Guantanamo, Madagaskar, Marshallinseln, Mikronesien, Midway-Inseln, Palau /Belau, Amerikanisch-Samoa, Tuvalu, Wake Inseln, Wallis und Futuna"},
  };
  double Tarif[][2] = {
    { 0.85, 0.85 }, // oest
    { 3.99, 3.33 }, // mob
    { 0.49, 0.49 }, // sur
    {2.50,2.50},  /* Deutschland ... */
    {3.50,3.50},  /* Italien ... */
    {4.00,4.00},  /* Andorra ... */
    {5.25,5.25},  /* Australien ... */
    {7.00,7.00},  /* Albanien ... */
    {8.90,8.90},  /* Algerien ... */
    {10.50,10.50},  /* Armenien ... */
    {12.50,12.50},  /* Angola ... */
    {17.50,17.50},  /* �gypten ... */
    {19.50,19.50},  /* Argentinien ... */
    {24.50,24.50},  /* Anguilla ... */
    {40.00,40.00}  /* Cookinseln ... */
  };
  int z;
  rprintf ("P:29,5", "LibroTel" );
  rprintf ("C:TarifChanged:", "10.09.1999" );
  rprintf ("C:Homepage:", "http://www.librotel.at" );
  rprintf ("C:Telefon:", "0800 400 808" );
  rprintf ("C:Special:", "Nur mit Router, ATS 299.-" );
  rprintf ("C:GT:", "Nur mit Router, ATS 299.-" );
  rprintf ("C:GF:", "Cost=299/12" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=%.2f(60)/60/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=%.2f(60)/60/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=%.2f(60)/60/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=%.2f(60)/60/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_tns(void) {
  char *Zone[][2] = {
    /* 2.30 */	{"�sterreich", "�sterreich"},
		/* 3.30 */	{"Mobil", "+43663,+43664,+43676,+43699"},
		/* 2.99 */	{"Deutschland", "Deutschland"},
		/* 3.30 */	{"USA", "USA"},
		/* 3.85 */	{"Gro�britannien ...", "Gro�britannien, Schweiz"},
		/* 4.40 */	{"Italien ...", "Italien, Schweden"},
		/* 4.46 */	{"Ungarn", "Ungarn"},
		/* 4.50 */	{"Alaska ...", "Alaska, Slowakische Republik, Slowenien"},
		/* 4.95 */	{"Belgien ...", "Belgien, D�nemark, Finnland, Hawaii, Kanada, Liechtenstein, Monaco, Niederlande, Norwegen, Tschechische Republik"},
				/* 5.00 */	{"Israel", "Israel"},
		/* 5.50 */	{"Irland ...", "Irland, Luxemburg, Polen"},
		/* 5.83 */	{"Puerto Rico", "Puerto Rico"},
		/* 5.95 */	{"Bulgarien ...", "Bulgarien, Kroatien, Jugoslawien, Zypern"},
		/* 6.05 */	{"Albanien ...", "Albanien, Andorra, Australien, Weissrussland, Bosnien-Herzegowina, Griechenland, Malta, Spanien"},
		/* 6.18 */	{"T�rkei", "T�rkei"},
		/* 6.30 */	{"Ukraine", "Ukraine"},
		/* 6.50 */	{"Estland ...", "Estland, Mazedonien"},
		/* 6.60 */	{"Island", "Island"},
		/* 6.70 */	{"Tunesien", "Tunesien"},
		/* 6.82 */	{"Rum�nien", "Rum�nien"},
		/* 7.15 */	{"Lettland ...", "Lettland, Litauen"},
		/* 7.26 */	{"Portugal", "Portugal"},
		/* 7.68 */	{"Marokko", "Marokko"},
		/* 7.70 */	{"Algerien ...", "Algerien, Bermuda, Gibraltar"},
		/* 7.90 */	{"Russische F�rderation (�stl.)", "Russische F�rderation (�stl.)"},
		/* 7.92 */	{"F�r�er-Inseln ...", "F�r�er-Inseln, Japan"},
		/* 8.14 */	{"Singapur", "Singapur"},
		/* 8.20 */	{"Tadschikistan", "Tadschikistan"},
		/* 8.25 */	{"Hongkong ...", "Hongkong, Niederl. Antillen, Usbekistan"},
		/* 8.70 */	{"Turkmenistan", "Turkmenistan"},
		/* 8.80 */	{"Libyen", "Libyen"},
		/* 9.11 */	{"Aserbaidschan", "Aserbaidschan"},
		/* 9.35 */	{"Georgien ...", "Georgien, Philippinen"},
		/* 9.90 */	{"Bahamas ...", "Bahamas, Chile, China, Guyana, Liberia, Neuseeland"},
		/* 10.12 */	{"S�dkorea", "S�dkorea"},
		/* 10.23 */	{"Marianen (SaipanNord-)", "Marianen (SaipanNord-)"},
		/* 10.45 */	{"Armenien ...", "Armenien, S�dafrika"},
		/* 10.89 */	{"Venezuela", "Venezuela"},
		/* 11.00 */	{"Brasilien ...", "Brasilien, Malaysia, Moldavien Moldau (Republik), Nicaragua"},
		/* 11.33 */	{"Botsuana Botswana ...", "Botsuana Botswana, Kaimaninseln"},
		/* 11.44 */	{"St. Pierre und Miquelon", "St. Pierre und Miquelon"},
		/* 11.50 */	{"Guadeloupe", "Guadeloupe"},
		/* 12.09 */	{"Aruba", "Aruba"},
		/* 12.10 */	{"Brunei ...", "Brunei, Macao, Mayotte"},
		/* 12.21 */	{"Dominica", "Dominica"},
		/* 12.32 */	{"Ecuador ...", "Ecuador, Reunion"},
		/* 12.54 */	{"Mexiko", "Mexiko"},
		/* 12.65 */	{"�gypten", "�gypten"},
		/* 13.20 */	{"Burundi ...", "Burundi, Guatemala, Kolumbien, Marshallinseln"},
		/* 13.42 */	{"Gr�nland ...", "Gr�nland, Honduras, Iran"},
		/* 13.53 */	{"Taiwan", "Taiwan"},
		/* 13.59 */	{"Angola", "Angola"},
		/* 13.64 */	{"Barbados ...", "Barbados, Dominikanische Republik, Ghana, Tansania"},
		/* 13.75 */	{"Argentinien ...", "Argentinien, Bolivien, Guam, Neukaledonien, Uruguay, Ver. Arabische Emirate"},
		/* 14.07 */	{"Anguilla", "Anguilla"},
		/* 14.30 */	{"Gambia", "Gambia"},
		/* 14.41 */	{"Panama", "Panama"},
		/* 14.85 */	{"Belize ...", "Belize, Bhutan, Peru, Saudi Arabien, Trinidad und Tobago"},
		/* 15.40 */	{"Benin ...", "Benin, Costa Rica, Jamaika, Jemen (Arab. Republik), Jordanien, Kamerun, Kuba, Kuwait, Laos"},
		/* 15.62 */	{"Haiti ...", "Haiti, Kasachstan, Katar"},
		/* 15.73 */	{"Bahrain ...", "Bahrain, Indonesien"},
		/* 15.95 */	{"Burkina Faso Obervolta ...", "Burkina Faso Obervolta, Gabun, Libanon, Niger"},
		/* 16.50 */	{"Indien ...", "Indien, Kirgistan, Madagaskar, Namibia, Nauru, Nepal"},
		/* 16.61 */	{"Sudan ...", "Sudan, Swasiland"},
		/* 16.72 */	{"El Salvador ...", "El Salvador, Sierra Leone, Vanuatu"},
		/* 16.83 */	{"Thailand ...", "Thailand, Zaire"},
		/* 16.94 */	{"Sambia", "Sambia"},
		/* 17.05 */	{"Paraguay", "Paraguay"},
		/* 17.16 */	{"Syrien", "Syrien"},
		/* 17.27 */	{"Sri Lanka", "Sri Lanka"},
		/* 17.38 */	{"St. Helena ...", "St. Helena, St. Vincent und Grenadinen, St. Lucia"},
		/* 17.50 */	{"Nigeria", "Nigeria"},
		/* 17.60 */	{"Kongo ...", "Kongo, Mikronesien, Montserrat"},
		/* 17.71 */	{"Diego Garcia", "Diego Garcia"},
		/* 17.82 */	{"Somalia", "Somalia"},
		/* 17.93 */	{"Kenia", "Kenia"},
		/* 18.04 */	{"Simbabwe ...", "Simbabwe, Suriname"},
		/* 18.15 */	{"Palau /Belau", "Palau /Belau"},
		/* 18.70 */	{"Franz�sisch-Polynesien ...", "Franz�sisch-Polynesien, Grenada, Malediven, Mauretanien, Mauritius, Mosambik"},
		/* 18.92 */	{"Irak", "Irak"},
		/* 19.31 */	{"Bangladesch", "Bangladesch"},
		/* 19.36 */	{"Sao Tome und Principe", "Sao Tome und Principe"},
		/* 19.80 */	{"Guinea-Bissau ...", "Guinea-Bissau, Kiribati, Mali, Uganda"},
		/* 19.91 */	{"Niue-Inseln ...", "Niue-Inseln, Oman"},
		/* 20.13 */	{"�thiopien", "�thiopien"},
		/* 20.24 */	{"Ruanda", "Ruanda"},
		/* 20.35 */	{"Senegal ...", "Senegal, Togo, Vietnam"},
		/* 20.90 */	{"Nordkorea", "Nordkorea"},
		/* 20.93 */	{"Amerikanisch-Samoa", "Amerikanisch-Samoa"},
		/* 21.01 */	{"Fidschi", "Fidschi"},
		/* 21.12 */	{"Falklandinseln", "Falklandinseln"},
		/* 21.45 */	{"Eritrea", "Eritrea"},
		/* 21.67 */	{"Papua-Neuguinea ...", "Papua-Neuguinea, Tonga"},
		/* 22.00 */	{"Antigua und Barbuda ...", "Antigua und Barbuda, Kambodscha, Mongolei"},
		/* 22.11 */	{"Zentralafrikanische Republik", "Zentralafrikanische Republik"},
		/* 22.44 */	{"Ascension", "Ascension"},
		/* 22.55 */	{"Tuvalu", "Tuvalu"},
		/* 23.10 */	{"Seyschellen", "Seyschellen"},
		/* 24.20 */	{"Dschibuti", "Dschibuti"},
		/* 24.31 */	{"Cookinseln ...", "Cookinseln, Pakistan"},
		/* 26.40 */	{"Afghanistan", "Afghanistan"}
  };
  double Tarif[] = {
    2.30,  /* Regional  ... */
    3.30,  /* Mobil */
    2.99,  /* Deutschland ... */
    3.30,  /* USA */
    3.85,  /* Gro�britannien ... */
    4.40,  /* Italien ... */
    4.46,  /* Ungarn ... */
    4.50,  /* Alaska ... */
    4.95,  /* Belgien ... */
    5.00,  /* Israel ... */
    5.50,  /* Irland ... */
    5.83,  /* Puerto Rico ... */
    5.95,  /* Bulgarien ... */
    6.05,  /* Albanien ... */
    6.18,  /* T�rkei ... */
    6.30,  /* Ukraine ... */
    6.50,  /* Estland ... */
    6.60,  /* Island ... */
    6.70,  /* Tunesien ... */
    6.82,  /* Rum�nien ... */
    7.15,  /* Lettland ... */
    7.26,  /* Portugal ... */
    7.68,  /* Marokko ... */
    7.70,  /* Algerien ... */
    7.90,  /* Russische F�rderation (�stl.) ... */
    7.92,  /* F�r�er-Inseln ... */
    8.14,  /* Singapur ... */
    8.20,  /* Tadschikistan ... */
    8.25,  /* Hongkong ... */
    8.70,  /* Turkmenistan ... */
    8.80,  /* Libyen ... */
    9.11,  /* Aserbaidschan ... */
    9.35,  /* Georgien ... */
    9.90,  /* Bahamas ... */
    10.12,  /* S�dkorea ... */
    10.23,  /* Marianen (SaipanNord-) ... */
    10.45,  /* Armenien ... */
    10.89,  /* Venezuela ... */
    11.00,  /* Brasilien ... */
    11.33,  /* Botsuana Botswana ... */
    11.44,  /* St. Pierre und Miquelon ... */
    11.50,  /* Guadeloupe ... */
    12.09,  /* Aruba ... */
    12.10,  /* Brunei ... */
    12.21,  /* Dominica ... */
    12.32,  /* Ecuador ... */
    12.54,  /* Mexiko ... */
    12.65,  /* �gypten ... */
    13.20,  /* Burundi ... */
    13.42,  /* Gr�nland ... */
    13.53,  /* Taiwan ... */
    13.59,  /* Angola ... */
    13.64,  /* Barbados ... */
    13.75,  /* Argentinien ... */
    14.07,  /* Anguilla ... */
    14.30,  /* Gambia ... */
    14.41,  /* Panama ... */
    14.85,  /* Belize ... */
    15.40,  /* Benin ... */
    15.62,  /* Haiti ... */
    15.73,  /* Bahrain ... */
    15.95,  /* Burkina Faso Obervolta ... */
    16.50,  /* Indien ... */
    16.61,  /* Sudan ... */
    16.72,  /* El Salvador ... */
    16.83,  /* Thailand ... */
    16.94,  /* Sambia ... */
    17.05,  /* Paraguay ... */
    17.16,  /* Syrien ... */
    17.27,  /* Sri Lanka ... */
    17.38,  /* St. Helena ... */
    17.50,  /* Nigeria ... */
    17.60,  /* Kongo ... */
    17.71,  /* Diego Garcia ... */
    17.82,  /* Somalia ... */
    17.93,  /* Kenia ... */
    18.04,  /* Simbabwe ... */
    18.15,  /* Palau /Belau ... */
    18.70,  /* Franz�sisch-Polynesien ... */
    18.92,  /* Irak ... */
    19.31,  /* Bangladesch ... */
    19.36,  /* Sao Tome und Principe ... */
    19.80,  /* Guinea-Bissau ... */
    19.91,  /* Niue-Inseln ... */
    20.13,  /* �thiopien ... */
    20.24,  /* Ruanda ... */
    20.35,  /* Senegal ... */
    20.90,  /* Nordkorea ... */
    20.93,  /* Amerikanisch-Samoa ... */
    21.01,  /* Fidschi ... */
    21.12,  /* Falklandinseln ... */
    21.45,  /* Eritrea ... */
    21.67,  /* Papua-Neuguinea ... */
    22.00,  /* Antigua und Barbuda ... */
    22.11,  /* Zentralafrikanische Republik ... */
    22.44,  /* Ascension ... */
    22.55,  /* Tuvalu ... */
    23.10,  /* Seyschellen ... */
    24.20,  /* Dschibuti ... */
    24.31,  /* Cookinseln ... */
    26.40  /* Afghanistan ... */
  };
  int z;
  rprintf ("P:29,6", "TNS" );
  rprintf ("C:TarifChanged:", "10.09.1999" );
  rprintf ("C:Name:", "Telephone Network Service Ges.m.b.H Telekommunikationsdienstleistungen" );
  rprintf ("C:Address:", "1100 Wien Davidgasse 79" );
  rprintf ("C:Homepage:", "http://www.tns.at" );
  rprintf ("C:Telefon:", "01 641 72 72-0" );
  rprintf ("C:Telefax:", "01 641 72 72-20" );
  rprintf ("C:Special:", "Zugangsnummer 1029 ??" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    rprintf ("T:*/*=%.2f(60)/30/6", "0-24h" , Tarif[z]);
    print_area(Zone[z][1]);
  }
}
void rate_priority_class(void) {
  double Tarif[][2] = {
    {0.84,0.42}, // Regional
    {2.46,0.84}, //Inland
    {4.50,3.40},  // Mobil
    {0.54,0.36}, //   Priority
    {3.50,3.30},  /* Ausland 1 ... */
    {3.60,3.50},  /* Ausland 2 ... */
    {5.00,4.20},  /* Ausland 3 ... */
    {6.00,5.00},  /* Ausland 4 ... */
    {8.50,8.00},  /* Ausland 5 ... */
    {11.00,11.00},  /* Ausland 6 ... */
    {15.00,15.00},  /* Ausland 7 ... */
    {22.00,22.00},  /* Ausland 8 ... */
  };
  char *Zone[][2] =  {
    { "Regional","" },
    { "�sterreich", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    { "Priority","" },
    /* 1 */	{"Ausland 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakische Republik, Slowenien, Tschechische Republik, Ungarn"},
    /* 2 */	{"Ausland 2", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Amerikanische Jungferninseln, Kanada, Kanarische Inseln, Luxemburg, Monaco, Niederlande, Norwegen, Puerto Rico, Schweden, Spanien, USA"},
    /* 3 */	{"Ausland 3", "Albanien, Andorra, Weissrussland, Bosnien-Herzegowina, Bulgarien, Estland, Gibraltar, Griechenland, Jugoslawien, Kroatien, Lettland, Malta, Mazedonien, Polen, Portugal, Rum�nien, San Marino, Tunesien, T�rkei, Ukraine, Vatikan, Zypern"},
    /* 4 */	{"Ausland 4", "Algerien, F�r�er-Inseln, Georgien, Island, Israel, Litauen, Marokko, Russische F�rderation (�stl.), Tadschikistan, Turkmenistan, Usbekistan"},
    /* 5 */	{"Ausland 5", "Antarktis, Niederl. Antillen, Armenien, Aserbaidschan, Australien, Chile, Weihnachtsinseln, Kokosinseln, Franz. Guyana, Guadeloupe, Hongkong, Japan, S�dkorea, Libyen, Malaysia, Martinique / Franz. Antillen, Mayotte, Moldavien Moldau (Republik), Neuseeland, Philippinen, Reunion, St. Pierre und Miquelon, Marianen (SaipanNord-), S�dafrika"},
    /* 6 */	{"Ausland 6", "Angola, Aruba, Bahamas, Barbados, Belize, Bermuda, Bhutan, Botsuana Botswana, Brasilien, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Iran, Jordanien, Kirgistan, Kolumbien, Kuwait, Libanon, Macao, Mexiko, Panama, St. Vincent und Grenadinen, Sao Tome und Principe, Saudi Arabien, Singapur, Trinidad und Tobago, Venezuela, Ver. Arabische Emirate, �gypten, �quatorial-Guinea"},
    /* 7 */	{"Ausland 7", "Argentinien, Bahrain, Benin, Costa Rica, Elfenbeink�ste Cote de Ivoire, Fidschi, Franz�sisch-Polynesien, Gambia, Guinea, Irak, Jamaika, Britische Jungferninseln, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar Burma, Namibia, Nauru, Nepal, Neukaledonien, Nicaragua, Nigeria, Niue-Inseln, Norfolkinseln, Oman, Peru, Ruanda, St. Helena, St. Lucia, Sambia, Samoa (West), Simbabwe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks- und Caicosinseln, Uganda, Vanuatu, Zentralafrikanische Republik, �thiopien"},
    /* 8 */	{"Ausland 8", "Afghanistan, Amerikanisch-Samoa, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso Obervolta, Kaimaninseln, Cookinseln, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Jemen (Arab. Republik), Kambodscha, Kamerun, Cape Verde, Kongo, Nordkorea, Kuba, Madagaskar, Mali, Marshallinseln, Mauritius, Mikronesien, Montserrat, Niger, Pakistan, Palau /Belau, Papua-Neuguinea, Paraguay, St. Kitts und Nevis, Salomonen, Senegal, Seyschellen, Sierra Leone, Somalia, Sri Lanka, Dschibuti, Tschad, Tuvalu, Uruguay, Vietnam, Zaire"},
  };
  int z;
  rprintf ("P:95,1", "Priority Telecom Classic" );
  rprintf ("D:priority", "# Verzonung" );
  rprintf ("C:Name:", "Priority Telecom, Telekabel Wien Ges.m.b.H." );
  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.prioritytelecom.at" );
  rprintf ("C:EMail:", "info@prioritytelecom.at" );
  rprintf ("C:Telefon:", "0800 700 700" );
  rprintf ("# Fixme:", "Taktung ?" );
  rprintf ("C:Special:", "Nur �ber Telekabel Wien" );
  rprintf ("C:GF:", "Cost=136-198" );
  rprintf ("C:GT:", "ATS 136 Grundgeb�hr/Monat" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=0.30|%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=0.30|%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=0.30|%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=0.30|%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_priority_stand(void) {
  double Tarif[][2] = {
    {0.80,0.40}, // Regional
    {1.00,0.80}, //Inland
    {4.00,3.00},  // Mobil
    {0.54,0.36}, //   Priority
    {2.70,2.50},  /* Ausland 1 ... */
    {4.20,3.50},  /* Ausland 2 ... */
    {5.50,4.20},  /* Ausland 3 ... */
    {6.50,6.00},  /* Ausland 4 ... */
    {9.00,8.00},  /* Ausland 5 ... */
    {15.00,13.00},  /* Ausland 6 ... */
    {20.00,19.00},  /* Ausland 7 ... */
    {25.00,23.00},  /* Ausland 8 ... */
  };
  char *Zone[][2] =  {
    { "Regional","" },
    { "�sterreich", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    { "Priority","" },
    /* 1 */	{"Ausland 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakische Republik, Slowenien, Tschechische Republik, Ungarn"},
    /* 2 */	{"Ausland 2", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Amerikanische Jungferninseln, Kanada, Kanarische Inseln, Luxemburg, Monaco, Niederlande, Norwegen, Puerto Rico, Schweden, Spanien, USA"},
    /* 3 */	{"Ausland 3", "Albanien, Andorra, Weissrussland, Bosnien-Herzegowina, Bulgarien, Estland, Gibraltar, Griechenland, Jugoslawien, Kroatien, Lettland, Malta, Mazedonien, Polen, Portugal, Rum�nien, San Marino, Tunesien, T�rkei, Ukraine, Vatikan, Zypern"},
    /* 4 */	{"Ausland 4", "Algerien, F�r�er-Inseln, Georgien, Island, Israel, Litauen, Marokko, Russische F�rderation (�stl.), Tadschikistan, Turkmenistan, Usbekistan"},
    /* 5 */	{"Ausland 5", "Antarktis, Niederl. Antillen, Armenien, Aserbaidschan, Australien, Chile, Weihnachtsinseln, Kokosinseln, Franz. Guyana, Guadeloupe, Hongkong, Japan, S�dkorea, Libyen, Malaysia, Martinique / Franz. Antillen, Mayotte, Moldavien Moldau (Republik), Neuseeland, Philippinen, Reunion, St. Pierre und Miquelon, Marianen (SaipanNord-), S�dafrika"},
    /* 6 */	{"Ausland 6", "Angola, Aruba, Bahamas, Barbados, Belize, Bermuda, Bhutan, Botsuana Botswana, Brasilien, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Iran, Jordanien, Kirgistan, Kolumbien, Kuwait, Libanon, Macao, Mexiko, Panama, St. Vincent und Grenadinen, Sao Tome und Principe, Saudi Arabien, Singapur, Trinidad und Tobago, Venezuela, Ver. Arabische Emirate, �gypten, �quatorial-Guinea"},
    /* 7 */	{"Ausland 7", "Argentinien, Bahrain, Benin, Costa Rica, Elfenbeink�ste Cote de Ivoire, Fidschi, Franz�sisch-Polynesien, Gambia, Guinea, Irak, Jamaika, Britische Jungferninseln, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar Burma, Namibia, Nauru, Nepal, Neukaledonien, Nicaragua, Nigeria, Niue-Inseln, Norfolkinseln, Oman, Peru, Ruanda, St. Helena, St. Lucia, Sambia, Samoa (West), Simbabwe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks- und Caicosinseln, Uganda, Vanuatu, Zentralafrikanische Republik, �thiopien"},
    /* 8 */	{"Ausland 8", "Afghanistan, Amerikanisch-Samoa, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso Obervolta, Kaimaninseln, Cookinseln, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Jemen (Arab. Republik), Kambodscha, Kamerun, Cape Verde, Kongo, Nordkorea, Kuba, Madagaskar, Mali, Marshallinseln, Mauritius, Mikronesien, Montserrat, Niger, Pakistan, Palau /Belau, Papua-Neuguinea, Paraguay, St. Kitts und Nevis, Salomonen, Senegal, Seyschellen, Sierra Leone, Somalia, Sri Lanka, Dschibuti, Tschad, Tuvalu, Uruguay, Vietnam, Zaire"},
  };
  int z;
  rprintf ("P:95,2", "Priority Telecom Standard" );
  rprintf ("D:priority", "# Verzonung" );
  rprintf ("C:Name:", "Priority Telecom, Telekabel Wien Ges.m.b.H." );
  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.prioritytelecom.at" );
  rprintf ("C:EMail:", "info@prioritytelecom.at" );
  rprintf ("C:Telefon:", "0800 700 700" );
  rprintf ("# Fixme:", "Taktung ?" );
  rprintf ("C:Special:", "Nur �ber Telekabel Wien" );
  rprintf ("C:GF:", "Cost=200-198" );
  rprintf ("C:GT:", "ATS 200 Grundgeb�hr/Monat" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=0.30|%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=0.30|%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=0.30|%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=0.30|%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_priority_busi(void) {
  double Tarif[][2] = {
    {0.72,0.35}, // Regional
    {0.96,0.72}, //Inland
    {3.84,2.88},  // Mobil
    {0.48,0.34}, //   Priority
    {2.60,2.45},  /* Ausland 1 ... */
    {3.30,3.00},  /* Ausland 2 ... */
    {4.20,3.70},  /* Ausland 3 ... */
    {5.30,4.60},  /* Ausland 4 ... */
    {6.80,5.50},  /* Ausland 5 ... */
    {9.00,9.00},  /* Ausland 6 ... */
    {14.00,14.00},  /* Ausland 7 ... */
    {18.70,17.00},  /* Ausland 8 ... */
  };
  char *Zone[][2] =  {
    { "Regional","" },
    { "�sterreich", "�sterreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
    { "Priority","" },
    /* 1 */	{"Ausland 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakische Republik, Slowenien, Tschechische Republik, Ungarn"},
    /* 2 */	{"Ausland 2", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Amerikanische Jungferninseln, Kanada, Kanarische Inseln, Luxemburg, Monaco, Niederlande, Norwegen, Puerto Rico, Schweden, Spanien, USA"},
    /* 3 */	{"Ausland 3", "Albanien, Andorra, Weissrussland, Bosnien-Herzegowina, Bulgarien, Estland, Gibraltar, Griechenland, Jugoslawien, Kroatien, Lettland, Malta, Mazedonien, Polen, Portugal, Rum�nien, San Marino, Tunesien, T�rkei, Ukraine, Vatikan, Zypern"},
    /* 4 */	{"Ausland 4", "Algerien, F�r�er-Inseln, Georgien, Island, Israel, Litauen, Marokko, Russische F�rderation (�stl.), Tadschikistan, Turkmenistan, Usbekistan"},
    /* 5 */	{"Ausland 5", "Antarktis, Niederl. Antillen, Armenien, Aserbaidschan, Australien, Chile, Weihnachtsinseln, Kokosinseln, Franz. Guyana, Guadeloupe, Hongkong, Japan, S�dkorea, Libyen, Malaysia, Martinique / Franz. Antillen, Mayotte, Moldavien Moldau (Republik), Neuseeland, Philippinen, Reunion, St. Pierre und Miquelon, Marianen (SaipanNord-), S�dafrika"},
    /* 6 */	{"Ausland 6", "Angola, Aruba, Bahamas, Barbados, Belize, Bermuda, Bhutan, Botsuana Botswana, Brasilien, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Iran, Jordanien, Kirgistan, Kolumbien, Kuwait, Libanon, Macao, Mexiko, Panama, St. Vincent und Grenadinen, Sao Tome und Principe, Saudi Arabien, Singapur, Trinidad und Tobago, Venezuela, Ver. Arabische Emirate, �gypten, �quatorial-Guinea"},
    /* 7 */	{"Ausland 7", "Argentinien, Bahrain, Benin, Costa Rica, Elfenbeink�ste Cote de Ivoire, Fidschi, Franz�sisch-Polynesien, Gambia, Guinea, Irak, Jamaika, Britische Jungferninseln, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar Burma, Namibia, Nauru, Nepal, Neukaledonien, Nicaragua, Nigeria, Niue-Inseln, Norfolkinseln, Oman, Peru, Ruanda, St. Helena, St. Lucia, Sambia, Samoa (West), Simbabwe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks- und Caicosinseln, Uganda, Vanuatu, Zentralafrikanische Republik, �thiopien"},
    /* 8 */	{"Ausland 8", "Afghanistan, Amerikanisch-Samoa, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso Obervolta, Kaimaninseln, Cookinseln, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Jemen (Arab. Republik), Kambodscha, Kamerun, Cape Verde, Kongo, Nordkorea, Kuba, Madagaskar, Mali, Marshallinseln, Mauritius, Mikronesien, Montserrat, Niger, Pakistan, Palau /Belau, Papua-Neuguinea, Paraguay, St. Kitts und Nevis, Salomonen, Senegal, Seyschellen, Sierra Leone, Somalia, Sri Lanka, Dschibuti, Tschad, Tuvalu, Uruguay, Vietnam, Zaire"},
  };
  int z;
  rprintf ("P:95,3", "Priority Telecom Business" );
  rprintf ("D:priority", "# Verzonung" );
  rprintf ("C:Name:", "Priority Telecom, Telekabel Wien Ges.m.b.H." );
  rprintf ("C:Address:", "" );
  rprintf ("C:Homepage:", "http://www.prioritytelecom.at" );
  rprintf ("C:EMail:", "info@prioritytelecom.at" );
  rprintf ("C:Telefon:", "0800 700 700" );
  rprintf ("# Fixme:", "Taktung ?" );
  rprintf ("C:Special:", "Nur �ber Telekabel Wien" );
  rprintf ("C:GF:", "Cost=396-198" );
  rprintf ("C:GT:", "ATS 396 Grundgeb�hr/Monat, Businesskunden" );

  for (z=0; z<COUNT(Zone); z++) {
    rprintf ("Z:%d", Zone[z][0] , z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("T:*/*=0.30|%.2f(60)/1", "0-24h" , Tarif[z][0]);
    } else {
      rprintf ("T:W/8-18=0.30|%.2f(60)/1", "Tag" , Tarif[z][0]);
      rprintf ("T:W/18-8=0.30|%.2f(60)/1", "Nacht" , Tarif[z][1]);
      rprintf ("T:E,H/*=0.30|%.2f(60)/1", "Weekend" , Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}


int main (int argc, char *argv[])
{
  printf ("# created by rate-at.c\n\n");
  printf ("# The information herein was machine-generated,\n");
  printf ("# so do not contribute patches to this file.\n\n");
  printf ("# Please contact Michael Reinelt <reinelt@eunet.at>\n");
  printf ("# or Leo T�tsch <lt@toetsch.at> if you have any\n");
  printf ("# corrections or additions.\n\n");
  printf ("# Many thanks to Daniela Bruder <dbruder@sime.com>\n");
  printf ("# for collecting and preparing most of the call charges.\n\n\n");

  printf ("V:1.90-Austria [06-Nov-1999]\n\n");
  printf ("U:%%.3f �S\n");
  write_services();

#if 0
  rate_1066();
#else
  rate_1001_old();
  rate_1001();
  rate_1002();
  rate_1003();
  rate_1004_1();
  rate_1004_2();
  rate_1005();
  rate_1007();
  rate_1008();
  rate_1009();
  rate_1011();
  rate_10elf();
  rate_1012(1012);
  rate_1013();
  rate_1014();
  rate_1024();
  rate_1029_privat();
  rate_1029_business();
  rate_tns();
  rate_librotel();
  /*  rate_1044_old(); */
  rate_1012(1044);
  rate_1049();
  rate_1066();
  rate_1067();
  rate_1069();
  if(0) {
    rate_priority_class();
    rate_priority_stand();
    rate_priority_busi();
  }
#endif
  return(EXIT_SUCCESS);
}

/* template
   rprintf ("C:Name:", "" );
   rprintf ("C:Maintainer:", "" );
   rprintf ("C:TarifChanged:", "" );
   rprintf ("C:Address:", "" );
   rprintf ("C:Homepage:", "" );
   rprintf ("C:TarifURL:", "" );
   rprintf ("C:EMail:", "" );
   rprintf ("C:Telefon:", "" );
   rprintf ("C:Telefax:", "" );
   rprintf ("C:Hotline:", "" );
   rprintf ("C:Zone:", "" );
   rprintf ("C:Special:", "" );
   rprintf ("C:GT:", "" );
   rprintf ("C:GF:", "" );
*/

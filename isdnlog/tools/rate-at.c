#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define TABSTOP 35
#define TAGSTOP 75

#define COUNT(array) sizeof(array)/sizeof(array[0])

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

void rprintf (char *name, char *fmt, ...) 
{
  char buf[BUFSIZ], *p;
  int len;
  va_list ap;
  
  va_start(ap, fmt);
  len=vsnprintf(buf, BUFSIZ, fmt, ap);
  if (name && *name) {
    p=buf+len;
    for (p=buf+len; len<TABSTOP;len++)
      *(p++)=' ';
    *p='\0';
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
	rprintf ("alle �brigen L�nder", "A:+");
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

void rate_sample(void) {

  char *Zone[][2] = {{ "", "" }, 
		     { "", "" },
		     { "", "+" }};
  
  double Tarif[] = { 01.00,
		     22.00 };
  
  int z;
  
  printf ("\n");
  rprintf ("Sample", "P:00");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/60", Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1001_old(void) {

  char *Name[] = { "Minimumtarif",
		   "Standartarif",
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
    printf ("\n");
    rprintf ("Telekom Austria", "P:01,%d", t+1);
    if (t==0)
      rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:maintained by:");
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name[t], Tarif[t]);
    rprintf (s, "# Tarif:");
    rprintf ("# Verzonung", "D:pta");
    for (z=0; z<COUNT(Zone); z++) {
      printf ("\n");
      rprintf (Zone[z][0], "Z:%d", z);
      if (z==0) { /* Freephone */
	rprintf (NULL,"T:*/*=0/72");
      } else if (Faktor[z][0]) {
	if (Faktor[z][0]==Faktor[z][1]) {
	  rprintf ("Tageszeit", "T:1-5/08-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("Tageszeit 1",   "T:1-5/08-12=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Tageszeit 1",   "T:1-4/13-16=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Tageszeit 2",   "T:1-4/12-13,16-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("Tageszeit 2",   "T:5/12-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	}
	if (Faktor[z][2]==Faktor[z][3]) {
	  rprintf ("Sparzeit", "T:1-5/18-8=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("Sparzeit", "T:E,H/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	} else {
	  rprintf ("Sparzeit",      "T:1-5/06-08,18-20=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("Sparzeit",      "T:E,H/06-20=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("Supersparzeit", "T:*/20-06=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][3]);
	}
      }
      print_area(Zone[z][1]);
    }
  }    
}


void rate_1001(void) {

  char *Name[] = { "Minimumtarif",
		   "Standartarif",
		   "Gesch�ftstarif 1",
		   "Gesch�ftstarif 2",
		   "Gesch�ftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "FreePhone", "112,122,133,141,142,144,+43800,+43801,+43802,+43803,+43804" },
		     { "Regionalzone", "111*,11820,15*,+437111,+437112,+437113,+437114,+43810" }, 
		     { "�sterreichzone", "�sterreich" },
		     { "Mobilfunk 1", "+43663,+43664,+43676" },
		     { "Mobilfunk 2", "+43699" },
		     { "Online", "+437189,19411,19430,19440" },
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

  for (t=0; t<4; t++) {
    printf ("\n");
    rprintf ("Telekom Austria", "P:01,%d", t+1);
    if (t==0)
      rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:maintained by:");
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name[t], Tarif[t]);
    rprintf (s, "# Tarif:");
    rprintf ("# Verzonung", "D:pta");
    for (z=0; z<COUNT(Zone); z++) {
      printf ("\n");
      rprintf (Zone[z][0], "Z:%d", z);
      if (z==0) { /* Freephone */
	rprintf (NULL,"T:*/*=0/72");
      } else if (Faktor[z][0]) {
	if (Faktor[z][0]==Faktor[z][1]) {
	  rprintf ("rund um die Uhr", "T:*/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("Gesch�ftszeit",   "T:1-5/08-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Freizeit",        "T:1-5/18-08=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("Freizeit",        "T:E,H/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	}
      }
      print_area(Zone[z][1]);
    }
  }    
}


void rate_1002(void) {

  char *Zone[][2] = {{ "Bundeslandzone", "" }, 
		     { "�sterreichzone", "�sterreich" },
		     { "special friends", "" },
		     { "Mobilnetz", "+43663,+43664,+43676,+43699" },
		     { "EasyInternet", "" },
		     { "Nachbarl�nder West", "Deutschland, Italien, Liechtenstein, Schweiz" },
		     { "Nachbarl�nder Ost", "Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 3, USA", "Belgien, D�nemark, Frankreich, Gro�britannien, Irland, Amerikanische Jungferninseln, Kanada, Luxemburg, Niederlande, Norwegen, Puerto Rico, Schweden, Gro�britannien, Vereinigte Staaten (USA), Alaska, Hawaii" },
		     { "Europa 4", "Andorra, Bosnien-Herzegowina, Finnland, Griechenland, Kanarische Inseln, Kroatien, Madeira, Monaco, Polen, Portugal, San Marino, Spanien, Vatikan" },
		     { "Europa 5", "Albanien, Azoren, Belarus, Bulgarien, Estland, Lettland, Malta, Mazedonien, Rum�nien, T�rkei, Ukraine, Zypern" },
		     { "Europa 6", "Algerien, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Litauen, Marokko, Ru�land, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Fernost, S�dafrika", "Australien, Hongkong, Japan, Kokosinseln, Neuseeland, Singapur, S�dafrika, S�dkorea, Weihnachtsinseln" },
		     { "Welt 1", "Armenien, Aserbaidschan, Chile, Franz�sisch-Guayana, Guadeloupe, Libyen, Martinique, Reunion" },
		     { "Welt 2", "Angola, Antarktis, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Kuwait, Macao, Malaysia, Mexiko, Saint Pierre und Miquelon, Moldau, Niederl�ndische Antillen, Philippinen, Saipan, Saudi-Arabien, Venezuela" },
		     { "Welt 3", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Bahrain, Barbados, Belize, Botswana, Brunei, Burundi, Dominica, Dominikanische Republik, Elfenbeink�ste, Gabun, Ghana, Grenada, Gr�nland, Guinea, Guinea-Bissau, Guyana, Jordanien, Britische Jungferninseln, Kasachstan, Katar, Kirgisistan, Kolumbien, Komoren, Laos, Lesotho, Libanon, Liberia, Malawi, Mayotte, Namibia, Nauru, Nepal, Neukaledonien, Saint Kitts und Nevis, Nigeria, Norfolk-Inseln, Panama, Peru, Sambia, Sao Tome und Principe, Simbabwe, Saint Helena, Saint Kitts und Nevis, Saint Lucia, Saint Vincent und die Grenadinen, Sudan, Swasiland, Syrien, Tansania, Tonga, Trinidad & Tobago, Turks- und Caicos-Inseln, Vereinigte Arabische Emirate, Samoa" },
		     { "Welt 4", "Anguilla, Argentinien, �thiopien, Benin, Bolivien, Caymaninseln, Costa Rica, Dschibuti, Fidschi, Franz�sisch-Polynesien, Gambia, Indien, Indonesien, Irak, Jamaica, Kenia, Kiribati, Kuba, Malediven, Mauretanien, Mauritius, Mongolei, Mosambik, Myanmar, Nicaragua, Niger, Niue, Nordkorea, Oman, Papua-Neuguinea, Paraguay, Ruanda, Sierra Leone, Sri Lanka, Surinam, Taiwan, Togo, Uganda, Uruguay, Vanuatu, Zentralafrikanische Republik" },
		     { "Welt 5", "Afghanistan, Amerikanisch-Samoa, Antigua & Barbuda, Ascension, Bangladesch, Burkina Faso, Cook-Inseln, Diego Garcia, El Salvador, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Kamerun, Kongo, Madagaskar, Honduras, Jemen, Kambodscha, Kamerun, Kap Verde, Kongo, Madagaskar, Mali, Marshallinseln, Midway-Inseln, Mikronesien, Montserrat, Pakistan, Palau, Salomonen, Senegal, Seychellen, Somalia, Tokelau, Tschad, Tuvalu, Vietnam, Wake-Inseln, Wallis- und Futuna-Inseln, Zaire" }};
  
  double Tarif[][2] = {{  1.00,  0.88 },
		       {  1.50,  1.00 },
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
  rprintf ("angrenzende Bezirke werden falsch verzont", "# Fixme:");
  rprintf ("# Verzonung", "D:uta");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("Gesch�ftszeit", "T:1-5/8-18=%.2f(60)/1", Tarif[z][0]);
    rprintf ("Freizeit", "T:1-5/18-8=%.2f(60)/1", Tarif[z][1]);
    rprintf ("Freizeit", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
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
		     { "Ausland Zone 5", "Algeria, Antarctica, Australia, Christmas Island, Cocos Island, Estonia, F�r�er, Gibraltar, Japan, South Korea, Latvia, Moldova, Marocco, Russian Federation, South Africa, Tajikistan" },
		     { "Ausland Zone 6", "Azerbaijan, Chile, French Guyana, Georgia, Greenland, Guadaloupe, Hongkong, Iceland, Libya, Lithuania, Martinique, Mayotte, Mexico, Phillipines, Reunion, St. Pierre and Miquelon, Singapore, Thailand, Turkmenistan, Uganda, United Arab Emirates" },
		     { "Ausland Zone 7", "Angola, Argentina, Bahrain, Bhutan, Botswana, Brazil, Burundi, Comoros, Dominican Republic, Ecuador, Egypt, Gabon, Gambia, India, Indonesia, Iran, Jordan, Kuwait, Kyrgyzstan, Macau, Malawi, Malaysia, Namibia, Netherlands Antilles, New Zealand, Nigeria, Oman, Saipan, Saudi Arabia, Sri Lanka, Swaziland, Taiwan, Venezuela, Zambia, Zimbabwe" },
		     { "Ausland Zone 8", "Armenia, Aruba, Bahamas, Barbados, Belize, Bermuda, Brunei, Central African Republic, China, Colombia, Costa Rica, Cuba, Djibouti, Equatorial Guinea, Ghana, Guinea-Bissau, Guyana, Kazakhstan, Lesotho, Liberia, Mongolia, Nepal, Panama, Saint Helena, Sao Tome and Principa, Sudan, Syria, Tanzania, Trinidad & Tobago, Turks & Caicos Islands" },
		     { "Ausland Zone 9", "Ascension, Benin, Bolivia, Burkina Faso, Cameroon, Cape Verde, Chad, Elfenbeink�ste, El Salvador, Ethiopia, Fiji, French Polynesia, Guatemala, Haiti, Honduras, Iraq, Jamaica, Kenya, Kiribati, Laos, Lebanon, Maldives, Mali, Mauritania, Mauritius, Mozambique, Myanmar, Nauru, New Caledonia, Nicaragua, Niue, Norfolk Island, Pakistan, Papua New Guinea, Peru, Quatar, Rwanda, Saint Lucia, St. Vincent and the Grenadines, Samoa, Senegal, Seychelles Islands, Sierra Leone, Somalia, Suriname, Togo, Tonga, Uzbekistan, Vanuatu, Jemen, Zaire" },
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
  
  printf ("\n");
  rprintf ("Multikom", "P:03");
  rprintf ("Verzonung nicht verifiziert", "# Fixme:");
  rprintf ("# Verzonung", "D:pta");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/60/1", Tarif[z][0]);
    else {
      rprintf ("Tag", "T:*/8-18=%.2f(60)/60/1", Tarif[z][0]);
      rprintf ("Nacht", "T:*/18-8=%.2f(60)/60/1", Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1005(void) {

  char *Zone[][2] = {{ "National", "�sterreich" }, 
		     { "Mobilfunknetz", "+43663,+43664,+43676,+43699" },
		     { "Europa 1 / USA", "Belgien, D�nemark, Deutschland,  Finnland, Frankreich, Griechenland, Grossbritannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Nordirland, Norwegen, Schweden, Schweiz, Slowakei, Slowenien, Spanien,  Tschechien, Ungarn" },
		     { "Europa 2", "Albanien, Algerien, Andorra, Bosnien-Herzegowina, Bulgarien, Estland, F�r�er-Inseln, Gibraltar, Island, Israel, Jugoslawien, Kroatien, Lettland, Litauen, Mazedonien, Malta, Marokko, Polen, Portugal, Rum�nien, Russland, San Marino, Tunesien, T�rkei, Zypern" },
		     { "Welt 1", "Australien, Hongkong, Japan, Neuseeland, S�dkorea, Singapur" },
		     { "Welt 2", "+" }};
  
  double Tarif[] = { 1.00, 4.00, 3.50, 6.00, 9.00, 19.00 };
  
  int z;
  

  printf ("\n");
  rprintf ("tele2", "P:05");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/60/1", Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_1007(void) {

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
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
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
    printf ("\n");
    rprintf ("European Telecom", "P:07,%d", t+1);
    rprintf ("Verzonung nicht verifiziert", "# Fixme:");
    rprintf ("# Verzonung", "D:1007");
    rprintf (Name[t], "# Tarif:");
    for (z=0; z<COUNT(Zone); z++) {
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (Tarif[t][z][0]==Tarif[t][z][1]) {
	rprintf ("Tageszeit", "T:1-5/08-18=%.3f(60)/1", Tarif[t][z][0]);
      } else {
	rprintf ("Tageszeit 1",   "T:1-5/08-12=%.3f(60)/1", Tarif[t][z][0]);
	rprintf ("Tageszeit 1",   "T:1-4/13-16=%.3f(60)/1", Tarif[t][z][0]);
	rprintf ("Tageszeit 2",   "T:1-4/12-13,16-18=%.3f(60)/1", Tarif[t][z][1]);
	rprintf ("Tageszeit 2",   "T:5/12-18=%.3f(60)/1", Tarif[t][z][1]);
      }
      if (Tarif[t][z][2]==Tarif[t][z][3]) {
	rprintf ("Sparzeit", "T:1-5/18-8=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Sparzeit", "T:E,H/*=%.3f(60)/1", Tarif[t][z][2]);
      } else {
	rprintf ("Sparzeit",      "T:1-5/06-08,18-20=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Sparzeit",      "T:E,H/06-20=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Supersparzeit", "T:*/20-06=%.3f(60)/1", Tarif[t][z][3]);
      }
      print_area(Zone[z][1]);
    }
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
		     {"Ausland 4", "Wei�ru�land, F�r�er-Inseln, Griechenland, Jugoslawien, Serbien, Montenegro, Britische Jungferninseln, Kanarische Inseln, Palau, Rum�nien, Tunesien, T�rkei, Ukraine"},
		     {"Ausland 5", "Albanien, Algerien, Antarktis, Australien, Estland, Lettland, Malawi, Malaysia, Marokko, Moldau, Moldau"},
		     {"Ausland 6", "Gibraltar, Island, Israel"},
		     {"Ausland 7", "Litauen, Russische F�deration, Tadschikistan"},
		     {"Ausland 8", "Georgien, Hongkong, Japan, S�dkorea, Neuseeland, Singapur, Turkmenistan, Usbekistan"},
		     {"Ausland 9", "Armenien, Aserbaidschan, Weihnachtsinseln, Kokosinseln, Mauritius, Mexiko"},
		     {"Ausland 10", "Chile, Franz�sisch-Guayana, Guadeloupe, Libyen, Peru, Reunion, St. Pierre und Miquelon, S�dafrika"},
		     {"Ausland 11", "Brasilien, Niederl�ndische Antillen"},
		     {"Ausland 12", "Angola, Bermuda, Bhutan, China, Ecuador, Iran, Kuwait, Mali, Saudi-Arabien, Venezuela, Vereinigte Arabische Emirate"},
		     {"Ausland 13", "Bahamas, Dominikanische Republik, Madagaskar, �gypten"},
		     {"Ausland 14", "Aruba, Barbados, Belize, Botswana, Brunei, Burundi, El Salvador, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Katar, Kirgisistan, Kolumbien, Libanon, Martinique, Mayotte, Sao Tome und Principe, Saint Vincent und die Grenadinen, Taiwan, Trinidad und Tobago, �quatorial-Guinea"},
		     {"Ausland 15", "Argentinien, Bahrain, Benin, Cote d'Ivoire, Dominica, Grenada, Guinea, Kasachstan, Kenia, Komoren, Laos, Lesotho, Liberia, Mauretanien, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Sambia, Samoa, Simbabwe, St. Helena, Saint Kitts und Nevis, St. Lucia, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln"},
		     {"Ausland 16", "Mikronesien, Mikronesien, �thiopien"},
		     {"Ausland 17", "Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Indonesien, Irak, Jamaika, Kiribati, Nicaragua, Niue, Oman, Panama, Philippinen, Ruanda, Amerikanisch-Samoa, Suriname, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik"},
		     {"Ausland 18", "Afghanistan, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso, Cook-Inseln, Caymaninseln, Dschibuti, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Honduras, Indien, Jemen, Kambodscha, Kamerun, Kap Verde, Kongo, Nordkorea, Kuba, Macao, Malta, Marshallinseln, Mazedonien, Mazedonien, Niger, Pakistan, Salomonen, Senegal, Seychellen, Sierra Leone, Somalia, Sri Lanka, Tschad, Tuvalu, Uruguay, Vietnam, Wallis- und Futuna-Inseln, Zaire"}};
  
  double Tarif[][2] = {{ 1.00, 0.88 },
		       { 1.68, 0.88 },
		       { 4.00, 4.00 },
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
  
  printf ("\n");
  rprintf ("RSL COM", "P:11");
  rprintf ("# Verzonung", "D:1012");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      rprintf ("Tag", "T:1-5/8-18=%.2f(60)/1", Tarif[z][0]);
      rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/1", Tarif[z][1]);
      rprintf ("Weekend", "T:E/*=%.2f(60)/1", Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1012(void) {

  char *Zone[][2] = {{ "Nah", "" }, 
		     { "Fern", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Nachbarn", "Frankreich, Italien, Schweiz, Liechtenstein, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 1 + USA", "Belgien, D�nemark, Finnland, Gro�britannien, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Schweden, Spanien, Vereinigte Staaten (USA)" },
		     { "Europa 2", "Andorra, Bosnien-Herzegowina, F�r�er-Inseln, Jugoslawien, Serbien, Montenegro, Kroatien, Malta, Monaco" },
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
  
  printf ("\n");
  rprintf ("1012", "P:12");
  rprintf ("# Verzonung", "D:1012");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (z<3) {
      rprintf ("Tag", "T:1-5/8-18=%.2f(60)/60/1", Tarif[z][0]);
      rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/60/1", Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/60/1", Tarif[z][1]);
    } else {
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/60/1", Tarif[z][0]);
    }
    print_area(Zone[z][1]);
  }
}    

void rate_1024(void) {

  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Bundesland", "" },
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland, Schweiz", "Deutschland, Schweiz" },
		     { "International A", "Belgien, D�nemark, Finnland, Frankreich, Gro�britannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Norwegen, Schweden, Slowenien, Slowakei, Spanien, Tschechien, Ungarn, Vatikan, Vereinigte Staaten (USA), Puerto Rico, Alaska, Hawaii, Kanada" },
		     { "International B", "Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Griechenland, Island, Kroatien, Lettland, Litauen, Malta, Mazedonien, Polen, Portugal, Rum�nien, Serbien, Montenegro, Zypern, Australien, F�r�er, Israel, Neuseeland, Amerikanische Jungferninseln" },
		     { "International C", "Albanien, Weihnachtsinseln, Gibraltar, Kokosinseln, Ru�land, San Marino, T�rkei, Tunesien, Ukraine, Wei�ru�land, Algerien, Hongkong, Japan, S�dkorea, Marokko, Philippinen, Russische F�deration, Singapur" },
		     { "International D", "�gypten, S�dafrika, Aserbaidschan, Armenien, China, Georgien, Libyen, Malaysia, Moldau, Tadschikistan, Taiwan, Turkmenistan, Usbekistan" },
		     { "International E", "Angola, Aruba, Bahamas, Bahrain, Barbados, Bermuda, Bhutan, Bolivien, Botswana, Brunei, Burundi, Ghana, Gr�nland, Guadeloupe, Guam, Guinea, Martinique, Franz�sisch-Guayana, Honduras, Indien, Indonesien, Iran, Jordanien, Kasachstan, Kirgisistan, Kuba, Kuwait, Lesotho, Libanon, Liberia, Macao, Malawi, Martinique, Mayotte, Namibia, Niederl�ndische Antillen, Nigeria, Paraguay, Reunion, Saipan, Saudi-Arabien, St. Lucia, St. Pierre und Miquelon, Saint Vincent und die Grenadinen, Sudan, Syrien, Swasiland, Tansania, Thailand, Trinidad und Tobago, Uruguay, Vereinigte Arabische Emirate" },
		     { "International F", "+" }};
  
  double Tarif[][2] = {{ 0.98, 0.88},
		       { 1.28, 1.18},
		       { 1.98, 1.18},
		       { 3.98, 3.98},
		       { 3.48, 3.48},
		       { 3.98, 3.98},
		       { 5.98, 5.98},
		       { 7.38, 7.38},
		       {12.98,12.98},
		       {14.98,14.98},
		       {19.98,19.98}};
  
  int z;
  
  printf ("\n");
  rprintf ("Telepassport", "P:24");
  rprintf ("'International D': Zentral- und S�damerika nicht implementiert", "# Fixme:");
  rprintf ("# Verzonung", "D:1024");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      rprintf ("Tag", "T:1-5/8-18=%.2f(60)/1", Tarif[z][0], Tarif[z][0]);
      rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1029(void) {

  char *Name[] = { "easy.Talk privat",
		   "easy.Talk business" };
  
  double Einheit[] = {1.116, 1.029};
  
  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "Mobilfunk A1, max", "+43663,+43664,+43676" },
		     { "Mobilfunk One", "+43699" },
		     { "Deutschland", "Deutschland" },
		     { "Schweiz", "Schweiz" },
		     { "USA", "Vereinigte Staaten (USA)" },
		     { "International", "+" }};
  
  double Tarif[2][9][4] = {{{ 0.99, 0.84, 0.57, 0.39},
			    { 2.67, 2.41, 1.21, 0.87},
			    { 2.67, 2.62, 1.61, 1.26},
			    { 4.46, 4.46, 3.14, 3.14},
			    { 4.46, 4.46, 3.14, 3.14},
			    { 3.50, 3.50, 3.50, 3.50},
			    { 3.50, 3.50, 3.50, 3.50},
			    { 5.25, 5.25, 5.25, 5.25},
			    { 0.00, 0.00, 0.00, 0.00}},
			
			   {{ 0.96, 0.96, 0.96, 0.96},
			    { 2.52, 2.52, 1.20, 1.20},
			    { 2.52, 2.52, 1.20, 1.20},
			    { 4.48, 4.48, 4.48, 4.48},
			    { 4.58, 4.48, 4.48, 4.48},
			    { 3.24, 3.24, 3.24, 3.24},
			    { 3.84, 3.84, 3.84, 3.84},
			    { 4.38, 4.38, 4.38, 4.38},
			    { 0.00, 0.00, 0.00, 0.00}}};
			 
  int t, z;
  char s[BUFSIZ];

  for (t=0; t<2; t++) {
    printf ("\n");
    rprintf ("CyberTron", "P:29,%d", t+1);
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name[t], Einheit[t]);
    rprintf (s, "# Tarif:");
    rprintf ("Internationale Zonen nicht implementiert!", "# Fixme:");

    for (z=0; z<COUNT(Zone); z++) {
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (Tarif[t][z][0]!=0.0) { /* Tarif bekannt? */
	if (Tarif[t][z][0]==Tarif[t][z][1] && Tarif[t][z][0]==Tarif[t][z][2] && Tarif[t][z][0]==Tarif[t][z][3]) {
	  rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/%.5g", Tarif[t][z][0], Einheit[t]/Tarif[t][z][0]);
	} else {
	  if (Tarif[t][z][0]==Tarif[t][z][1]) {
	    rprintf ("Tageszeit", "T:1-5/8-18=%.2f(60)/%.5g", Tarif[t][z][0], Einheit[t]/Tarif[t][z][0]);
	  } else {
	    rprintf ("Tageszeit 1", "T:1-5/8-12=%.2f(60)/%.5g", Tarif[t][z][0], Einheit[t]/Tarif[t][z][0]);
	    rprintf ("Tageszeit 1", "T:1-4/13-16=%.2f(60)/%.5g", Tarif[t][z][0], Einheit[t]/Tarif[t][z][0]);
	    rprintf ("Tageszeit 2", "T:1-4/12-13,16-18=%.2f(60)/%.5g", Tarif[t][z][1], Einheit[t]/Tarif[t][z][1]);
	    rprintf ("Tageszeit 2", "T:5/12-18=%.2f(60)/%.5g", Tarif[t][z][1], Einheit[t]/Tarif[t][z][1]);
	  }
	  if (Tarif[t][z][2]==Tarif[t][z][3]) {
	    rprintf ("Sparzeit", "T:1-5/08-18=%.2f(60)/%.5g", Tarif[t][z][2], Einheit[t]/Tarif[t][z][2]);
	  } else {
	    rprintf ("Sparzeit",      "T:1-5/06-08,18-20=%.2f(60)/%.5g", Tarif[t][z][2], Einheit[t]/Tarif[t][z][2]);
	    rprintf ("Sparzeit",      "T:E,H/06-20=%.2f(60)/%.5g", Tarif[t][z][2], Einheit[t]/Tarif[t][z][2]);
	    rprintf ("Supersparzeit", "T:*/20-06=%.2f(60)/%.5g", Tarif[t][z][3], Einheit[t]/Tarif[t][z][3]);
	  }
	}
      }
      print_area(Zone[z][1]);
    }
  }
}

void rate_1044(void) {

  char *Zone[][2] = {{ "Regional", "" }, 
		     { "�sterreich", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland", "+" }};
  
  double Tarif[][2] = {{ 0.72, 0.72 },
		       { 1.44, 0.72 },
		       { 4.20, 4.20 },
		       { 0.00, 0.00 }};
  
  int z;
  
  printf ("\n");
  rprintf ("Citykom", "P:44");
  rprintf ("# Verzonung", "D:1012");
  rprintf ("internationale Zonen nicht implementiert", "# Fixme:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif [z][0]!=0.0) { /* Tarif bekannt? */
      if (Tarif[z][0]==Tarif[z][1]) {
	rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/1", Tarif[z][0]);
      } else {
	rprintf ("Tag", "T:1-5/8-18=%.2f(60)/1", Tarif[z][0]);
	rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/1", Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
      }
    }
    print_area(Zone[z][1]);
  }
}

void rate_1049(void) {

  char *Zone[][2] = {{ "Regionalzone", ""},
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "�sterreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Europa 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, D�nemark,Finnland, Frankreich, Gro�britannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rum�nien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Global 1", "Algerien, Estland, F�r�er-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische F�deration, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Global 2", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Global 3", "Chile, Franz�sisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, S�dafrika" },
		     { "Global 4", "Japan, S�dkorea, Malaysia, Niederl�ndische Antillen, Philippinen, Saipan" },
		     { "Global 5", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Global 6", "�gypten, �quatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Gr�nland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Global 7", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Global 8", "Argentinien, �thiopien, Benin, Costa Rica, Fidschi, Franz�sisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Global 9", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Global 10", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Global 11", "Cook-Inseln, Madagaskar" },
		     { "Global 12", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Global 13", "Griechenland, Polen, Tunesien, T�rkei" },
		     { "Global 14", "Afghanistan, Amerikanisch-Samoa, Guantanamo, Marshallinseln, Midway-Inseln, Mikronesien, Palau, Pitcairn-Inseln, Tuvalu, Wake-Inseln" },
		     { "Grenzgebiete", "" }};
  

  double Tarif[] = { 1.02,
		     2.16,
		     3.24,
		     4.68,
		     3.96,
		     5.28,
		     7.20,
		     9.24,
		     11.28,
		     13.20,
		     15.24,
		     17.28,
		     19.80,
		     22.56,
		     25.80,
		     29.16,
		     39.12,
		     6.00,
		     5.28,
		     48.96,
		     3.96};

  int z;
  
  printf ("\n");
  rprintf ("Pegasus Telecom", "P:49");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/20/1", Tarif[z]);
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
		     { "Welt 4", "Afghanistan, Algerien, Anguilla, Antigua und Barbuda, �quatorial-Guinea, �thiopien, Bangladesch, Cook-Inseln, Cote d'Ivoire, Diego Garcia, Eritrea, Guam, Guantanamo, Guinea, Guinea-Bissau, Guyana, Irak, Jemen, Kambodscha, Kamerun, Kap Verde, Kasachstan, Kiribati, Komoren, Madagaskar, Mali, Midway-Inseln, Mikronesien, Mongolei, Mosambik, Myanmar, Namibia, Nauru, Niger, Niue, Oman, Pakistan, Palau, Papua-Neuguinea, Pitcairn-Inseln, Sao Tome und Principe, Amerikanisch-Samoa, Samoa, Senegal, Seychellen, Sierra Leone, Sri Lanka, Suriname, Swasiland, Togo, Tonga, Tschad, Turks- und Caicos-Inseln, Tuvalu, Uganda, Vietnam, Wake-Inseln, Zaire, Zentralafrikanische Republik" }};
  
  double Tarif[][2] = {{ 0.96, 0.80},
		       { 2.26, 1.86 },
		       { 4.26, 3.60 },
		       { 4.00, 3.66 },
		       { 5.56, 5.56 },
		       { 3.76, 3.46 },
		       { 6.66, 6.66 },
		       {11.66,11.66 },
		       {19.66,19.66 },
		       {26.66,26.66 }};
		       
  int z;
  
  printf ("\n");
  rprintf ("MIT 1066", "P:66");
  rprintf ("# Verzonung", "D:1066");
  rprintf ("Verzonung -50/+50 nicht verifiziert", "# Fixme");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      if (18.0/Tarif[z][0]>1.0)
	rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/%.2f/1", Tarif[z][0], 18.0/Tarif[z][0]);
      else
	rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      if (18.0/Tarif[z][0]>1.0)
	rprintf ("Tag", "T:1-5/8-18=%.2f(60)/%.2f/1", Tarif[z][0], 18.0/Tarif[z][0]);
      else
	rprintf ("Tag", "T:1-5/8-18=%.2f(60)/1", Tarif[z][0]);
      if (18.0/Tarif[z][1]>1.0) {
	rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/%.2f/1", Tarif[z][1], 18.0/Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=%.2f(60)/%.2f/1", Tarif[z][1], 18.0/Tarif[z][1]);
      } else {
	rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/1", Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
      }
    }
    print_area(Zone[z][1]);
  }
}

void rate_1067(void) {
  
  char *Zone[][2] = {{ "Festnetz", "�sterreich" }, 
		     { "max.box", "+4367622" },
		     { "max.online", "+436762323" },
		     { "max.mobil", "+43676" },
		     { "andere Mobilfunknetze", "+43663,+43664,+43669" },
		     { "Nachbarl�nder", "Deutschland, Italien, Schweiz, Tschechische Republik, Ungarn, Slowakische Republik, Slowenien, Liechtenstein" },
		     { "EU", "Belgien, D�nemark, Finnland, Frankreich, Griechenland, Irland, Luxemburg, Niederlande, Portugal, Spanien, Schweden, Gro�britannien, Nordirland" },
		     { "Weltzone 1", "Andorra, Australien, Bahamas, Bosnien-Herzegowina, Kanada, Kroatien, Far�er, Gibraltar, Island, Japan, Lettland, Malta, Mazedonien, Monaco, Neuseeland, Norwegen, Polen, Puerto Rico, San Marino, Singapur, USA, Vatikan, Jugoslawien" },
		     { "Weltzone 2", "Albanien, Algerien, Belarus, Bulgarien, Zypern, Estland, Hong Kong, Litauen, Marokko, Oman, Rum�nien, Ru�land, Tunesien, T�rkei, Ukraine, Virgin Islands (US)" },
		     { "Weltzone 3", "Bermuda, Brasilien, Brunei, Chile, Christmas Islands, Cocos Islands, Dominikanische Republik, Georgien, Ghana, Gr�nland, Israel, Jamaica, Kasachstan, Korea, Kirgisistan, Libyen, Macau, Malaysien, Mexiko, S�dafrika, Taiwan, Tadschikistan" },
		     { "Weltzone 4", "+" },
		     { "Iridium 8816", "Iridium 008816"},
		     { "Iridium 8817", "Iridium 008817"}};
  
  double Tarif[] = { 01.00, 
		     01.00, 
		     01.00, 
		     02.70, 
		     03.90, 
		     03.30, 
		     03.50, 
		     05.50, 
		     09.70, 
		     15.00, 
		     22.00, 
		    122.00, 
		     62.00 }; 
  
  int z;
  
  printf ("\n");
  rprintf ("max.plus","P:67");
  rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:maintained by:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/30", Tarif[z]);
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
		      { "Internationale Zone 5", "+" }};
  
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
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/30", Tarif[z]);
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

  printf ("V:1.83-Austria [25-Jul-1999]\n\n");
  printf ("U:%%.3f �S\n");

#if 0
  rate_1066();
#else
  rate_1001_old();
  rate_1002();
  rate_1003();
  rate_1005();
  rate_1007();
  rate_1011();
  rate_1012();
  rate_1024();
  rate_1029();
  rate_1044();
  rate_1066();
  rate_1067();
  rate_1069();
#endif
  return(EXIT_SUCCESS);	
}

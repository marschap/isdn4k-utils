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
	rprintf ("# alle übrigen Länder", "A:+");
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
    rprintf ("0-24h", "T:*/*=%.2f(60)/60", Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1001_old(void) {

  char *Name[] = { "Minimumtarif",
		   "Standartarif",
		   "Geschäftstarif 1",
		   "Geschäftstarif 2",
		   "Geschäftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "FreePhone", "122,133,144,+43800" },
		     { "Regionalzone", "+43660" }, 
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Online-Tarif", "+437189,19411,19430,19440" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark,Finnland, Frankreich, Großbritannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rumänien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, Färöer-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische Föderation, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Französisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, Südafrika" },
		     { "Ausland Zone 6", "Japan, Südkorea, Malaysia, Niederländische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "Ägypten, Äquatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Grönland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, Äthiopien, Benin, Costa Rica, Fidschi, Französisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, Türkei" },
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
			 {  4.80,  4.80,  4.80,  4.80 }}; /* Votingline 0717x */

  int t, z;
  char s[BUFSIZ];

  for (t=0; t<4; t++) {
    printf ("\n");
    rprintf ("Telekom Austria", "P:01,%d", t+1);
    rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:Maintainer:");
    rprintf ("Die Regionalzone geht bis zu einer Entfernung von 50 Km, ","C:Zone:");
    rprintf ("Fernzone 1 bis 200 Km, Fernzone 2 darüber.","C:Zone:");
    rprintf ("Variable Taktung, abhängig von Zone und Tageszeit.","C:Special:"); 
    rprintf ("Telekom Austria","C:Name:");
    rprintf ("Postgasse 8, A-1010 Wien","C:Address:");
    rprintf ("http://www.telekom.at","C:Homepage:");
    rprintf ("http://www.telekom.at/tarife/","C:TarifURL:");
    rprintf ("0800 100 100","C:Telefon:");
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
	  rprintf ("Tageszeit", "T:W/08-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("Tageszeit 1",   "T:W/08-12=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Tageszeit 1",   "T:1-4/13-16=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Tageszeit 2",   "T:1-4/12-13,16-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("Tageszeit 2",   "T:5/12-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	}
	if (Faktor[z][2]==Faktor[z][3]) {
	  rprintf ("Sparzeit", "T:W/18-8=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	  rprintf ("Sparzeit", "T:E,H/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
	} else {
	  rprintf ("Sparzeit",      "T:W/06-08,18-20=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][2]);
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
		   "Geschäftstarif 1",
		   "Geschäftstarif 2",
		   "Geschäftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{ "FreePhone", "112,122,133,141,142,144,+43800,+43801,+43802,+43803,+43804" },
		     { "Regionalzone", "111*,11820,15*,+437111,+437112,+437113,+437114,+43810" }, 
		     { "Österreichzone", "Österreich" },
		     { "Mobilfunk 1", "+43663,+43664,+43676" },
		     { "Mobilfunk 2", "+43699" },
		     { "Online", "+437189,19411,19430,19440" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark,Finnland, Frankreich, Großbritannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rumänien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, Färöer-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische Föderation, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Französisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, Südafrika" },
		     { "Ausland Zone 6", "Japan, Südkorea, Malaysia, Niederländische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "Ägypten, Äquatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Grönland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, Äthiopien, Benin, Costa Rica, Fidschi, Französisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, Türkei" },
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
			 {  2.88,  1.00 },  /* Österreichzone */
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

  for (t=0; t<5; t++) {
    printf ("\n");
    sprintf(s,"Telekom Austria %s",Name[t]);
    rprintf (s, "P:01,%d", t+1);
    rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:Maintainer:");
    rprintf ("Die Regionalzone geht bis zu einer Entfernung von 50 Km, alles andere ist Österreichzone.","C:Zone:");
    rprintf ("Variable Taktung, abhängig von Zone und Tageszeit.","C:Special:"); 
    rprintf ("Telekom Austria","C:Name:");
    rprintf ("Postgasse 8, A-1010 Wien","C:Address:");
    rprintf ("http://www.telekom.at","C:Homepage:");
    rprintf ("http://www.telekom.at/tarife/","C:TarifURL:");
    rprintf ("0800 100 100","C:Telefon:");
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
	  rprintf ("0-24h", "T:*/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	} else {
	  rprintf ("Geschäftszeit",   "T:W/08-18=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][0]);
	  rprintf ("Freizeit",        "T:W/18-08=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	  rprintf ("Freizeit",        "T:E,H/*=%.3f/%.5g", Tarif[t], 72.0/Faktor[z][1]);
	}
      }
      print_area(Zone[z][1]);
    }
  }    
}


void rate_1002(void) {

  char *Zone[][2] = {
		     { "Österreich", "Österreich" },
		     { "special friends", "" },
		     { "Mobilnetz", "+43663,+43664,+43676,+43699" },
		     { "EasyInternet", "" },
		     { "Nachbarländer West", "Deutschland, Italien, Liechtenstein, Schweiz" },
		     { "Nachbarländer Ost", "Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 3, USA", "Belgien, Dänemark, Frankreich, Großbritannien, Irland, Amerikanische Jungferninseln, Kanada, Luxemburg, Niederlande, Norwegen, Puerto Rico, Schweden, Großbritannien, Vereinigte Staaten (USA), Alaska, Hawaii" },
		     { "Europa 4", "Andorra, Bosnien-Herzegowina, Finnland, Griechenland, Kanarische Inseln, Kroatien, Madeira, Monaco, Polen, Portugal, San Marino, Spanien, Vatikan" },
		     { "Europa 5", "Albanien, Azoren, Belarus, Bulgarien, Estland, Lettland, Malta, Mazedonien, Rumänien, Türkei, Ukraine, Zypern" },
		     { "Europa 6", "Algerien, Färöer-Inseln, Georgien, Gibraltar, Island, Israel, Litauen, Marokko, Rußland, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Fernost, Südafrika", "Australien, Hongkong, Japan, Kokosinseln, Neuseeland, Singapur, Südafrika, Südkorea, Weihnachtsinseln" },
		     { "Welt 1", "Armenien, Aserbaidschan, Chile, Französisch-Guayana, Guadeloupe, Libyen, Martinique, Reunion" },
		     { "Welt 2", "Angola, Antarktis, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Kuwait, Macao, Malaysia, Mexiko, Saint Pierre und Miquelon, Moldau, Niederländische Antillen, Philippinen, Saipan, Saudi-Arabien, Venezuela" },
		     { "Welt 3", "Ägypten, Äquatorial-Guinea, Aruba, Bahamas, Bahrain, Barbados, Belize, Botswana, Brunei, Burundi, Dominica, Dominikanische Republik, Elfenbeinküste, Gabun, Ghana, Grenada, Grönland, Guinea, Guinea-Bissau, Guyana, Jordanien, Britische Jungferninseln, Kasachstan, Katar, Kirgisistan, Kolumbien, Komoren, Laos, Lesotho, Libanon, Liberia, Malawi, Mayotte, Namibia, Nauru, Nepal, Neukaledonien, Saint Kitts und Nevis, Nigeria, Norfolk-Inseln, Panama, Peru, Sambia, Sao Tome und Principe, Simbabwe, Saint Helena, Saint Kitts und Nevis, Saint Lucia, Saint Vincent und die Grenadinen, Sudan, Swasiland, Syrien, Tansania, Tonga, Trinidad & Tobago, Turks- und Caicos-Inseln, Vereinigte Arabische Emirate, Samoa" },
		     { "Welt 4", "Anguilla, Argentinien, Äthiopien, Benin, Bolivien, Caymaninseln, Costa Rica, Dschibuti, Fidschi, Französisch-Polynesien, Gambia, Indien, Indonesien, Irak, Jamaica, Kenia, Kiribati, Kuba, Malediven, Mauretanien, Mauritius, Mongolei, Mosambik, Myanmar, Nicaragua, Niger, Niue, Nordkorea, Oman, Papua-Neuguinea, Paraguay, Ruanda, Sierra Leone, Sri Lanka, Surinam, Taiwan, Togo, Uganda, Uruguay, Vanuatu, Zentralafrikanische Republik" },
		     { "Welt 5", "Afghanistan, Amerikanisch-Samoa, Antigua & Barbuda, Ascension, Bangladesch, Burkina Faso, Cook-Inseln, Diego Garcia, El Salvador, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Kamerun, Kongo, Madagaskar, Honduras, Jemen, Kambodscha, Kamerun, Kap Verde, Kongo, Madagaskar, Mali, Marshallinseln, Midway-Inseln, Mikronesien, Montserrat, Pakistan, Palau, Salomonen, Senegal, Seychellen, Somalia, Tokelau, Tschad, Tuvalu, Vietnam, Wake-Inseln, Wallis- und Futuna-Inseln, Zaire" }};
  
  double Tarif[][2] = {
		       {  0.88,  0.88 }, /* 1.9. rundum die Uhr */
		       {  0.88,  0.88 }, /* spec. friends ?? 0.73 */
		       {  3.90,  2.88 },
		       {  0.53,  0.18 }, /* internet = TA-Online */
		       {  2.50,  2.30 }, /* Nachb west */
		       {  3.70,  3.50 },
		       {  3.60,  3.40 },
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
  
  printf ("\n");
  rprintf ("UTA", "P:02");
  rprintf ("Leopold Toetsch <lt@toetsch.at>", "C:Maintainer:");
  rprintf ("http://www.utanet.at","C:Homepage:");
  rprintf ("http://www.utanet.at/1/txt/0/12.html","C:TarifURL:");
  rprintf ("UTA Telekom AG","C:Name:");
  rprintf ("Rooseveltplatz 2, A-1090 Wien","C:Address:");
  rprintf ("0800 882 882","C:Telefon:");
  rprintf ("0800 882 329","C:Telefax:");
  rprintf ("Mindestumsatz in 2 Monaten ATS 198.","C:GT:");
  rprintf ("Cost = Ch > 198/2 ? 0 : 198/2","C:GF:");

  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    else {
      rprintf ("Geschäftszeit", "T:W/8-18=%.2f(60)/1", Tarif[z][0]);
      rprintf ("Freizeit", "T:W/18-8=%.2f(60)/1", Tarif[z][1]);
      rprintf ("Freizeit", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
    }  
    print_area(Zone[z][1]);
  }
}    

void rate_1003(void) {

  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "5 Freunde", "" }, 
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Europa spezial", "Belgien, Frankreich, Großbritannien" },
		     { "Ausland Zone 1", "Czech Republic, Hungary, Italy, Slovakia, Slovenia, Switzerland" },
		     { "Ausland Zone 2", "Bosnia and Hercegovina, Canada, Croatia, Denmark, Finland, Luxembourg, Monaco, Netherlands, Norway, Poland, Puerto Rico, Spain, Sweden, United States of America, American Virgin Isles, Vatican City, Montenegro" },
		     { "Ausland Zone 3", "Andorra, Belarus, Bulgaria, Greece, Ireland, Malta, Macedonia, Romania, San Marino, Tunesia" },
		     { "Ausland Zone 4", "Albania, Cyprus, Israel, Portugal, Turkey, Ukraine" },
		     { "Ausland Zone 5", "Algeria, Antarctica, Australia, Weihnachtsinseln, Cocos Island, Estonia, Färöer, Gibraltar, Japan, South Korea, Latvia, Moldova, Marocco, Russian Federation, South Africa, Tajikistan" },
		     { "Ausland Zone 6", "Azerbaijan, Chile, French Guyana, Georgia, Greenland, Guadaloupe, Hongkong, Iceland, Libya, Lithuania, Martinique, Mayotte, Mexico, Phillipines, Reunion, St. Pierre and Miquelon, Singapore, Thailand, Turkmenistan, Uganda, United Arab Emirates" },
		     { "Ausland Zone 7", "Angola, Argentina, Bahrain, Bhutan, Botswana, Brazil, Burundi, Comoros, Dominican Republic, Ecuador, Egypt, Gabon, Gambia, India, Indonesia, Iran, Jordan, Kuwait, Kyrgyzstan, Macau, Malawi, Malaysia, Namibia, Netherlands Antilles, New Zealand, Nigeria, Oman, Saipan, Saudi Arabia, Sri Lanka, Swaziland, Taiwan, Venezuela, Zambia, Zimbabwe" },
		     { "Ausland Zone 8", "Armenia, Aruba, Bahamas, Barbados, Belize, Bermuda, Brunei, Central African Republic, China, Colombia, Costa Rica, Cuba, Djibouti, Equatorial Guinea, Ghana, Guinea-Bissau, Guyana, Kazakhstan, Lesotho, Liberia, Mongolia, Nepal, Panama, Saint Helena, Sao Tome and Principa, Sudan, Syria, Tanzania, Trinidad & Tobago, Turks & Caicos Islands" },
		     { "Ausland Zone 9", "Ascension, Benin, Bolivia, Burkina Faso, Cameroon, Cape Verde, Chad, Elfenbeinküste, El Salvador, Ethiopia, Fiji, French Polynesia, Guatemala, Haiti, Honduras, Iraq, Jamaica, Kenya, Kiribati, Laos, Lebanon, Maldives, Mali, Mauritania, Mauritius, Mozambique, Myanmar, Nauru, New Caledonia, Nicaragua, Niue, Norfolk Island, Pakistan, Papua New Guinea, Peru, Quatar, Rwanda, Saint Lucia, St. Vincent and the Grenadines, Samoa, Senegal, Seychelles Islands, Sierra Leone, Somalia, Suriname, Togo, Tonga, Uzbekistan, Vanuatu, Jemen, Zaire" },
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
  rprintf ("1003 Multikom Austria Telekom","C:Name:");
  rprintf ("","C:Address:");
  rprintf ("http://www.multikom.at","C:Homepage:");
  rprintf ("multikom@multikom.at","C:EMail:");
  rprintf ("Die Regionalzone geht bis zu einer Entfernung von 50 Km, die Fernzone 1 bis ","C:Zone:");
  rprintf ("200 Km alles andere ist Fernzone 2.","C:Zone:");
  rprintf ("Verzonung nicht verifiziert", "# Fixme:");
  rprintf ("# Verzonung", "D:pta");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("0-24h", "T:*/*=%.2f(60)/60/1", Tarif[z][0]);
    else {
      rprintf ("Tag", "T:*/8-18=%.2f(60)/60/1", Tarif[z][0]);
      rprintf ("Nacht", "T:*/18-8=%.2f(60)/60/1", Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1004_1(void) {
  char *Zone[][2] = {
    { "Regional", "" }, 
    { "Austria", "Österreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
  }; 
  double Tarif[][2] = {
   {0.65, 0.65}, /* W/N/B O/ST/K V/T */ /* Fixme: + 20 % ??? */
   {0.90, 0.75},
   {2.50, 2.05}
  }; 
  int z;
  
  printf ("\n");
  rprintf ("Globalone", "P:04,1");
  rprintf ("Globalone","C:Name:");
  rprintf ("Print Eugen-Str. 8-10, A-1040","C:Address:");
  rprintf ("http://www.globalone.at","C:Homepage:");
  rprintf ("http://www.globalone.at/tele_assi_national.html","C:TarifURL:");
  rprintf ("info.at@globalone.at","C:EMail:");
  rprintf ("Regionalzone sind Gespräche innerhalb W/NÖ/B, OÖ/St/K und S/T/V, ","C:Zone:");
  rprintf ("Gespräche zwischen diesen 3 Gebieten sind Fernzone ","C:Zone:");
  rprintf ("# Verzonung", "D:1004");
  rprintf ("Ausland nicht implemtiert", "# Fixme:");
  rprintf ("Ausland nicht implemtiert", "C:Special:");
  rprintf ("Businesskunden ab ATS 5000", "C:GT:");
  rprintf ("Ch >= 5000 ? 1 : -1", "C:GF:");
  
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]*1.2);
    else {
      rprintf ("Geschäftszeit", "T:W/8-18=%.2f(60)/1", Tarif[z][0]*1.2);
      rprintf ("Freizeit", "T:*/*=%.2f(60)/1", Tarif[z][1]*1.2);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1004_2(void) {
  char *Zone[][2] = {
    { "Regional", "" }, 
    { "Austria", "Österreich" },
    { "Mobilfunk", "+43663,+43664,+43676,+43699" },
  }; 
  double Tarif[][2] = {
   {0.55, 0.55}, /* W/N/B O/ST/K V/T */ /* Fixme: + 20 % ??? */
   {0.80, 0.70},
   {2.40, 1.95}
  }; 
  int z;
  
  printf ("\n");
  rprintf ("Globalone Bus.", "P:04,2");
  rprintf ("Globalone","C:Name:");
  rprintf ("","C:Address:");
  rprintf ("http://www.globalone.at","C:Homepage:");
  rprintf ("http://www.globalone.at/tele_assi_national.html","C:TarifURL:");
  rprintf ("Regionalzone sind Gespräche innerhalb Wien/NÖ/B, OÖ/St/K und S/T/V, ","C:Zone:");
  rprintf ("Gespräche zwischen diesen 3 Gebieten sind Fernzone ","C:Zone:");
  rprintf ("# Verzonung", "D:1004");
  rprintf ("Ausland nicht implemtiert", "# Fixme:");
  rprintf ("Ausland nicht implemtiert", "C:Special:");
  rprintf ("Wien Geschäftszeit 0.68+Mwst.", "C:Special:");
  rprintf ("Businesskunden ab ATS 5000", "C:GT:");
  rprintf ("Ch >= 5000 ? 1 : -1", "C:GF:");
  
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1])
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]*1.2);
    else {
      rprintf ("Geschäftszeit", "T:W/8-18=%.2f(60)/1", Tarif[z][0]*1.2);
      rprintf ("Freizeit", "T:*/*=%.2f(60)/1", Tarif[z][1]*1.2);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1005(void) {

  char *Zone[][2] = {{ "National", "Österreich" }, 
		     { "Mobilfunknetz", "+43663,+43664,+43676,+43699" },
		     { "D-I-CH","Deutschland, Italien, Schweiz" },
		     { "Europa 1 / USA", "Belgien, Dänemark, Finnland, Frankreich, Griechenland, Grossbritannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Nordirland, Norwegen, Schweden, Schweiz, Slowakei, Slowenien, Spanien,  Tschechien, Ungarn, USA, Hawaii, Kanada" },
		     { "Europa 2", "Albanien, Algerien, Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Färöer-Inseln, Gibraltar, Island, Israel, Jugoslawien, Kroatien, Lettland, Litauen, Mazedonien, Malta, Marokko, Polen, Portugal, Rumänien, Russland, San Marino, Tunesien, Türkei, Zypern" },
		     { "Welt 1", "Australien, Hongkong, Japan, Neuseeland, Südkorea, Singapur" },
		     { "Welt 2", "+" }};
  
  double Tarif[] = { 0.90, 3.90, 2.50, 3.50, 6.00, 9.00, 19.00 };
  
  int z;
  

  printf ("\n");
  rprintf ("tele2", "P:05");
  rprintf ("Tele2 Telecommunications Services GmbH","C:Name:");
/*  rprintf ("","C:Address:"); */
  rprintf ("http://www.tele2.at","C:Homepage:");
  rprintf ("http://www.tele2.at/ger/tarif.htm","C:TarifURL:");
  rprintf ("0800 24 00 24","C:Telefon:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/60/1", Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_1007(void) { /* Fairtel lt 24.08.99 */
  char *Zone[][2] = {{ "Nahzone", "" },
		     { "Fernzone", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "EU+Angrenzende","Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn,"
		       "Belgien, Dänemark, Finnland, Griechenland, Großbritannien, Irland, Luxemburg, Niederlande, Norwegen, Portugal, Schweden, Spanien"
		      },
		      {"Welt","+"}
		      };
  double Tarif[] = { 0.82, 0.88, 3.60, 2.50, 21.12 }; /* Fixme: Auslandstarife*/
  int z;      

  printf ("\n"); 
  rprintf ("European Telecom Fairtel", "P:07");
  rprintf ("European Telecom","C:Name:");
  rprintf ("Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der ","C:Zone:");
  rprintf ("Nahzone sonst in der Fernzone.","C:Zone:");
  rprintf ("Florag. 7, A-1040 Wien","C:Address:"); 
  rprintf ("http://www.europeantelecom.at","C:Homepage:");
  rprintf ("info@europeantelecom.at","C:EMail:");
  rprintf ("0800 1007 1007","C:Telefon:"); 
    rprintf ("# Verzonung", "D:1012");
    rprintf ("Fairtel", "# Tarif:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z], Tarif[z]);
    print_area(Zone[z][1]);
  }
}    
  
void rate_1007_old(void) { /* lt 24.08.99 */

  char *Name[] = { "1 Jahr feste Bindung",
		   "14-tägig kündbar" };

  char *Zone[][2] = {{ "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland Zone 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Ausland Zone 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark,Finnland, Frankreich, Großbritannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rumänien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Ausland Zone 3", "Algerien, Estland, Färöer-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische Föderation, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Ausland Zone 4", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Ausland Zone 5", "Chile, Französisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, Südafrika" },
		     { "Ausland Zone 6", "Japan, Südkorea, Malaysia, Niederländische Antillen, Philippinen, Saipan" },
		     { "Ausland Zone 7", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Ausland Zone 8", "Ägypten, Äquatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Grönland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Ausland Zone 9", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Ausland Zone 10", "Argentinien, Äthiopien, Benin, Costa Rica, Fidschi, Französisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Ausland Zone 11", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Ausland Zone 12", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Ausland Zone 13", "Cook-Inseln, Madagaskar" },
		     { "Ausland Zone 14", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Ausland Zone 15", "Griechenland, Polen, Tunesien, Türkei" },
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
  rprintf ("European Telecom","C:Name:");
/*  rprintf ("","C:Address:"); */
  rprintf ("http://www.europeantelecom.at","C:Homepage:");
  rprintf ("0800 1007 1007","C:Telefon:"); 
  rprintf ("Die Fernzone 1 reicht bis 200 Km alles andere ist Fernzone 2.","C:Zone:");
  rprintf ("12-monatige Bindungsdauer.","C:Special");
    rprintf ("# Verzonung", "D:1007");
    rprintf (Name[t], "# Tarif:");
    for (z=0; z<COUNT(Zone); z++) {
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (Tarif[t][z][0]==Tarif[t][z][1]) {
	rprintf ("Tageszeit", "T:W/08-18=%.3f(60)/1", Tarif[t][z][0]);
      } else {
	rprintf ("Tageszeit 1",   "T:W/08-12=%.3f(60)/1", Tarif[t][z][0]);
	rprintf ("Tageszeit 1",   "T:1-4/13-16=%.3f(60)/1", Tarif[t][z][0]);
	rprintf ("Tageszeit 2",   "T:1-4/12-13,16-18=%.3f(60)/1", Tarif[t][z][1]);
	rprintf ("Tageszeit 2",   "T:5/12-18=%.3f(60)/1", Tarif[t][z][1]);
      }
      if (Tarif[t][z][2]==Tarif[t][z][3]) {
	rprintf ("Sparzeit", "T:W/18-8=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Sparzeit", "T:E,H/*=%.3f(60)/1", Tarif[t][z][2]);
      } else {
	rprintf ("Sparzeit",      "T:W/06-08,18-20=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Sparzeit",      "T:E,H/06-20=%.3f(60)/1", Tarif[t][z][2]);
	rprintf ("Supersparzeit", "T:*/20-06=%.3f(60)/1", Tarif[t][z][3]);
      }
      print_area(Zone[z][1]);
    }
  }
}


void rate_1008(void) {
  
  char *Zone[][2] = { {"Nah", ""},
		     { "Österreich", "Österreich" },
		     { "Österreich", "Österreich" },
		     { "Mobil A1,max","+43664, +43676"},
		     { "Mobil D,One", "+43663, +43699"}
		    };
		     
  double Tarif[][2] = {{ 0.98, 0.98 },
		       { 2.48, 1.20 },
		       { 2.48, 1.20 },
		       { 4.20, 3.30 }, /* a1,max */
		       { 4.68, 3.30 } /* one, D */
		      };
  int z;
  		       
  printf ("\n");
  rprintf ("TELEforum", "P:08");
  rprintf ("# Verzonung", "D:pta");
  printf ("# Verzonung nicht verifiziert\n");
  rprintf ("Freischaltgebühren ATS 480","C:GT:");
  rprintf ("Cost = 480/12","C:GF:");
  rprintf ("TELEforum Telekommunikations GmbH", "C:Name:");
  rprintf ("Sporg. 32, A-8010 Graz","C:Address:");
  rprintf ("http://www.teleforum.at", "C:Homepage:");
  rprintf ("office@teleforum.at", "C:EMail:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      rprintf ("Tag", "T:W/8-18=%.2f(60)/1", Tarif[z][0]);
      rprintf ("Nacht", "T:W/18-8=%.2f(60)/1", Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}  

void rate_1009(void) {

  char *Zone[][2] = {
                     { "Regional",""},
		     { "Österreich", "Österreich" },
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
/* 2.86 */	{"Deutschland", "Deutschland"},
/* 3.46 */	{"USA", "USA"},
/* 3.56 */	{"Frankreich ...", "Frankreich, Großbritannien, Italien, Liechtenstein, Schweden, Schweiz"},
/* 3.76 */	{"Belgien", "Belgien"},
/* 3.86 */	{"Niederlande", "Niederlande"},
/* 3.96 */	{"Dänemark ...", "Dänemark, Finnland, Kanada, Spanien, Ungarn"},
/* 4.46 */	{"Irland ...", "Irland, Norwegen, Slowenien, Tschechische Republik"},
/* 4.66 */	{"Monaco", "Monaco"},
/* 4.76 */	{"Andorra ...", "Andorra, Bosnien-Herzegowina, Vatikan, Kroatien, Luxemburg, Puerto Rico, Amerikanische Jungferninseln"},
/* 5.16 */	{"Malta ...", "Malta, San Marino, Slowakische Republik"},
/* 5.36 */	{"Griechenland ...", "Griechenland, Polen"},
/* 5.56 */	{"Jugoslawien", "Jugoslawien"},
/* 5.86 */	{"Portugal", "Portugal"},
/* 5.96 */	{"Albanien ...", "Albanien, Australien, Weissrussland, Bulgarien, Estland, Israel, Mazedonien, Zypern"},
/* 6.36 */	{"Rumänien ...", "Rumänien, Türkei, Ukraine"},
/* 6.46 */	{"Färöer-Inseln ...", "Färöer-Inseln, Island"},
/* 6.76 */	{"Tunesien", "Tunesien"},
/* 6.96 */	{"Gibraltar ...", "Gibraltar, Lettland, Russische Förderation (östl.)"},
/* 7.16 */	{"Antarktis ...", "Antarktis, Weihnachtsinseln, Kokosinseln, Hongkong, Japan, Litauen, Neuseeland, Singapur"},
/* 7.46 */	{"Algerien", "Algerien"},
/* 7.76 */	{"Südkorea", "Südkorea"},
/* 7.96 */	{"Marokko", "Marokko"},
/* 8.36 */	{"Malaysia ...", "Malaysia, Mexiko, Philippinen, St. Pierre und Miquelon, Südafrika, Tadschikistan"},
/* 8.96 */	{"Chile ...", "Chile, Georgien, Guadeloupe, Libyen, Moldavien Moldau (Republik), Marianen (SaipanNord-), Turkmenistan, Usbekistan"},
/* 9.26 */	{"Aserbaidschan", "Aserbaidschan"},
/* 9.56 */	{"Macao ...", "Macao, Martinique / Franz. Antillen, Mayotte, Niederl. Antillen"},
/* 9.96 */	{"Armenien ...", "Armenien, Französisch-Guayana"},
/* 10.16 */	{"Amerikanisch-Samoa", "Amerikanisch-Samoa"},
/* 10.76 */	{"Bermuda ...", "Bermuda, Dominikanische Republik, Reunion"},
/* 11.36 */	{"China", "China"},
/* 11.96 */	{"Angola ...", "Angola, Bahamas, Brasilien, Ecuador, Kolumbien, Panama, Taiwan, Thailand, Venezuela"},
/* 13.16 */	{"Ägypten ...", "Ägypten, Brunei, Grönland, St. Vincent und Grenadinen, Trinidad und Tobago, Ver. Arabische Emirate"},
/* 13.96 */	{"Barbados ...", "Barbados, Botsuana Botswana, Ghana, Jordanien"},
/* 14.36 */	{"Aruba ...", "Aruba, Indien, Indonesien, Iran, Kirgistan, Kuwait, Libanon, Liberia, Peru, Katar, Sudan, Syrien, Tansania"},
/* 14.96 */	{"Bhutan ...", "Bhutan, Gabun, Saudi Arabien, Senegal"},
/* 15.56 */	{"Argentinien ...", "Argentinien, Costa Rica, Guinea, Guinea-Bissau, Kasachstan, Nordkorea, Kuba, Lesotho, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Norfolkinseln, Papua-Neuguinea, Paraguay, Sambia, West-Samoa, Simbabwe, Swasiland, Turks- und Caicosinseln"},
/* 15.96 */	{"Bahrain ...", "Bahrain, Laos, Nigeria, St. Helena, St. Lucia"},
/* 16.76 */	{"Ascension ...", "Ascension, Belize, Bolivien, Burundi, Dominica, El Salvador, Fidschi, Grenada, Guatemala, Guyana, Jamaika, Kenia, Malediven, Mauretanien, Mauritius, Montserrat, Nicaragua, Somalia, Uruguay, Vietnam"},
/* 16.96 */	{"Mayotte ...", "Mayotte, Sao Tome und Principe"},
/* 17.96 */	{"Äquatorial-Guinea ...", "Äquatorial-Guinea, Äthiopien, Afghanistan, Anguilla, Antigua und Barbuda, Bangladesch, Benin, Burkina Faso Obervolta, Kaimaninseln, "
    "Cookinseln, Elfenbeinküste Cote de Ivoire, Dschibuti, Eritrea, Falklandinseln, Französisch-Polynesien, Gambia, Guam, Guantanamo Bay, Haiti, Honduras, Irak, Jemen (Arab. Republik), "
    "Kambodscha, Kamerun, Cape Verde, Kiribati, Kongo, Madagaskar, Mali, Marshallinseln, Mikronesien, Mongolei, Mosambik, Myanmar Burma, Niger, Niue-Inseln, Oman, Pakistan, Palau /Belau, "
    "Ruanda, Salomonen, Seyschellen, Sierra Leone, Sri Lanka, St. Kitts und Nevis, Suriname, Togo, Tonga, Tschad, Tuvalu, Uganda, Vanuatu, Britische Jungferninseln, Wake Inseln, Zaire, Zentralafrikanische Republik"},
		     
        }; 
		    
  double Tarif[] = { 
    9.99,
    1.46,
    1.46,
    3.98, /* Mobil */
2.86,  /* Deutschland ... */
3.46,  /* USA ... */
3.56,  /* Frankreich ... */
3.76,  /* Belgien ... */
3.86,  /* Niederlande ... */
3.96,  /* Dänemark ... */
4.46,  /* Irland ... */
4.66,  /* Monaco ... */
4.76,  /* Andorra ... */
5.16,  /* Malta ... */
5.36,  /* Griechenland ... */
5.56,  /* Serbien ... */
5.86,  /* Portugal ... */
5.96,  /* Albanien ... */
6.36,  /* Rumänien ... */
6.46,  /* Faroer Inseln ... */
6.76,  /* Tunesien ... */
6.96,  /* Gibraltar ... */
7.16,  /* Antarctica Terr. Davis, Maws ... */
7.46,  /* Algerien ... */
7.76,  /* Korea Süd ... */
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
13.16,  /* Ägypten ... */
13.96,  /* Barbados ... */
14.36,  /* Aruba ... */
14.96,  /* Bhutan ... */
15.56,  /* Argentinien ... */
15.96,  /* Bahrain ... */
16.76,  /* Ascension Island ... */
16.96,  /* Komoren ... */
17.96  /* Äquatorialguinea ... */
			
		    };	 
  int z;
  
  printf ("\n");
  rprintf ("Vocalis", "P:09");
  rprintf ("Vocalis Telekom-Dienste GmbH","C:Name:");
  rprintf ("34 g Gesprächs-Herstellungsgebühr","C:Special:"); /* Fixme: oder Mindestgeb. ? */
  rprintf ("http://www.vocalis.at", "C:Homepage:");
  rprintf ("http://www.vocalis.at/voc_tarife_frame.htm", "C:TarifURL:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=0.34/0,%.2f(60)/1", Tarif[z]);
    print_area(Zone[z][1]);
  }
}  

void rate_1011(void) {
  
  char *Zone[][2] = {{ "Bundesland", "" }, 
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     {"Deutschland", "Deutschland"},
		     {"Ausland 1", "Frankreich, Großbritannien, Italien, Liechtenstein, Paraguay, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn, Vatikanstadt"},
		     {"Ausland 2", "Belgien, Dänemark, Finnland, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Papua-Neuguinea, San Marino, Schweden, Spanien, Vereinigte Staaten (USA)"},
		     {"Ausland 3", "Andorra, Bosnien-Herzegowina, Bulgarien, Kroatien, Malediven, Zypern"},
		     {"Ausland 4", "Weißrußland, Färöer-Inseln, Griechenland, Jugoslawien, Serbien, Montenegro, Britische Jungferninseln, Kanarische Inseln, Palau, Rumänien, Tunesien, Türkei, Ukraine"},
		     {"Ausland 5", "Albanien, Algerien, Antarktis, Australien, Estland, Lettland, Malawi, Malaysia, Marokko, Moldau, Moldau"},
		     {"Ausland 6", "Gibraltar, Island, Israel"},
		     {"Ausland 7", "Litauen, Russische Föderation, Tadschikistan"},
		     {"Ausland 8", "Georgien, Hongkong, Japan, Südkorea, Neuseeland, Singapur, Turkmenistan, Usbekistan"},
		     {"Ausland 9", "Armenien, Aserbaidschan, Weihnachtsinseln, Kokosinseln, Mauritius, Mexiko"},
		     {"Ausland 10", "Chile, Französisch-Guayana, Guadeloupe, Libyen, Peru, Reunion, St. Pierre und Miquelon, Südafrika"},
		     {"Ausland 11", "Brasilien, Niederländische Antillen"},
		     {"Ausland 12", "Angola, Bermuda, Bhutan, China, Ecuador, Iran, Kuwait, Mali, Saudi-Arabien, Venezuela, Vereinigte Arabische Emirate"},
		     {"Ausland 13", "Bahamas, Dominikanische Republik, Madagaskar, Ägypten"},
		     {"Ausland 14", "Aruba, Barbados, Belize, Botswana, Brunei, Burundi, El Salvador, Gabun, Ghana, Grönland, Guinea-Bissau, Guyana, Jordanien, Katar, Kirgisistan, Kolumbien, Libanon, Martinique, Mayotte, Sao Tome und Principe, Saint Vincent und die Grenadinen, Taiwan, Trinidad und Tobago, Äquatorial-Guinea"},
		     {"Ausland 15", "Argentinien, Bahrain, Benin, Cote d'Ivoire, Dominica, Grenada, Guinea, Kasachstan, Kenia, Komoren, Laos, Lesotho, Liberia, Mauretanien, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Sambia, Samoa, Simbabwe, St. Helena, Saint Kitts und Nevis, St. Lucia, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln"},
		     {"Ausland 16", "Mikronesien, Mikronesien, Äthiopien"},
		     {"Ausland 17", "Costa Rica, Fidschi, Französisch-Polynesien, Gambia, Indonesien, Irak, Jamaika, Kiribati, Nicaragua, Niue, Oman, Panama, Philippinen, Ruanda, Amerikanisch-Samoa, Suriname, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik"},
		     {"Ausland 18", "Afghanistan, Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Bolivien, Burkina Faso, Cook-Inseln, Caymaninseln, Dschibuti, Eritrea, Falklandinseln, Guam, Guantanamo, Guatemala, Haiti, Honduras, Indien, Jemen, Kambodscha, Kamerun, Kap Verde, Kongo, Nordkorea, Kuba, Macao, Malta, Marshallinseln, Mazedonien, Mazedonien, Niger, Pakistan, Salomonen, Senegal, Seychellen, Sierra Leone, Somalia, Sri Lanka, Tschad, Tuvalu, Uruguay, Vietnam, Wallis- und Futuna-Inseln, Zaire"}};
  
  double Tarif[][2] = {{ 1.00, 0.88 },
		       { 1.50, 0.88 },
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
  rprintf ("RSL COM Austria AG","C:Name:");
  rprintf ("Marktstr. 3, A-7000 Eisenstadt","C:Address:"); 
  rprintf ("http://www.rslcom.at","C:Homepage:");
  rprintf ("info@rslcom.at","C:EMail:");
  rprintf ("0800 008000","C:Telefon:");
  rprintf ("Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der ","C:Zone:");
  rprintf ("Bundelandzone sonst in der Zone Österreich.","C:Zone:");
  rprintf ("Grundgebühr ATS 69, bei Rechnung < 200 ATS.","C:GT:");
  rprintf ("Cost = Ch >= 200 ? 0 : 69","C:GF:");

  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      rprintf ("Tag", "T:W/8-18=%.2f(60)/1", Tarif[z][0]);
      rprintf ("Nacht", "T:W/18-8=%.2f(60)/1", Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1012(void) { /* Fixme: ab wann ( 1.9.99 ?? ) */

  char *Zone[][2] = {
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Nachbarn", "Frankreich, Italien, Schweiz, Liechtenstein, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 1 + USA", "Belgien, Dänemark, Finnland, Großbritannien, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Schweden, Spanien, Vereinigte Staaten (USA)" },
		     { "Europa 2", "Andorra, Bosnien-Herzegowina, Färöer-Inseln, Jugoslawien, Serbien, Montenegro, Kroatien, Malta, Monaco" },
		     { "Europa 3", "Albanien, Bulgarien, Estland, Griechenland, Mazedonien, Polen, Portugal, Rumänien, San Marino, Tunesien, Ukraine,Weißrußland, Zypern" },
		     { "Welt 1", "Algerien, Australien, Gibraltar, Hongkong, Island, Israel, Japan, Lettland, Litauen, Marokko, Neuseeland, Russische Föderation, Singapur, Südkorea, Türkei" },
		     { "Welt 2", "+" }};
  
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
  
  printf ("\n");
  rprintf ("1012", "P:12");
  rprintf ("1012 privat","C:Name:");
  rprintf ("Linzer Str. 221, A-1140 Wien","C:Address:"); 
  rprintf ("http://www.1012privat.at","C:Homepage:");
  rprintf ("0800 1012 1012","C:Telefon:"); 
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/60/1", Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_1012_old(void) {

  char *Zone[][2] = {{ "Nah", "" }, 
		     { "Fern", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Nachbarn", "Frankreich, Italien, Schweiz, Liechtenstein, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 1 + USA", "Belgien, Dänemark, Finnland, Großbritannien, Irland, Kanada, Luxemburg, Niederlande, Norwegen, Schweden, Spanien, Vereinigte Staaten (USA)" },
		     { "Europa 2", "Andorra, Bosnien-Herzegowina, Färöer-Inseln, Jugoslawien, Serbien, Montenegro, Kroatien, Malta, Monaco" },
		     { "Europa 3", "Albanien, Bulgarien, Estland, Griechenland, Mazedonien, Polen, Portugal, Rumänien, San Marino, Tunesien, Ukraine,Weißrußland, Zypern" },
		     { "Welt 1", "Algerien, Australien, Gibraltar, Hongkong, Island, Israel, Japan, Lettland, Litauen, Marokko, Neuseeland, Russische Föderation, Singapur, Südkorea, Türkei" },
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
  rprintf ("1012 privat","C:Name:");
  rprintf ("Linzer Str. 221, A-1140 Wien","C:Address:"); 
  rprintf ("http://www.1012privat.at","C:Homepage:");
  rprintf ("0800 1012 1012","C:Telefon:"); 
  rprintf ("Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der ","C:Zone:");
  rprintf ("Nahzone sonst in der Fernzone.","C:Zone:");
/*  rprintf ("","C:Special"); */

  rprintf ("# Verzonung", "D:1012");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (z<3) {
      rprintf ("Tag", "T:W/8-18=%.2f(60)/60/1", Tarif[z][0]);
      rprintf ("Nacht", "T:W/18-8=%.2f(60)/60/1", Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/60/1", Tarif[z][1]);
    } else {
      rprintf ("0-24h", "T:*/*=%.2f(60)/60/1", Tarif[z][0]);
    }
    print_area(Zone[z][1]);
  }
}    

void rate_1013(void) {
  printf ("\n");
  rprintf ("NETnet", "P:13");
  rprintf ("NETnet Telekommunikation GmbH","C:Name:");
  rprintf ("Nur Businesskunden","C:Special:");
  rprintf ("Inkustr. 2-7, A-3400 Klosterneuburg", "C:Address:");
  rprintf ("http://www.netnet.at", "C:Homepage:");
  rprintf ("customercare@netnet.co.at","C:EMail:");
}  


void rate_1024(void) {

  char *Zone[][2] = {{ "RundUm-Wien", "" }, 
		     { "RundUm-Regional", "" },
		     { "Ganzland", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "WeltWeit 1", "Deutschland, Schweiz" },
		     { "WeltWeit 2", "Belgien, Dänemark, Finnland, Frankreich, Großbritannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Norwegen, Schweden, Slowenien, Slowakei, Spanien, Tschechien, Ungarn, Vatikan, Vereinigte Staaten (USA), Puerto Rico, Alaska, Hawaii, Kanada" },
		     { "WeltWeit 3", "Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Griechenland, Island, Kroatien, Lettland, Litauen, Malta, Mazedonien, Polen, Portugal, Rumänien, Serbien, Montenegro, Zypern, Australien, Färöer, Israel, Neuseeland, Amerikanische Jungferninseln" },
		     { "WeltWeit 4", "Albanien, Weihnachtsinseln, Gibraltar, Kokosinseln, Rußland, San Marino, Türkei, Tunesien, Ukraine, Weißrußland, Algerien, Hongkong, Japan, Südkorea, Marokko, Philippinen, Russische Föderation, Singapur" },
		     { "WeltWeit 5", "Ägypten, Südafrika, Aserbaidschan, Armenien, China, Georgien, Libyen, Malaysia, Moldau, Tadschikistan, Taiwan, Turkmenistan, Usbekistan, Argentinien, Brasilien, Chile, Costa Rica, Dominikanische Republik, Ecuador, El Salvador, Guatemala, Kolumbien, Mexiko, Nicaragua, Panama, Peru, Venezuela" },
		     { "WeltWeit 6", "Angola, Aruba, Bahamas, Bahrain, Barbados, Bermuda, Bhutan, Bolivien, Botswana, Brunei, Burundi, Ghana, Grönland, Guadeloupe, Guam, Guinea, Martinique, Französisch-Guayana, Honduras, Indien, Indonesien, Iran, Jordanien, Kasachstan, Kirgisistan, Kuba, Kuwait, Lesotho, Libanon, Liberia, Macao, Malawi, Martinique, Mayotte, Namibia, Niederländische Antillen, Nigeria, Paraguay, Reunion, Saipan, Saudi-Arabien, St. Lucia, St. Pierre und Miquelon, Saint Vincent und die Grenadinen, Sudan, Syrien, Swasiland, Tansania, Thailand, Trinidad und Tobago, Uruguay, Vereinigte Arabische Emirate" },
		     { "WeltWeit 7", "+" }};
  
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
  
  printf ("\n");
  rprintf ("Telepassport", "P:24");
  rprintf ("Leopold Toetsch <lt@toetsch.at>", "C:Maintainer:");
  rprintf ("TelePassport Telekommunikationsdiensleitungen GmbH", "C:Name:");
  rprintf ("Prinz-Eugen-Str. 10, A 1040 Wien", "C:Address:");
  rprintf ("http://www.telepassport.at", "C:Homepage:");
  rprintf ("0800 80 1024", "C:Hotline:");
  rprintf ("info@telepassport.at","C:EMail:");
  rprintf ("01 506 44-100","C:Telefon:");
  rprintf ("01 506 44-199","C:Telefax:");
  rprintf ("Vorwahlen 1XXX und 2XXX, sind in der Zone RundUm-Wien, andere Vorwahlen "
    "mit gleicher Anfangsziffer liegen in der Zone RundUm-Regional, der Rest ist "
    "Ganzland.","C:Zone");
  rprintf ("# Verzonung", "D:1024");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    } else {
      rprintf ("Tag", "T:W/8-18=%.2f(60)/1", Tarif[z][0], Tarif[z][0]);
      rprintf ("Nacht", "T:W/18-8=%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
      rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1], Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1029_privat(void) {

  char *Name =  "easy.Talk privat" ;
  
  double Einheit = 67;
  
  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk A1, max", "+43663,+43664,+43676" },
		     { "Mobilfunk One", "+43699" },
		     { "Deutschland", "Deutschland, Liechtenstein, Schweiz, Italien" },
		     { "International 1", "Frankreich, Grossbritannien, Slowakei, Slowenien, Tschechische Republik, Ungarn" },
	{ "International 2", "Andorra, Belgien, Bosnien-Herzegowina, Dänemark, Finnland, Irland, Britische Jungferninseln, Kanada, Kroatien, Luxemburg, Malta, Monaco, Niederlande, Norwegen, Polen, Schweden, Spanien, USA, Vatikan, Zypern" },
	{ "International 3-1","Albanien,Belarus,Bulgarien,Mazedonien,Rumänien,San Marino,Serbien,Ukraine"},
	{ "International 3-2","Estland, Färöer Inseln, Gibraltar, Griechenland, Island, Israel,Portugal, Puerto Rico, Russland, Tunesien, Türkei"},
	{ "International 4","Angola, Bermuda, Brasilien, Chile, Franz. Guyana, Guadeloupe, Hong Kong, Japan, Libyen, Macao, Madeira, Malaysia, Martinique, Mayotte, Mexico, Moldawien, Neu Seeland, Niederl. Antillen, Philippinen, Reunion, Saipan, Singapur, St. Pierre und Miquelon, Südafrika, Südkorea, Venezuela"},
	{ "International 5","Ägypten, Aruba, Bahamas, Barbados, Belize, Bhutan, Botswana, Brunei, Burundi, China, Dominikanische Republik, Ecuador, Gabun, Ghana, Grönland, Guyana, Iran, Jordanien, Kirgisistan, Kolumbien, Kuweit, Libanon, Panama, Saudiarabien, St. Vincent und Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate"},
	{ "International 6","Argentinien, Äthiopien, Bahrain, Benin, Costa Rica, Elfenbeinküste, Französisch-Polynesien, Gambia, Guinea, Guinea-Bissau, Jamaica, Kasachstan, Katar, Kenia, Kiribati, Komoren, Laos, Lesotho, Liberia, Malawi, Malediven, Mauretanien, Mozambique, Namibia, Nauru, Nepal, Neu Kaledonien, Nicaragua, Nigeria, Niue, Norfolk Inseln, Oman, Peru, Ruanda, Sambia, Samoa West, Simbabwe, St. Helena, St. Lucia, Sao Tome und Principe, Sudan, Suriname, Swasiland, Syrien, Taiwan, Tansania, Thailand, Togo, Tonga, Turks & Caicos Islands, Uganda, Vanuatu, Zentralafrikanische Republik"},
	{ "International 7","Anguilla, Antigua & Barbuda, Ascension, Bangla Desh, Bolivien, Burkina Faso, Myanmar/Burma, Cayman Inseln, Dominica, Dschibuti, El Salvador, Eritrea, Falkland Inseln, Grenada, Guam, Guatemala, Haiti, Honduras, Indien, Indonesien, Irak, Jemen, Jungfern Inseln britisch, Kambodscha, Kamerun, Kap Verde, Kongo, Kuba, Mali, Mauritius, Mongolei, Montserrat, Nevis Islands, Niger, Nordkorea, Pakistan, Papua Neuguinea, Paraguay, Salomonen, Senegal, Seychellen, Sierra Leone, Somalia, Sri Lanka, St. Kitts & Nevis, Tschad, Uruguay, Vietnam, Zaire"},
	{ "International 8","Algerien, Georgien, Lettland, Litauen, Marokko, Tadschikistan, Turkmenistan, Usbekistan"}, 
	{ "International 9","Armenien, Aserbaidschan, Christmas Island, Kokos Inseln"},

		     { "International 10", "+" }}; /* Afghanistan, Cook Inseln, Diego Garcia, Guantanamo, Madagaskar, Marshall Inseln, Micronesia, Midway Inseln, Palau, Samoa Ost, Tuvalu, Wake, Wallis/Futuna */

  
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

    printf ("\n");
    rprintf ("CyberTron easy.Talk privat", "P:29,1");

  rprintf ("Cybertron EDV- NetzwerkbetriebsgmbH","C:Name:");
  rprintf ("Davidstr. 79, A-1100 Wien","C:Address:");
  rprintf ("http://www.cybertron.at","C:Homepage:");
  rprintf ("http://www.cybertron.at/html/easy.prod_1.html","C:TarifURL:");
  rprintf ("0800 400 400, 1029 1029","C:Telefon:");
  rprintf ("Die Regionalzone geht bis zu einer Entfernung von 50 Km, die Fernzone 1 bis ","C:Zone:");
  rprintf ("200 Km alles andere ist Fernzone 2.","C:Zone:");
  rprintf ("Variable Taktung.","C:Special:");

    rprintf ("# Verzonung", "D:pta");
    sprintf (s, "%s (ATS %.3f pro Einheit)", Name, Einheit);
    rprintf (s, "# Tarif:");

    for (z=0; z<COUNT(Zone); z++) {
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (Tarif[z][0]!=0.0) { /* Tarif bekannt? */
	if (Tarif[z][0]==Tarif[z][1] && Tarif[z][0]==Tarif[z][2] && Tarif[z][0]==Tarif[z][3]) {
	  rprintf ("0-24h", "T:*/*=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	} else {
	  if (Tarif[z][0]==Tarif[z][1]) {
	    rprintf ("Tageszeit", "T:W/8-18=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	  } else {
	    rprintf ("Tageszeit 1", "T:W/8-12=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	    rprintf ("Tageszeit 1", "T:1-4/13-16=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	    rprintf ("Tageszeit 2", "T:1-4/12-13,16-18=%.2f(60)/%.0f", Tarif[z][1], Einheit/Tarif[z][1]);
	    rprintf ("Tageszeit 2", "T:5/12-18=%.2f(60)/%.0f", Tarif[z][1], Einheit/Tarif[z][1]);
	  }
	  if (Tarif[z][2]==Tarif[z][3]) {
	    rprintf ("Sparzeit", "T:*/*=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	  } else {
	    rprintf ("Sparzeit",      "T:W/06-08,18-20=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	    rprintf ("Sparzeit",      "T:E,H/06-20=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	    rprintf ("Supersparzeit", "T:*/20-06=%.2f(60)/%.0f", Tarif[z][3], Einheit/Tarif[z][3]);
	  }
	}
      }
      print_area(Zone[z][1]);
    }
}

void rate_1029_business(void) {

  char *Name =  "easy.Talk business" ;
  
  double Einheit = 74;
  
  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Fernzone", "Österreich" },
		     { "Mobilfunk A1, max", "+43663,+43664,+43676" },
		     { "Mobilfunk One", "+43699" },
/* 3.24 */	{"Deutschland", "Deutschland"},
/* 3.48 */	{"Liechtenstein ...", "Liechtenstein, Schweiz"},
/* 3.60 */	{"Italien ...", "Italien, Ungarn"},
/* 3.96 */	{"Frankreich ...", "Frankreich, Großbritannien, Großbritannien Mobilfunk, Kanada, Slowakische Republik, Slowenien, Tschechische Republik"},
/* 4.20 */	{"Spanien ...", "Spanien, USA"},
/* 4.32 */	{"Belgien ...", "Belgien, Niederlande, Norwegen, Schweden"},
/* 4.68 */	{"Dänemark ...", "Dänemark, Finnland, Irland, Luxemburg, Malta"},
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
/* 6.18 */	{"Albanien ...", "Albanien, Weissrussland, Rumänien, San Marino, Tunesien, Türkei, Ukraine"},
/* 6.24 */	{"Israel", "Israel"},
/* 6.30 */	{"Gibraltar ...", "Gibraltar, Portugal"},
/* 6.36 */	{"Australien ...", "Australien, Estland, Färöer-Inseln, Lettland, Litauen"},
/* 6.38 */	{"Russische Förderation (östl.) ...", "Russische Förderation (östl.), Moskau, St. Petersburg"},
/* 6.42 */	{"Island", "Island"},
/* 7.56 */	{"Hongkong ...", "Hongkong, Japan, Japan Mobilfunk, Tokyo"},
/* 7.80 */	{"Algerien ...", "Algerien, Marokko"},
/* 7.98 */	{"Madeira", "Madeira"},
/* 8.04 */	{"Weihnachtsinseln ...", "Weihnachtsinseln, Cookinseln"},
/* 8.16 */	{"Singapur", "Singapur"},
/* 8.52 */	{"Georgien ...", "Georgien, Tadschikistan, Turkmenistan, Usbekistan"},
/* 9.48 */	{"Südafrika", "Südafrika"},
/* 9.54 */	{"Libyen ...", "Libyen, Neuseeland"},
/* 9.66 */	{"Chile ...", "Chile, Martinique / Franz. Antillen, Moldavien Moldau (Republik), St. Pierre und Miquelon"},
/* 9.72 */	{"Französisch-Guayana ...", "Französisch-Guayana, Guadeloupe, Mayotte, Reunion"},
/* 9.96 */	{"Armenien", "Armenien"},
/* 10.44 */	{"Aserbaidschan", "Aserbaidschan"},
/* 11.16 */	{"Malaysia ...", "Malaysia, Niederl. Antillen, Philippinen, Südkorea"},
/* 11.28 */	{"Marianen (SaipanNord-)", "Marianen (SaipanNord-)"},
/* 11.76 */	{"Brasilien", "Brasilien"},
/* 12.78 */	{"Bermuda ...", "Bermuda, Macao, Mexiko, Venezuela"},
/* 12.84 */	{"China", "China"},
/* 12.96 */	{"Angola ...", "Angola, Bhutan, Ecuador, Iran, Saudi Arabien"},
/* 13.32 */	{"Ägypten", "Ägypten"},
/* 14.40 */	{"Aruba ...", "Aruba, Bahamas, Brunei, Dominikanische Republik, Ghana, Jordanien, Kolumbien, Kolumbien Mobilfunk, Kuwait, St. Vincent und Grenadinen, Trinidad und Tobago"},
/* 14.52 */	{"Panama ...", "Panama, Ver. Arabische Emirate"},
/* 14.58 */	{"Barbados ...", "Barbados, Botsuana Botswana, Grönland"},
/* 14.64 */	{"Belize ...", "Belize, Kirgistan"},
/* 15.84 */	{"Libanon", "Libanon"},
/* 15.96 */	{"Gabun", "Gabun"},
/* 16.56 */	{"Bahrain ...", "Bahrain, Katar, Lesotho, Liberia, Namibia, Peru, Sambia, Simbabwe, St. Helena, St. Lucia, Sudan, Swasiland, Tansania, Turks- und Caicosinseln"},
/* 16.68 */	{"Nigeria ...", "Nigeria, Amerikanisch-Samoa"},
/* 16.74 */	{"Burundi ...", "Burundi, Elfenbeinküste Cote de Ivoire, Guinea, Kasachstan, Laos, Nauru, Neukaledonien"},
/* 16.92 */	{"Nepal ...", "Nepal, Syrien"},
/* 17.28 */	{"Guyana", "Guyana"},
/* 18.12 */	{"Guinea-Bissau ...", "Guinea-Bissau, Sao Tome und Principe"},
/* 18.30 */	{"Nicaragua", "Nicaragua"},
/* 18.56 */	{"Malawi", "Malawi"},
/* 18.66 */	{"Argentinien ...", "Argentinien, Benin, Costa Rica, Französisch-Polynesien, Gambia, Jamaika, Kenia, Malediven, Niue-Inseln, Oman, Ruanda, Taiwan, Thailand, Uganda, Vanuatu"},
/* 18.74 */	{"Norfolkinseln", "Norfolkinseln"},
/* 18.78 */	{"Wallis und Futuna", "Wallis und Futuna"},
/* 18.90 */	{"Fidschi", "Fidschi"},
/* 18.96 */	{"Äthiopien ...", "Äthiopien, Irak, Kiribati, Mayotte, Mauretanien, Mongolei, Mosambik, Suriname"},
/* 19.14 */	{"Tonga ...", "Tonga, Zentralafrikanische Republik"},
/* 19.44 */	{"Togo", "Togo"},
/* 20.28 */	{"Myanmar Burma", "Myanmar Burma"},
/* 21.36 */	{"Bolivien ...", "Bolivien, Kaimaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Kuba, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay"},
/* 21.66 */	{"Mauritius ...", "Mauritius, Nordkorea"},
/* 24.06 */	{"Anguilla ...", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso Obervolta, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen (Arab. Republik), Kambodscha, Kamerun, Cape Verde, Kongo, Mali, Montserrat, Niue-Inseln, Pakistan, Salomonen, Senegal, Seyschellen, Somalia, St. Kitts und Nevis, Tschad, Vietnam, Zaire"},
/* 29.28 */	{"Wake Inseln", "Wake Inseln"},
/* 30.60 */	{"Diego Garcia", "Diego Garcia"},
/* 32.16 */	{"Cookinseln ...", "Cookinseln, Madagaskar"},
/* 40.14 */	{"Afghanistan ...", "Afghanistan, Guantanamo, Marshallinseln, Mikronesien, Midway-Inseln, Palau /Belau, Amerikanisch-Samoa"},
		     
};
		     
  
  double Tarif[][4] = 	
			   {{ 0.96, 0.96, 0.84, 0.84}, /* Business regional */
			    { 2.52, 2.52, 1.20, 1.20},
			    { 4.40, 4.40, 4.40, 4.40},
			    { 4.46, 4.46, 4.46, 4.46}, /* One */
{3.24, 3.24, 3.24, 3.24}, /* Deutschland ... */
{3.48, 3.48, 3.48, 3.48}, /* Liechtenstein ... */
{3.60, 3.60, 3.60, 3.60}, /* Italien ... */
{3.96, 3.96, 3.96, 3.96}, /* Frankreich ... */
{4.20, 4.20, 4.20, 4.20}, /* Spanien ... */
{4.32, 4.32, 4.32, 4.32}, /* Belgien ... */
{4.68, 4.68, 4.68, 4.68}, /* Dänemark ... */
{4.74, 4.74, 4.74, 4.74}, /* Monaco ... */
{4.80, 4.80, 4.80, 4.80}, /* Bosnien-Herzegovina ... */
{4.86, 4.86, 4.86, 4.86}, /* Polen ... */
{5.04, 5.04, 5.04, 5.04}, /* Griechenland ... */
{5.16, 5.16, 5.16, 5.16}, /* Andorra ... */
{5.22, 5.22, 5.22, 5.22}, /* Jungfern Inseln USA ... */
{5.28, 5.28, 5.28, 5.28}, /* Puerto Rico ... */
{5.52, 5.52, 5.52, 5.52}, /* Serbien/Montenegro ... */
{5.94, 5.94, 5.94, 5.94}, /* Bulgarien ... */
{6.06, 6.06, 6.06, 6.06}, /* Mazedonien ... */
{6.18, 6.18, 6.18, 6.18}, /* Albanien ... */
{6.24, 6.24, 6.24, 6.24}, /* Israel ... */
{6.30, 6.30, 6.30, 6.30}, /* Gibraltar ... */
{6.36, 6.36, 6.36, 6.36}, /* Australien ... */
{6.38, 6.38, 6.38, 6.38}, /* Russland ... */
{6.42, 6.42, 6.42, 6.42}, /* Island ... */
{7.56, 7.56, 7.56, 7.56}, /* Hongkong ... */
{7.80, 7.80, 7.80, 7.80}, /* Algerien ... */
{7.98, 7.98, 7.98, 7.98}, /* Madeira ... */
{8.04, 8.04, 8.04, 8.04}, /* Christmas Insel ... */
{8.16, 8.16, 8.16, 8.16}, /* Singapur ... */
{8.52, 8.52, 8.52, 8.52}, /* Georgien ... */
{9.48, 9.48, 9.48, 9.48}, /* Südafrika ... */
{9.54, 9.54, 9.54, 9.54}, /* Libyen ... */
{9.66, 9.66, 9.66, 9.66}, /* Chile ... */
{9.72, 9.72, 9.72, 9.72}, /* Franz. Guyana ... */
{9.96, 9.96, 9.96, 9.96}, /* Armenien ... */
{10.44, 10.44, 10.44, 10.44}, /* Aserbaidschan ... */
{11.16, 11.16, 11.16, 11.16}, /* Malaysia ... */
{11.28, 11.28, 11.28, 11.28}, /* Saipan ... */
{11.76, 11.76, 11.76, 11.76}, /* Brasilien ... */
{12.78, 12.78, 12.78, 12.78}, /* Bermudas ... */
{12.84, 12.84, 12.84, 12.84}, /* China ... */
{12.96, 12.96, 12.96, 12.96}, /* Angola ... */
{13.32, 13.32, 13.32, 13.32}, /* Ägypten ... */
{14.40, 14.40, 14.40, 14.40}, /* Aruba ... */
{14.52, 14.52, 14.52, 14.52}, /* Panama ... */
{14.58, 14.58, 14.58, 14.58}, /* Barbados ... */
{14.64, 14.64, 14.64, 14.64}, /* Belize ... */
{15.84, 15.84, 15.84, 15.84}, /* Libanon ... */
{15.96, 15.96, 15.96, 15.96}, /* Gabun ... */
{16.56, 16.56, 16.56, 16.56}, /* Bahrain ... */
{16.68, 16.68, 16.68, 16.68}, /* Nigeria ... */
{16.74, 16.74, 16.74, 16.74}, /* Burundi ... */
{16.92, 16.92, 16.92, 16.92}, /* Nepal ... */
{17.28, 17.28, 17.28, 17.28}, /* Guyana ... */
{18.12, 18.12, 18.12, 18.12}, /* Guinea-Bissau ... */
{18.30, 18.30, 18.30, 18.30}, /* Nicaragua ... */
{18.56, 18.56, 18.56, 18.56}, /* Malawi ... */
{18.66, 18.66, 18.66, 18.66}, /* Argentinien ... */
{18.74, 18.74, 18.74, 18.74}, /* Norfolk Inseln ... */
{18.78, 18.78, 18.78, 18.78}, /* Wallis/Futuna ... */
{18.90, 18.90, 18.90, 18.90}, /* Fidschi ... */
{18.96, 18.96, 18.96, 18.96}, /* Äthiopien ... */
{19.14, 19.14, 19.14, 19.14}, /* Tonga ... */
{19.44, 19.44, 19.44, 19.44}, /* Togo ... */
{20.28, 20.28, 20.28, 20.28}, /* Burma/Myanmar ... */
{21.36, 21.36, 21.36, 21.36}, /* Bolivien ... */
{21.66, 21.66, 21.66, 21.66}, /* Mauritius ... */
{24.06, 24.06, 24.06, 24.06}, /* Anguilla ... */
{29.28, 29.28, 29.28, 29.28}, /* Wake ... */
{30.60, 30.60, 30.60, 30.60}, /* Diego Garcia ... */
{32.16, 32.16, 32.16, 32.16}, /* Cook Inseln ... */
{40.14, 40.14, 40.14, 40.14} /* Afghanistan ... */
};			 
  int  z;
  char s[BUFSIZ];

    printf ("\n");
    rprintf ("CyberTron easy.Talk business", "P:29,2");

  rprintf ("Cybertron - austrian digital telecom","C:Name:");
  rprintf ("","C:Address:");
  rprintf ("http://www.cybertron.at","C:Homepage:");
  rprintf ("http://www.cybertron.at/html/easy.prod_0.html","C:TarifURL:");
  rprintf ("0800 400 400, 1029 1029","C:Telefon:");
  rprintf ("Die Regionalzone geht bis zu einer Entfernung von 100 Km, die Fernzone ","C:Zone:");
  rprintf ("darüber - Die Regionalzone ist nicht implementiert","C:Zone:");
  rprintf ("Variable Taktung.","C:Special:");

    rprintf ("# Verzonung", "#Fixme: +-100Km nicht implemtiert");
    sprintf (s, "%s (%.3f Takteinheit)", Name, Einheit);
    rprintf (s, "# Tarif:");
    rprintf ("Internationale Zonen nicht implementiert!", "# Fixme:");

    for (z=0; z<COUNT(Zone); z++) {
      rprintf (Zone[z][0], "Z:%d", z+1);
      if (Tarif[z][0]!=0.0) { /* Tarif bekannt? */
	if (Tarif[z][0]==Tarif[z][1] && Tarif[z][0]==Tarif[z][2] && Tarif[z][0]==Tarif[z][3]) {
	  rprintf ("0-24h", "T:*/*=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	} else {
	  if (Tarif[z][0]==Tarif[z][1]) {
	    rprintf ("Tageszeit", "T:W/8-18=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	  } else {
	    rprintf ("Tageszeit 1", "T:W/8-12=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	    rprintf ("Tageszeit 1", "T:1-4/13-16=%.2f(60)/%.0f", Tarif[z][0], Einheit/Tarif[z][0]);
	    rprintf ("Tageszeit 2", "T:1-4/12-13,16-18=%.2f(60)/%.0f", Tarif[z][1], Einheit/Tarif[z][1]);
	    rprintf ("Tageszeit 2", "T:5/12-18=%.2f(60)/%.0f", Tarif[z][1], Einheit/Tarif[z][1]);
	  }
	  if (Tarif[z][2]==Tarif[z][3]) {
	    rprintf ("Sparzeit", "T:*/*=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	  } else {
	    rprintf ("Sparzeit",      "T:W/06-08,18-20=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	    rprintf ("Sparzeit",      "T:E,H/06-20=%.2f(60)/%.0f", Tarif[z][2], Einheit/Tarif[z][2]);
	    rprintf ("Supersparzeit", "T:*/20-06=%.2f(60)/%.0f", Tarif[z][3], Einheit/Tarif[z][3]);
	  }
	}
      }
      print_area(Zone[z][1]);
    }
}

void rate_1044(void) {

  char *Zone[][2] = {{ "Regional", "" }, 
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Ausland", "+" }};
  
  double Tarif[][2] = {{ 0.864, 0.864 },
		       { 1.728, 0.864 },
		       { 5.04, 5.04 },
		       { 0.00, 0.00 }};
  
  int z;
  
  printf ("\n");
  rprintf ("Citykom", "P:44");

  rprintf ("Citykom Austria Telekommunikations GmbH","C:Name:");
  rprintf ("Alserbachstr. 144-16, A-1090 Wien","C:Address:"); 
  rprintf ("http://www.citykom.at","C:Homepage:");
  rprintf ("j.schmolinsk@citykom.at","C:EMail:");
  rprintf ("0800 201 301","C:Telefon:"); 
  rprintf ("Vorwahlen 1XXX und 2XXX, bzw. mit gleicher Anfangsziffer liegen in der ","C:Zone:");
  rprintf ("Regionalzone sonst in der Österreichzone.","C:Zone:");
/*  rprintf ("","C:Special:"); */

  rprintf ("# Verzonung", "D:1012");
  rprintf ("internationale Zonen nicht implementiert", "# Fixme:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif [z][0]!=0.0) { /* Tarif bekannt? */
      if (Tarif[z][0]==Tarif[z][1]) {
	rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z][0]);
      } else {
	rprintf ("Tag", "T:W/8-18=%.2f(60)/1", Tarif[z][0]);
	rprintf ("Nacht", "T:W/18-8=%.2f(60)/1", Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=%.2f(60)/1", Tarif[z][1]);
      }
    }
    print_area(Zone[z][1]);
  }
}

void rate_1049(void) {

  char *Zone[][2] = {{ "Regionalzone", ""},
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
/* 3.00 */	{"Deutschland", "Deutschland"},
/* 3.60 */	{"Italien ...", "Italien, Liechtenstein, Schweiz"},
/* 3.96 */	{"Dänemark ...", "Dänemark, Frankreich, Finnland, Großbritannien, Niederlande, Schweden, Ungarn"},
/* 4.08 */	{"Belgien", "Belgien"},
/* 4.20 */	{"Kanada ...", "Kanada, Norwegen, Slowenien, USA"},
/* 4.56 */	{"Spanien", "Spanien"},
/* 4.68 */	{"Irland ...", "Irland, Luxemburg, Slowakische Republik, Tschechische Republik"},
/* 4.80 */	{"Polen", "Polen"},
/* 5.28 */	{"Amerikanische Jungferninseln ...", "Amerikanische Jungferninseln, Andorra, Azoren, Griechenland, Monaco, Malta, Jugoslawien, Jugoslawien, Ukraine"},
/* 5.52 */	{"Kroatien", "Kroatien"},
/* 5.64 */	{"Portugal", "Portugal"},
/* 5.88 */	{"Albanien ...", "Albanien, Bosnien-Herzegowina, Bulgarien, Rumänien, Zypern (Griechenland)"},
/* 6.00 */	{"Puerto Rico ...", "Puerto Rico, San Marino"},
/* 6.24 */	{"Australien ...", "Australien, Türkei"},
/* 6.60 */	{"Algerien ...", "Algerien, Estland, Mazedonien"},
/* 7.08 */	{"Island ...", "Island, Israel, Palestina"},
/* 7.20 */	{"Gibraltar ...", "Gibraltar, Lettland, Litauen, Russische Förderation (östl.), Tadschikistan, Weissrussland"},
/* 7.44 */	{"Weihnachtsinseln ...", "Weihnachtsinseln, Tunesien"},
/* 7.92 */	{"Färöer-Inseln ...", "Färöer-Inseln, Kanarische Inseln, Madeira, Marokko"},
/* 8.28 */	{"Georgien ...", "Georgien, Japan, Singapur"},
/* 9.00 */	{"Hongkong ...", "Hongkong, Usbekistan"},
/* 9.24 */	{"Armenien ...", "Armenien, Aserbaidschan, Antarktis"},
/* 9.48 */	{"Libyen ...", "Libyen, Neuseeland"},
/* 9.60 */	{"Chile ...", "Chile, Martinique / Franz. Antillen"},
/* 10.20 */	{"Moldavien Moldau (Republik) ...", "Moldavien Moldau (Republik), Südafrika"},
/* 10.68 */	{"Guadeloupe ...", "Guadeloupe, Niederl. Antillen, Südkorea, Reunion, Zypern (Türkei)"},
/* 10.92 */	{"Französisch-Guayana", "Französisch-Guayana"},
/* 11.40 */	{"Turkmenistan", "Turkmenistan"},
/* 11.88 */	{"Malaysia", "Malaysia"},
/* 12.60 */	{"Ägypten ...", "Ägypten, Angola, Brasilien, Bermuda, Bhutan"},
/* 13.08 */	{"China ...", "China, Venezuela"},
/* 13.80 */	{"Dominikanische Republik ...", "Dominikanische Republik, Ecuador"},
/* 14.28 */	{"Macao ...", "Macao, Philippinen, Taiwan"},
/* 14.40 */	{"Aruba ...", "Aruba, Bahamas, Bolivien, Ver. Arabische Emirate"},
/* 14.64 */	{"Jordanien", "Jordanien"},
/* 15.00 */	{"Barbados ...", "Barbados, Belize, El Salvador, Ghana, Grönland, Mayotte"},
/* 15.24 */	{"Saudi Arabien", "Saudi Arabien"},
/* 15.48 */	{"Iran ...", "Iran, Kuwait, Kirgistan, St. Vincent und Grenadinen"},
/* 15.60 */	{"Argentinien", "Argentinien"},
/* 15.84 */	{"Liberia", "Liberia"},
/* 16.20 */	{"Burkina Faso Obervolta ...", "Burkina Faso Obervolta, Burundi, Trinidad und Tobago"},
/* 16.44 */	{"Libanon", "Libanon"},
/* 16.68 */	{"Diego Garcia ...", "Diego Garcia, Gabun, Guinea, Peru, Panama, Swasiland, Syrien, Tansania"},
/* 17.28 */	{"Kasachstan ...", "Kasachstan, Katar"},
/* 17.40 */	{"Bahrain ...", "Bahrain, Benin, Kaimaninseln"},
/* 17.88 */	{"Dominica ...", "Dominica, Kolumbien, Malawi, St. Pierre und Miquelon, Sudan"},
/* 18.00 */	{"Äquatorial-Guinea ...", "Äquatorial-Guinea, Elfenbeinküste Cote de Ivoire, Französisch-Polynesien"},
/* 19.08 */	{"Fidschi ...", "Fidschi, Guyana, Mayotte, Kenia, Namibia, Nauru, Nepal, Nigeria, Neukaledonien, Sao Tome und Principe, St. Helena, St. Kitts und Nevis, St. Lucia, Simbabwe, West-Samoa"},
/* 19.20 */	{"Ascension ...", "Ascension, Britische Jungferninseln, Brunei"},
/* 19.56 */	{"Botsuana Botswana", "Botsuana Botswana"},
/* 19.80 */	{"Äthiopien ...", "Äthiopien, Anguilla, Gambia, Grenada, Norfolkinseln, Sambia"},
/* 20.28 */	{"Cookinseln ...", "Cookinseln, Costa Rica, Dschibuti, Guam, Guatemala, Guinea-Bissau, Kiribati, Cape Verde, Laos, Lesotho, Mikronesien, Mongolei, Montserrat, Mosambik, Nicaragua, Oman, Papua-Neuguinea, Paraguay, Sri Lanka, Thailand, Tonga, Uganda, Uruguay"},
/* 21.48 */	{"Honduras ...", "Honduras, Indien, Indonesien, Irak, Jamaika, Mauritius, Malediven, Mauretanien, Niger, Pakistan, Palau /Belau, Ruanda, Somalia, Amerikanisch-Samoa, Sierra Leone, Togo, Tuvalu, Vanuatu"},
/* 21.60 */	{"Antigua und Barbuda", "Antigua und Barbuda"},
/* 22.20 */	{"Eritrea", "Eritrea"},
/* 22.68 */	{"Niue-Inseln ...", "Niue-Inseln, Jemen (Arab. Republik), Senegal, Seyschellen, Wallis und Futuna, Zentralafrikanische Republik"},
/* 23.40 */	{"Afghanistan ...", "Afghanistan, Falklandinseln, Kuba, Madagaskar"},
/* 23.88 */	{"Haiti ...", "Haiti, Jemen (Arab. Republik), Kambodscha, Kamerun, Kongo, Nordkorea, Mali, Marshallinseln, Myanmar Burma, Pitcairn Inseln, Salomonen, Suriname, Tschad, Vietnam"},
/* 35.88 */	{"Wake Inseln", "Wake Inseln"},
};

  double Tarif[] = { 0.96, /* W,N,B 50 km */
		     1.74,
		     1.74,
		     3.60,
3.00,  /* Deutschland ... */
3.60,  /* Italien ... */
3.96,  /* Dänemark ... */
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
7.92,  /* Färöer - Inseln ... */
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
12.60,  /* Ägypten ... */
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
18.00,  /* Äquat. - Guinea ... */
19.08,  /* Fidschi ... */
19.20,  /* Ascension ... */
19.56,  /* Botswana ... */
19.80,  /* Äthiopien ... */
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
  
  printf ("\n");
  rprintf ("Pegasus Telekom", "P:49");
  rprintf ("# Verzonung", "D:pta");
  rprintf ("Pegasus Telekom Netzwerkdienste AG","C:Name:");
  rprintf ("Serrravag. 10-12, A-1140 Wien","C:Address:");
  rprintf ("http://www.ptag.net","C:Homepage:"); 
  rprintf ("http://www.ptag.net/F_Tarife.htm","C:TarifURL:"); 
  rprintf ("01 9150 0","C:Telefon:"); 
  rprintf ("Fixme: Verzonung nicht verifiziert.","#");
//  rprintf ("Bei einem Rechnungswert unter ATS 500 netto, werden ATS 50 zusätzlich verrechnet.","C:GT:");
//  rprintf ("Cost = Ch < 500 ? 50 : 0","C:GF:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/1", Tarif[z]);
    print_area(Zone[z][1]);
  }
}


void rate_1066(void) {

  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "+43663,+43664,+43676,+43699" },
		     { "Euro 1", "Deutschland, Frankreich, Großbritannien, Nordirland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Euro 2", "Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark, Estland, Finnland, Griechenland, Irland, Jugoslawien, Kroatien, Luxemburg, Monaco, Niederlande, Norwegen, Polen, Portugal, Rumänien, San Marino, Schweden, Spanien, Tunesien, Türkei, Ukraine, Vatikanstadt, Zypern" },
		     { "Euro City", "Amsterdam, Brüssel, Frankfurt, London, Luxemburg City, Mailand, Paris, Zürich" },
		     { "Welt 1", "Hawaii, Amerikanische Jungferninseln, Puerto Rico, Vereinigte Staaten (USA), Kanada" },
		     { "Welt 2", "Albanien, Antarktis, Armenien, Aserbaidschan, Australien, Bahamas, Chile, Weihnachtsinseln, Kokosinseln, Dominikanische Republik, Georgien, Gibraltar, Guadeloupe, Französisch-Guayana, Hongkong, Island, Israel, Japan, Lettland, Libyen, Litauen, Mazedonien, Malaysia, Malta, Marokko, Martinique, Mayotte, Moldau, Neuseeland, Niederländische Antillen, Philippinen, Russische Föderation, Reunion, St. Pierre und Miquelon, Singapur, Südafrika, Südkorea, Saipan" },
		     { "Welt 3", "Ägypten, Angola, Argentinien, Aruba, Ascension, Bahrain, Barbados, Belize, Benin, Bermuda, Bhutan, Bolivien, Botsuana, Brasilien, Brunei, Burkina Faso, Burundi, Caymaninseln, China, Costa Rica, Dschibuti, Dominica, Ecuador, El Salvador, Färöer-Inseln, Falklandinseln, Fidschi, Französisch-Polynesien, Gabun, Gambia, Ghana, Grenada, Grönland, Guatemala, Haiti, Honduras, Indien, Indonesien, Iran, Jamaika, Jordanien, Katar, Kenia, Kirgisistan, Kolumbien, Nordkorea, Kongo, Kuba, Kuwait, Laos, Lesotho, Libanon, Liberia, Macau, Malawi, Malediven, Marshallinseln, Mauretanien, Mauritius, Mexiko, Montserrat, Nepal, Neukaledonien, Nicaragua, Nigeria, Norfolk-Inseln, Panama, Paraguay, Peru, Ruanda, Sambia, Saudi-Arabien, Salomonen, Simbabwe, Somalia, St. Helena, Saint Kitts und Nevis, St. Lucia, Saint Vincent und die Grenadinen, Sudan, Syrien, Tadschikistan, Tansania, Taiwan, Thailand, Trinidad und Tobago, Turkmenistan, Uruguay, Usbekistan, Vanuatu, Venezuela, Vereinigte Arabische Emirate" },
		     { "Welt 4", "Afghanistan, Algerien, Anguilla, Antigua und Barbuda, Äquatorial-Guinea, Äthiopien, Bangladesch, Cook-Inseln, Cote d'Ivoire, Diego Garcia, Eritrea, Guam, Guantanamo, Guinea, Guinea-Bissau, Guyana, Irak, Jemen, Kambodscha, Kamerun, Kap Verde, Kasachstan, Kiribati, Komoren, Madagaskar, Mali, Midway-Inseln, Mikronesien, Mongolei, Mosambik, Myanmar, Namibia, Nauru, Niger, Niue, Oman, Pakistan, Palau, Papua-Neuguinea, Pitcairn-Inseln, Sao Tome und Principe, Amerikanisch-Samoa, Samoa, Senegal, Seychellen, Sierra Leone, Sri Lanka, Suriname, Swasiland, Togo, Tonga, Tschad, Turks- und Caicos-Inseln, Tuvalu, Uganda, Vietnam, Wake-Inseln, Zaire, Zentralafrikanische Republik" }};
  
  double Tarif[][2] = {{ 1.00, 0.80}, /* Fixme: */
		       { 2.20, 1.00 },
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

  rprintf ("M.I.T Multi Media Informations Technologies GesmbH","C:Name:");
  rprintf ("Modecenterstrr. 14/5, A-1030 Wien","C:Address:"); 
  rprintf ("http://www.mit.co.at","C:Homepage:");
  rprintf ("01 79040 1500","C:Telefon:"); 
  rprintf ("Die Regionalzone geht bis zu einer Entfernung von 50 Km, alles andere ist ","C:Zone:");
  rprintf ("Österreichzone.","C:Zone:");
  rprintf ("0.30 ATS Mindestentgelt pro Verbindung","C:Special:");
  rprintf ("# Verzonung", "D:1066");
  rprintf ("Verzonung -50/+50 nicht verifiziert", "# Fixme");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
	rprintf ("0-24h", "T:*/*=0.30|%.2f(60)/1", Tarif[z][0]);
    } else {
	rprintf ("Tag", "T:W/8-18=0.30|%.2f(60)/1", Tarif[z][0]);
	rprintf ("Nacht", "T:W/18-8=0.30|%.2f(60)/1", Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=0.30|%.2f(60)/1", Tarif[z][1]);
    } 
    print_area(Zone[z][1]);
  }
}

void rate_1067(void) {
  
  char *Zone[][2] = {{ "Festnetz", "Österreich" }, 
		     { "max.box", "+4367622" },
		     { "max.online", "+436762323" },
		     { "max.mobil", "+43676" },
		     { "andere Mobilfunknetze", "+43663,+43664,+43669" },
		     { "Nachbarländer", "Deutschland, Italien, Schweiz, Tschechische Republik, Ungarn, Slowakische Republik, Slowenien, Liechtenstein" },
		     { "EU", "Belgien, Dänemark, Finnland, Frankreich, Griechenland, Irland, Luxemburg, Niederlande, Portugal, Spanien, Schweden, Großbritannien, Nordirland" },
		     { "Weltzone 1", "Andorra, Australien, Bahamas, Bosnien-Herzegowina, Kanada, Kroatien, Färöer-Inseln, Gibraltar, Island, Japan, Lettland, Malta, Mazedonien, Monaco, Neuseeland, Norwegen, Polen, Puerto Rico, San Marino, Singapur, USA, Vatikan, Jugoslawien" },
		     { "Weltzone 2", "Albanien, Algerien, Belarus, Bulgarien, Zypern, Estland, Hong Kong, Litauen, Marokko, Oman, Rumänien, Rußland, Tunesien, Türkei, Ukraine, American Virgin Isles" },
		     { "Weltzone 3", "Bermuda, Brasilien, Brunei, Chile, Christmas Island, Cocos Island, Dominikanische Republik, Georgien, Ghana, Grönland, Israel, Jamaica, Kasachstan, Südkorea, Kirgisistan, Libyen, Macau, Malaysia, Mexiko, Südafrika, Taiwan, Tadschikistan" },
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
  rprintf ("Michael Reinelt <reinelt@eunet.at>", "C:Maintainer:");
  rprintf ("max.plus","C:Name:");
/*  rprintf ("","C:Address:"); */
  rprintf ("http://www.maxmobil.at","C:Homepage:");
  rprintf ("0676 20 200","C:Telefon:");
  rprintf ("Nur für max - Vertragskunden.","C:Special:");

  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/30", Tarif[z]);
    print_area(Zone[z][1]);
  }
}    


void rate_1069(void) {

  char *Zone[][2] = {{ "Festnetz", "Österreich" }, 
		      { "One Mobilfunknetz", "+43699" },
		      { "andere Mobilfunknetze", "+43663,+43664,+43676" },
		      { "EU und Nachbarländer", "Belgien, Dänemark, Deutschland, Finnland, Frankreich, Großbritannien, Irland, Italien, Liechtenstein, Luxemburg, Niederlande, Norwegen, Schweden, Schweiz, Slowakei, Slowenien, Spanien, Tschechien, Ungarn" },
		      { "Internationale Zone 2", "Andorra, Australien, Bosnien-Herzegowina, Griechenland, Island, Japan, Jugoslawien, Kanada, Kroatien, Malta, Monaco, Neuseeland, Polen, Portugal, Puerto Rico, Mazedonien, San Marino, Vereinigte Staaten (USA), Vatikanstadt" },
		      { "Internationale Zone 3", "Albanien, Algerien, Bulgarien, Estland, Gibraltar, Hongkong, Israel, Lettland,Litauen, Marokko, Rumänien, Rußland, Singapur, Tunesien, Türkei, Ukraine, Weißrußland, Zypern" },
		      { "Internationale Zone 4", "Aserbaidschan, Amerikanisch-Samoa, Armenien, Bahamas, Bermuda, Brasilien, Brunei, Chile, Dominikanische Republik, Falklandinseln, Georgien, Ghana, Grönland, Jamaika, Kasachstan, Kokosinseln, Libyen, Macao, Mexico, Malaysia, Moldau, Oman, Südkorea, Südafrika, Tadschikistan,Taiwan, Usbekistan, Venezuela, Vereinigte Arabische Emirate, Weihnachtsinseln" },
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

  rprintf ("Connect Austria Gesellschaft für Telekommunikation GmbH","C:Name:");
  rprintf ("Brünner Straße 52, A-1210 Wien ","C:Address:"); 
  rprintf ("http://www.one.at","C:Homepage:");
  rprintf ("0800 1069 3000","C:Telefon:");
/*  rprintf ("","C:Zone:");
  rprintf ("","C:Special:"); */
  
  for (z=0; z<7; z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("0-24h", "T:*/*=%.2f(60)/30", Tarif[z]);
    print_area(Zone[z][1]);
  }
}    

void rate_priority(void) {
 /*
  Classic 136 Grundentgeld
   Priority 0.54,0.36
   Regional 0.84,0.42
   Inland   2.46,0.84
   Mobil    4.50,3.40
   
  Business 396 Grundentgeld
   Priority 0.48,0.34
   Regional 0.72,0.35
   Inland   0.96,0.72
   Mobil    3.84,2.88
   
  Standard 200 Grundentgeld
   Priority 0.54,0.36
   Regional 0.80,0.40
   Inland   1.00,0.80
   Mobil    4.00,3.00
   
  (8-18 , *) 
  */
  rprintf ("Priority Telecom","P:PT-TK"); /* Telekabel */
  rprintf ("Priority Telecom, Telekabel Wien Ges.m.b.H.","C:Name:");
  rprintf ("","C:Address:");
  rprintf ("http://www.prioritytelecom.at","C:Homepage:");
  rprintf ("infp@prioritytelecom.at", "C:EMail:");
  rprintf ("0800 700 700","C:Telefon:");
  rprintf ("","C:Zone:");
  rprintf ("Nur über Telekabel Wien","C:Special:");
}


int main (int argc, char *argv[])
{
  printf ("# created by rate-at.c\n\n");
  printf ("# The information herein was machine-generated,\n");
  printf ("# so do not contribute patches to this file.\n\n");
  printf ("# Please contact Michael Reinelt <reinelt@eunet.at>\n");
  printf ("# or Leo Tötsch <lt@toetsch.at> if you have any\n");
  printf ("# corrections or additions.\n\n");
  printf ("# Many thanks to Daniela Bruder <dbruder@sime.com>\n");
  printf ("# for collecting and preparing most of the call charges.\n\n\n");

  printf ("V:1.84-Austria [30-Aug-1999]\n\n");
  printf ("U:%%.3f öS\n");

#if 0
  rate_1066();
#else
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
  rate_1012();
  rate_1013();
  rate_1024();
  rate_1029_privat();
  rate_1029_business();
  rate_1044();
  rate_1049();
  rate_1066();
  rate_1067();
  rate_1069();
#endif
  return(EXIT_SUCCESS);	
}

/* template 
  rprintf ("","C:Name:");
  rprintf ("","C:Maintainer:");
  rprintf ("","C:Address:");
  rprintf ("","C:Homepage:");
  rprintf ("","C:TarifURL:");
  rprintf ("","C:EMail:");
  rprintf ("","C:Telefon:");
  rprintf ("","C:Telefax:");
  rprintf ("","C:Hotline:");
  rprintf ("","C:Zone:");
  rprintf ("","C:Special:"); 
  rprintf ("","C:GT:"); 
  rprintf ("","C:GF:"); 
  */

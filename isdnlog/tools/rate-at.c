#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define TABSTOP 40
#define COUNT(array) sizeof(array)/sizeof(array[0])

typedef struct {
  char *code;
  char *name;
} COUNTRY;

COUNTRY *Country = NULL;
int     nCountry = 0;

char *strip (char *s)
{
  char *p;

  while (*s==' ' || *s=='\t') s++;
  for (p=s; *p; p++)
    if (*p=='#' || *p=='\n') {
      *p='\0';
      break;
    }
  for (p--; p>s && (*p==' '||*p=='\t'); p--)
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

static char* str2set (char **s)
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

char *xlat (char *s)
{
  static char buffer[BUFSIZ];
  char *p = buffer;

  do {
    *p=tolower(*s);
    switch (*p) {
    case 'ä':
      *p++='a';
      *p  ='e';
      break;
    case 'ö':
      *p++='o';
      *p  ='e';
      break;
    case 'ü':
      *p++='u';
      *p  ='e';
      break;
    case 'ß':
      *p++='s';
      *p  ='s';
      break;
    }
    p++;
  } while (*s++);
  return buffer;
}

int min3(int x, int y, int z)
{
  if (x < y)
    y = x;
  if (y < z)
    z = y;
  return(z);
}

#define WMAX 64
#define  P    1
#define  Q    1
#define  R    1

int wld(char *needle, char *haystack) /* weighted Levenshtein distance */
{
  int i, j;
  int l1 = strlen(needle);
  int l2 = strlen(haystack);
  int dw[WMAX+1][WMAX+1];

  dw[0][0]=0;

  for (j=1; j<=WMAX; j++)
    dw[0][j]=dw[0][j-1]+Q;

  for (i=1; i<=WMAX; i++)
    dw[i][0]=dw[i-1][0]+R;

  for (i=1; i<=l1; i++)
    for(j=1; j<=l2; j++)
      dw[i][j]=min3(dw[i-1][j-1]+((needle[i-1]==haystack[j-1])?0:P),dw[i][j-1]+Q,dw[i-1][j]+R);

  return(dw[l1][l2]);
}

void initCountry (char *path)
{
  FILE *stream;
  char  buffer[BUFSIZ];
  char *p, *s;
  int   n = 0;

  if ((stream=fopen(path,"r"))==NULL) {
    fprintf (stderr, "Error: could not load countries from %s: %s\n", path, strerror(errno));
    return;
  }

  while ((s=fgets(buffer,BUFSIZ,stream))!=NULL) {
    if (*(s=strip(s))=='\0')
      continue;
    p=s;
    while (*p) {
      if (isblank(*p))
	break;
      p++;
    }
    if (*p=='\0')
      continue;

    *p='\0';
    p=strip(p+1);
    if (*s=='0' && *(s+1)=='0')
      *++s='+';
    
    Country=realloc (Country, (nCountry+1)*sizeof(COUNTRY));
    Country[nCountry].name=strdup(p);
    Country[nCountry].code=strdup(s);
    nCountry++;
    n++;
  }
  fprintf (stderr, "loaded %d countries from %s\n", n, path);
  return;
}

char *findArea (char *country)
{
  char buffer[BUFSIZ];
  int  c, i, m, w;

  c=0;
  w=0;
  m=666;
  strcpy (buffer, xlat(country));
  for (i=0; i<nCountry; i++)
    if ((w=wld (buffer, xlat(Country[i].name)))<m) {
      m=w;
      c=i;
    }
  if (m>0)
    fprintf (stderr, "%s = %s?\n", country, Country[c].name);
  
  return Country[c].code;
}

static int byArea(const void *a, const void *b)
{
  return strcmp (((COUNTRY*)a)->code, ((COUNTRY*)b)->code);
}

void print_area(char *area) 
{
  int     nArea=0;
  COUNTRY *Area=NULL;
  char     comment[BUFSIZ];
  char    *a, *b;
  int      i;

  if (!*area)
    return;

  while (1) {
    if (*(a=strip(str2set(&area)))) {
      Area=realloc(Area, (nArea+1)*sizeof(COUNTRY));
      if (*a=='+') {
	Area[nArea].code=strdup(a);
	Area[nArea].name=*(a+1)=='\0'?strdup("alle übrigen Länder"):NULL;
      } else if (strcmp(a,"0663")==0) {
	Area[nArea].code=strdup("+43663");
	Area[nArea].name=strdup("C-Netz");
      } else if (strcmp(a,"0664")==0) {
	Area[nArea].code=strdup("+43664");
	Area[nArea].name=strdup("D-Netz");
      } else if (strcmp(a,"0676")==0) {
	Area[nArea].code=strdup("+43676");
	Area[nArea].name=strdup("max.mobil");
      } else if (strcmp(a,"0699")==0) {
	Area[nArea].code=strdup("+43699");
	Area[nArea].name=strdup("one");
      } else {
	Area[nArea].code=strdup(findArea(a));
	Area[nArea].name=strdup(a);
      }
      nArea++;
    }
    if (*area==',') {
      area++;
      continue;
    }
    break;
  }
  qsort(Area, nArea, sizeof(COUNTRY), byArea);
  a=""; b="";
  for (i=0; i<nArea; i++) {
    if (strcmp(a,Area[i].code)==0) {
      sprintf (comment, "%s, %s", b, Area[i].name);
      free (Area[i-1].code);Area[i-1].code=NULL;
      free (Area[i-1].name);Area[i-1].name=NULL;
      free (Area[i].name);Area[i].name=strdup(comment);
    }
    a=Area[i].code;
    b=Area[i].name;
  }  
  
  for (i=0; i<nArea; i++) {
    if (Area[i].code) {
      if (Area[i].name) {
	sprintf (comment, "# %s", Area[i].name);
	rprintf (comment, "A:%s", Area[i].code);
	free (Area[i].name);
      } else {
	rprintf (NULL, "A:%s", Area[i].code);
      }
      free (Area[i].code);
    }
  }
  free (Area);
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


void rate_1001(void) {

  char *Name[] = { "Minimumtarif",
		   "Standartarif",
		   "Geschäftstarif 1",
		   "Geschäftstarif 2",
		   "Geschäftstarif 3" };

  double Tarif[] = { 1.116, 1.056, 0.996, 0.936, 0.816 };

  char *Zone[][2] = {{"FreePhone", "+43660" },
		     { "Regionalzone", "" }, 
		     { "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Online-Tarif", "+437189" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
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
    sprintf (s, "%s (ATS %.3f / Impuls)", Name[t], Tarif[t]);
    rprintf (s, "C:Tarif:");
    for (z=0; z<COUNT(Zone); z++) {
      printf ("\n");
      rprintf (Zone[z][0], "Z:%d", z);
      if (z==0) { /* Freephone */
	rprintf (NULL,"T:*/*=0/72\n");
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


void rate_1002(void) {

  char *Zone[][2] = {{ "Bundeslandzone", "" }, 
		     { "Österreichzone", "Österreich" },
		     { "special friends", "" },
		     { "Mobilnetz", "0663,0664,0676,0699" },
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
    rprintf ("Geschäftszeit", "T:*/*=%.2f(60)/1", Tarif[z][0]);
    rprintf ("Freizeit", "T:*/*=%.2f(60)/1", Tarif[z][1]);
    print_area(Zone[z][1]);
  }
}    

void rate_1003(void) {

  char *Zone[][2] = {{ "Regionalzone", "" }, 
		     { "Fernzone 1", "" },
		     { "5 Freunde", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
		     { "Deutschland spezial", "Deutschland" },
		     { "Europa spezial", "Belgien, Frankreich, Großbritannien" },
		     { "Ausland Zone 1", "Czech Republic, Hungary, Italy, Slovakia, Slovenia, Switzerland" },
		     { "Ausland Zone 2", "Bosnia and Hercegovina, Canada, Croatia, Denmark, Finland, Luxembourg, Monaco, Netherlands, Norway, Poland, Puerto Rico, Spain, Sweden, United States of America, American Virgin Isles, Vatican City, Montenegro" },
		     { "Ausland Zone 3", "Andorra, Belarus, Bulgaria, Greece, Ireland, Malta, Macedonia, Romania, San Marino, Tunesia" },
		     { "Ausland Zone 4", "Albania, Cyprus, Israel, Portugal, Turkey, Ukraine" },
		     { "Ausland Zone 5", "Algeria, Antarctica, Australia, Christmas Island, Cocos Island, Estonia, Färöer, Gibraltar, Japan, South Korea, Latvia, Moldova, Marocco, Russian Federation, South Africa, Tajikistan" },
		     { "Ausland Zone 6", "Azerbaijan, Chile, French Guyana, Georgia, Greenland, Guadaloupe, Hongkong, Iceland, Libya, Lithuania, Martinique, Mayotte, Mexico, Phillipines, Reunion, St. Pierre and Miquelon, Singapore, Thailand, Turkmenistan, Uganda, United Arab Emirates" },
		     { "Ausland Zone 7", "Angola, Argentina, Bahrain, Bhutan, Botswana, Brazil, Burundi, Comoros, Dominican Republic, Ecuador, Egypt, Gabon, Gambia, India, Indonesia, Iran, Jordan, Kuwait, Kyrgyzstan, Macau, Malawi, Malaysia, Namibia, Netherlands Antilles, New Zealand, Nigeria, Oman, Saipan, Saudi Arabia, Sri Lanka, Swaziland, Taiwan, Venezuela, Zambia, Zimbabwe" },
		     { "Ausland Zone 8", "Armenia, Aruba, Bahamas, Barbados, Belize, Bermuda, Brunei, Central African Republic, China, Colombia, Costa Rica, Cuba, Djibouti, Equatorial Guinea, Ghana, Guinea-Bissau, Guyana, Kazakhstan, Lesotho, Liberia, Mongolia, Nepal, Panama, Saint Helena, Sao Tome and Principa, Sudan, Syria, Tanzania, Trinidad & Tobago, Turks & Caicos Islands" },
		     { "Ausland Zone 9", "Ascension, Benin, Bolivia, Burkina Faso, Cameroon, Cape Verde, Chad, Elfenbeinküste, El Salvador, Ethiopia, Fiji, French Polynesia, Guatemala, Haiti, Honduras, Iraq, Jamaica, Kenya, Kiribati, Laos, Lebanon, Maldives, Mali, Mauritania, Mauritius, Mozambique, Myanmar, Nauru, New Caledonia, Nicaragua, Niue, Norfolk Island, Pakistan, Papua New Guinea, Peru, Quatar, Rwanda, Saint Lucia, St. Vincent and the Grenadines, Samoa, Senegal, Seychelles Islands, Sierra Leone, Somalia, Suriname, Togo, Tonga, Uzbekistan, Vanuatu, Jemen, Zaire" },
		     { "Ausland Zone 10", "Afghanistan, American Samoa, Anguilla, Antigua & Barbuda, Bangladesh, British Virgin Isles, Cambodia, Cayman Islands, Congo, Cook Islands, Dominica, Eritrea, Falkland Islands, Grenada, Guam, Guantanamo, Nordkorea, Madagascar, Marshall Islands, Montserrat, Niger, Palau, Paraguay, Pitcairn Islands, Tuvalu, Uruguay, Vietnam, Wake Island" }};
  
  double Tarif[][2] = {{ 1.0,  1.0},
		       { 1.0,  1.0},
		       { 1.0,  1.0},
		       { 2.0,  2.0},
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

  char *Zone[][2] = {{ "National", "Österreich" }, 
		     { "Mobilfunknetz", "0663,0664,0676,0699" },
		     { "Europa 1 / USA", "Belgien, Dänemark, Deutschland,  Finnland, Frankreich, Griechenland, Grossbritannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Nordirland, Norwegen, Schweden, Schweiz, Slowakei, Slowenien, Spanien,  Tschechien, Ungarn" },
		     { "Europa 2", "Albanien, Algerien, Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Färöer-Inseln, Gibraltar, Island, Israel, Jugoslawien, Kroatien, Lettland, Litauen, Mazedonien, Malta, Marokko, Polen, Portugal, Rumänien, Russland, San Marino, Tunesien, Türkei, Zypern" },
		     { "Welt 1", "Australien, Hongkong, Japan, Neuseeland, Südkorea, Singapur" },
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
		   "14-tägig kündbar" };

  char *Zone[][2] = {{ "Fernzone 1", "" },
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
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
    rprintf (Name[t], "C:Tarif:");
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
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
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
  
  double Tarif[][2] = {{ 1.00, 1.00 },
		       { 1.98, 1.20 },
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
		     { "Fern", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
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
		     { "Österreich", "Österreich" },
		     { "Deutschland, Schweiz", "Deutschland, Schweiz" },
		     { "International A", "Belgien, Dänemark, Finnland, Frankreich, Großbritannien, Irland, Italien, Liechtenstein, Luxemburg, Monaco, Niederlande, Norwegen, Schweden, Slowenien, Slowakei, Spanien, Tschechien, Ungarn, Vatikan, Vereinigte Staaten (USA), Puerto Rico, Alaska, Hawaii, Kanada" },
		     { "International B", "Andorra, Bosnien-Herzegowina, Bulgarien, Estland, Griechenland, Island, Kroatien, Lettland, Litauen, Malta, Mazedonien, Polen, Portugal, Rumänien, Serbien, Montenegro, Zypern, Australien, Färöer, Israel, Neuseeland, Amerikanische Jungferninseln" },
		     { "International C", "Albanien, Weihnachtsinseln, Gibraltar, Kokosinseln, Rußland, San Marino, Türkei, Tunesien, Ukraine, Weißrußland, Algerien, Hongkong, Japan, Südkorea, Marokko, Philippinen, Russische Föderation, Singapur" },
		     { "International D", "Ägypten, Südafrika, Aserbaidschan, Armenien, China, Georgien, Libyen, Malaysia, Moldau, Tadschikistan, Taiwan, Turkmenistan, Usbekistan" },
		     { "International E", "Angola, Aruba, Bahamas, Bahrain, Barbados, Bermuda, Bhutan, Bolivien, Botswana, Brunei, Burundi, Ghana, Grönland, Guadeloupe, Guam, Guinea, Martinique, Französisch-Guayana, Honduras, Indien, Indonesien, Iran, Jordanien, Kasachstan, Kirgisistan, Kuba, Kuwait, Lesotho, Libanon, Liberia, Macao, Malawi, Martinique, Mayotte, Namibia, Niederländische Antillen, Nigeria, Paraguay, Reunion, Saipan, Saudi-Arabien, St. Lucia, St. Pierre und Miquelon, Saint Vincent und die Grenadinen, Sudan, Syrien, Swasiland, Tansania, Thailand, Trinidad und Tobago, Uruguay, Vereinigte Arabische Emirate" },
		     { "International F", "+" }};
  
  double Tarif[][2] = {{ 0.98, 0.88},
		       { 1.28, 1.18},
		       { 2.48, 1.48},
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
  rprintf ("International D: Zentral- und Südamerika nicht implementiert", "C:Fixme:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/1", Tarif[z]);
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
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk A1, max", "0663,0664,0676" },
		     { "Mobilfunk One", "0699" },
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
    rprintf ("CyberTron", "P:29,%d", t);
    sprintf (s, "%s (ATS %.3f / Impuls)", Name[t], Einheit[t]);
    rprintf (s, "C:Tarif:");
    rprintf ("Internationale Zonen nicht implementiert!", "C:Fixme:");

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
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
		     { "Ausland", "+" }};
  
  double Tarif[][2] = {{ 1.20, 1.20 },
		       { 2.16, 2.70 },
		       { 4.20, 4.20 },
		       { 0.00, 0.00 }};
  
  int z;
  
  printf ("\n");
  rprintf ("Citykom", "P:44");
  rprintf ("internationale Zonen nicht implementiert", "C:Fixme:");
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
		     { "Fernzone 2", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
		     { "Europa 1", "Deutschland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Europa 2", "Albanien, Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark,Finnland, Frankreich, Großbritannien, Nordirland, Irland, Jugoslawien, Serbien, Montenegro, Kroatien, Luxemburg, Malta, Mazedonien, Monaco, Niederlande, Norwegen, Rumänien, San Marino, Schweden, Spanien, Ukraine, Vatikanstadt, Zypern" },
		     { "Global 1", "Algerien, Estland, Färöer-Inseln, Georgien, Gibraltar, Island, Israel, Lettland, Litauen, Marokko, Portugal, Russische Föderation, Tadschikistan, Turkmenistan, Usbekistan" },
		     { "Global 2", "Antarktis, Armenien, Australien, Aserbaidschan, Weihnachtsinseln, Kokosinseln" },
		     { "Global 3", "Chile, Französisch-Guayana, Guadeloupe, Hongkong, Libyen, Martinique, Mayotte, Moldau, Neuseeland, Reunion, St. Pierre und Miquelon, Südafrika" },
		     { "Global 4", "Japan, Südkorea, Malaysia, Niederländische Antillen, Philippinen, Saipan" },
		     { "Global 5", "Angola, Bermuda, Bhutan, Brasilien, China, Ecuador, Iran, Macao, Mexiko, Saudi-Arabien, Venezuela" },
		     { "Global 6", "Ägypten, Äquatorial-Guinea, Aruba, Bahamas, Barbados, Belize, Botsuana, Brunei, Burundi, Dominikanische Republik, Gabun, Ghana, Grönland, Guinea-Bissau, Guyana, Jordanien, Kirgisistan, Kolumbien, Kuwait, Libanon, Panama, Singapur, Sao Tome und Principe, Saint Vincent und die Grenadinen, Trinidad und Tobago, Vereinigte Arabische Emirate" },
		     { "Global 7", "Bahrain, Cote d'Ivoire, Guinea, Kasachstan, Katar, Laos, Lesotho, Liberia, Malawi, Namibia, Nauru, Nepal, Neukaledonien, Nigeria, Norfolk-Inseln, Peru, Saint Helena, Saint Lucia, Samoa, Sudan, Swasiland, Syrien, Tansania, Tonga, Turks- und Caicos-Inseln, Sambia, Simbabwe" },
		     { "Global 8", "Argentinien, Äthiopien, Benin, Costa Rica, Fidschi, Französisch-Polynesien, Gambia, Irak, Jamaika, Kenia, Kiribati, Komoren, Malediven, Mauretanien, Mongolei, Mosambik, Myanmar, Nicaragua, Niue, Oman, Ruanda, Suriname, Taiwan, Thailand, Togo, Uganda, Vanuatu, Zentralafrikanische Republik" },
		     { "Global 9", "Bolivien, Caymaninseln, Dschibuti, Indien, Indonesien, Britische Jungferninseln, Nordkorea, Kuba, Mauritius, Niger, Papua-Neuguinea, Paraguay, Sierra Leone, Sri Lanka, Uruguay" },
		     { "Global 10", "Anguilla, Antigua und Barbuda, Ascension, Bangladesch, Burkina Faso, Dominica, El Salvador, Eritrea, Falklandinseln, Grenada, Guam, Guatemala, Haiti, Honduras, Jemen, Kamerun, Kap Verde, Kambodscha, Kongo, Mali, Montserrat, Pakistan, Saint Kitts und Nevis, Senegal, Seychellen, Salomonen, Somalia, Tschad, Vietnam, Zaire" },
		     { "Global 11", "Cook-Inseln, Madagaskar" },
		     { "Global 12", "Amerikanische Jungferninseln, Kanada, Puerto Rico, Vereinigte Staaten (USA)" },
		     { "Global 13", "Griechenland, Polen, Tunesien, Türkei" },
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
		     { "Österreich", "Österreich" },
		     { "Mobilfunk", "0663,0664,0676,0699" },
		     { "Euro 1", "Deutschland, Frankreich, Großbritannien, Nordirland, Italien, Liechtenstein, Schweiz, Slowakei, Slowenien, Tschechien, Ungarn" },
		     { "Euro 2", "Andorra, Belarus, Belgien, Bosnien-Herzegowina, Bulgarien, Dänemark, Estland, Finnland, Griechenland, Irland, Jugoslawien, Kroatien, Luxemburg, Monaco, Niederlande, Norwegen, Polen, Portugal, Rumänien, San Marino, Schweden, Spanien, Tunesien, Türkei, Ukraine, Vatikanstadt, Zypern" },
		     { "Euro City", "" }, /* Amsterdam, Brüssel, Frankfurt, London, Luxemburg, Mailand, Paris, Zürich */ 
		     { "Welt 1", "Hawaii, Amerikanische Jungferninseln, Puerto Rico, Vereinigte Staaten (USA), Kanada" },
		     { "Welt 2", "Albanien, Antarktis, Armenien, Aserbaidschan, Australien, Bahamas, Chile, Weihnachtsinseln, Kokosinseln, Dominikanische Republik, Georgien, Gibraltar, Guadeloupe, Französisch-Guayana, Hongkong, Island, Israel, Japan, Lettland, Libyen, Litauen, Mazedonien, Malaysia, Malta, Marokko, Martinique, Mayotte, Moldau, Neuseeland, Niederländische Antillen, Philippinen, Russische Föderation, Reunion, St. Pierre und Miquelon, Singapur, Südafrika, Südkorea, Saipan" },
		     { "Welt 3", "Ägypten, Angola, Argentinien, Aruba, Ascension, Bahrain, Barbados, Belize, Benin, Bermuda, Bhutan, Bolivien, Botsuana, Brasilien, Brunei, Burkina Faso, Burundi, Caymaninseln, China, Costa Rica, Dschibuti, Dominica, Ecuador, El Salvador, Färöer-Inseln, Falklandinseln, Fidschi, Französisch-Polynesien, Gabun, Gambia, Ghana, Grenada, Grönland, Guatemala, Haiti, Honduras, Indien, Indonesien, Iran, Jamaika, Jordanien, Katar, Kenia, Kirgisistan, Kolumbien, Nordkorea, Kongo, Kuba, Kuwait, Laos, Lesotho, Libanon, Liberia, Macau, Malawi, Malediven, Marshallinseln, Mauretanien, Mauritius, Mexiko, Montserrat, Nepal, Neukaledonien, Nicaragua, Nigeria, Norfolk-Inseln, Panama, Paraguay, Peru, Ruanda, Sambia, Saudi-Arabien, Salomonen, Simbabwe, Somalia, St. Helena, Saint Kitts und Nevis, St. Lucia, Saint Vincent und die Grenadinen, Sudan, Syrien, Tadschikistan, Tansania, Taiwan, Thailand, Trinidad und Tobago, Turkmenistan, Uruguay, Usbekistan, Vanuatu, Venezuela, Vereinigte Arabische Emirate, Samoa" },
		     { "Welt 4", "Afghanistan, Algerien, Anguilla, Antigua und Barbuda, Äquatorial-Guinea, Äthiopien, Bangladesch, Cook-Inseln, Cote d'Ivoire, Diego Garcia, Eritrea, Guam, Guantanamo, Guinea, Guinea-Bissau, Guyana, Irak, Jemen, Kambodscha, Kamerun, Kap Verde, Kasachstan, Kiribati, Komoren, Madagaskar, Mali, Midway-Inseln, Mikronesien, Mongolei, Mosambik, Myanmar, Namibia, Nauru, Niger, Niue, Oman, Pakistan, Palau, Papua-Neuguinea, Pitcairn-Inseln, Sao Tome und Principe, Amerikanisch-Samoa, Samoa, Senegal, Seychellen, Sierra Leone, Sri Lanka, Suriname, Swasiland, Togo, Tonga, Tschad, Turks- und Caicos-Inseln, Tuvalu, Uganda, Vietnam, Wake-Inseln, Zaire, Zentralafrikanische Republik" }};
  
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
  rprintf ("Euro City nicht implementiert", "C:Fixme:");
  for (z=0; z<COUNT(Zone); z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    if (Tarif[z][0]==Tarif[z][1]) {
      rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/%.2f/1", Tarif[z][0], 18.0/Tarif[z][0]);
    } else {
	rprintf ("Tag", "T:1-5/8-18=%.2f(60)/%.2f/1", Tarif[z][0], 18.0/Tarif[z][0]);
	rprintf ("Nacht", "T:1-5/18-8=%.2f(60)/%.2f/1", Tarif[z][1], 18.0/Tarif[z][1]);
	rprintf ("Weekend", "T:E,H/*=%.2f(60)/%.2f/1", Tarif[z][1], 18.0/Tarif[z][1]);
    }
    print_area(Zone[z][1]);
  }
}

void rate_1069(void) {

  char *Zone[][2] = {{ "Festnetz", "Österreich" }, 
		      { "ONE Mobilfunknetz", "0699" },
		      { "andere Mobilfunknetze", "0663,0664,0676" },
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
  for (z=0; z<7; z++) {
    rprintf (Zone[z][0], "Z:%d", z+1);
    rprintf ("rund um die Uhr", "T:*/*=%.2f(60)/30", Tarif[z]);
    print_area(Zone[z][1]);
  }
}    


void main (int argc, char *argv[])
{
  initCountry ("../countries-at.dat");
  initCountry ("../countries-de.dat");
  initCountry ("../countries-us.dat");

  printf ("# created by rate-at.c\n\n");
  printf ("# The information herein was machine-generated,\n");
  printf ("# so do not contribute patches to this file.\n\n");
  printf ("# Please contact Michael Reinelt <reinelt@eunet.at>\n");
  printf ("# if you have any corrections or additions.\n\n");
  printf ("# Many thanks to Daniela Bruder <dbruder@sime.com>\n");
  printf ("# for collecting and preparing most of the call charges.\n\n\n");

  printf ("V:1.2-Austria [19-Apr-1999]\n");

#if 0
  rate_1066();
#else
  rate_1001();
  rate_1002();
  rate_1003();
  rate_1005();
  rate_1007();
  rate_1012();
  rate_1024();
  rate_1029();
  rate_1044();
  rate_1066();
  rate_1069();
#endif

}

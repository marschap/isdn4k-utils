#include <stdio.h>
#include <stdlib.h>

char *Name[5] = { "Minimumtarif",
		  "Standartarif",
		  "Geschäftstarif 1",
		  "Geschäftstarif 2",
		  "Geschäftstarif 3" };

double Tarif[5] = { 1.116,
		    1.056,
		    0.996,
		    0.936,
		    0.816 };

char *Zone[30] = { "Regionalzone",
		   "Fernzone 1",
		   "Fernzone 2",
		   "Online-Tarif",
		   "Mobilfunk",
		   "Ausland Zone 1",
		   "Ausland Zone 2",
		   "Ausland Zone 3",
		   "Ausland Zone 4",
		   "Ausland Zone 5",
		   "Ausland Zone 6",
		   "Ausland Zone 7",
		   "Ausland Zone 8",
		   "Ausland Zone 9",
		   "Ausland Zone 10",
		   "Ausland Zone 11",
		   "Ausland Zone 12",
		   "Ausland Zone 13",
		   "Ausland Zone 14",
		   "Ausland Zone 15",
		   "Handvermittelter Verkehr",
		   "Grenznahverkehr",
		   "Teleinfo 04570",
		   "Teleinfo 04500 ",
		   "Telebusiness 04590",
		   "Teleinfo 04580",
		   "Businessline 0711x",
		   "Businessline 0713x",
		   "Businessline 0714x",
		   "Votingline 0717x" };

/* Einheiten in 72 sec */

double Faktor [30][4] = {{  1.25,  1.00,  0.66,  0.45 },  /* Regionalzone bis 50 km */
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


void main (void)
{
  int t, z;

  printf ("# created by rate-pta.c\n\n");
  printf ("V:1.0-Austria [11-Mar-1999]\n");

  for (t=0; t<4; t++) {
    printf ("\nP:00,%d\t\t\t\tPTA\n", t+1);
    printf ("C:Name:\t\t\t\tPost & Telekom Austria\n");
    printf ("C:Tarif:\t\t\t%s (ATS %.3f / Impuls)\n", Name[t], Tarif[t]);
    for (z=0; z<30; z++) {
      printf ("\nZ:%d\t\t\t\t%s\n", z+1, Zone[z]);
      if (Faktor[z][2]) printf ("T:1-5/06-08,18-20=%.3f/%.5g\tSparzeit\n", Tarif[t], 72.0/Faktor[z][2]);
      if (Faktor[z][0]) printf ("T:1-5/08-12=%.3f/%.5g\t\tTageszeit 1\n", Tarif[t], 72.0/Faktor[z][0]);
      if (Faktor[z][0]) printf ("T:1-4/13-16=%.3f/%.5g\t\tTageszeit 1\n", Tarif[t], 72.0/Faktor[z][0]);
      if (Faktor[z][1]) printf ("T:1-4/12-13,16-18=%.3f/%.5g\tTageszeit 2\n", Tarif[t], 72.0/Faktor[z][1]);
      if (Faktor[z][1]) printf ("T:5/12-18=%.3f/%.5g\t\tTageszeit 2\n", Tarif[t], 72.0/Faktor[z][1]);
      if (Faktor[z][2]) printf ("T:E,H/06-20=%.3f/%.5g\t\tSparzeit\n", Tarif[t], 72.0/Faktor[z][2]);
      if (Faktor[z][3]) printf ("T:*/20-06=%.3f/%.5g\t\tSupersparzeit\n", Tarif[t], 72.0/Faktor[z][3]);
    }
  }    

}

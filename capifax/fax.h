#ifndef _fax_h_
#define _fax_h_

/* FAX Resolutions */
#define FAX_STANDARD_RESOLUTION 0
#define FAX_HIGH_RESOLUTION     1

/* FAX Formats */
#define FAX_SFF_FORMAT                  0
#define FAX_PLAIN_FORMAT                1
#define FAX_PCX_FORMAT                  2
#define FAX_DCX_FORMAT                  3
#define FAX_TIFF_FORMAT                 4
#define FAX_ASCII_FORMAT                5
#define FAX_EXTENDED_ASCII_FORMAT       6
#define FAX_BINARY_FILE_TRANSFER_FORMAT 7

typedef struct fax3proto3 {
	unsigned char len;
	unsigned short resolution __attribute__ ((packed));
	unsigned short format __attribute__ ((packed));
	unsigned char Infos[100] __attribute__ ((packed));
} B3_PROTO_FAXG3;

void SetupB3Config(B3_PROTO_FAXG3 *B3conf, int FAX_Format);

#endif	/* _fax_h_ */

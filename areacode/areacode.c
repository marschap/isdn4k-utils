/*****************************************************************************/
/*                                                                           */
/*                                AREACODE.C                                 */
/*                                                                           */
/*     Portable library module to search for an area code in a database.     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1996,97  Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.com                                            */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any express or implied         */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



/*
 * The code assumes
 *      - 8 bit bytes
 *      - unsigned long is 32 bit. This may be changed by #defining u32 to
 *        a data type that is an 32 bit unsigned when compiling this module.
 *      - ascii character set
 *
 * The code does *not* assume
 *      - a specific byte order. Currently the code autoadjusts to big or
 *        little endian data. If you have something more weird than that,
 *        you have to add conversion code.
 *
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "areacode.h"



/*****************************************************************************/
/*                          Externally visible data                          */
/*****************************************************************************/



/* The name of the areacode data file. The default is what is #defined as
 * DATA_FILENAME. If this is not #defined, the default is "areacode.dat",
 * which is probably not what you want. In the latter case set this to
 * the correct filename *before* your first call to GetAreaCodeInfo.
 */
#ifdef DATA_FILENAME
char* acFileName = DATA_FILENAME;
#else
char* acFileName = "areacode.dat";
#endif

/* How much dynamic memory is GetAreaCodeInfo allowed to consume? Having less
 * memory means more disk access and vice versa. The function does even work
 * if you set this value to zero. For maximum performance, the function needs
 * 4 byte per area code stored in the data file. The default is 32KB.
 */
unsigned long   acMaxMem        = 0x8000L;



/*****************************************************************************/
/*                            Data and structures                            */
/*****************************************************************************/



/* Define an unsigned quantity with 32 bits. Try to make some clever
 * assumptions using the data from limits.h. This may break some older
 * (non ISO compliant) compilers, but I can't help...
 */
#if !defined(u32) && defined(ULONG_MAX)
#  if ULONG_MAX == 4294967295UL
#    define u32             unsigned long
#  endif
#endif
#if !defined(u32) && defined(UINT_MAX)
#  if UINT_MAX == 4294967295UL
#    define u32             unsigned
#  endif
#endif
#if !defined(u32) && defined(USHRT_MAX)
#  if USHRT_MAX == 4294967295UL
#    define u32             unsigned short
#  endif
#endif
#if !defined(u32)
#  define u32               unsigned long
#endif

/* The version of the data file we support (major only, minor is ignored) */
#define acVersion       0x100

/* The magic words in little and big endian format */
#define LittleMagic     0x35465768L
#define BigMagic        0x68574635L

/* Defining the byte ordering */
#define boLittleEndian  0
#define boBigEndian     1

/* The byte order used in the file is little endian (intel) format */
#define FileByteOrder   boLittleEndian

/* This is the header data of the data file. It is not used anywhere in
 * the code, just have a look at it since it describes the layout in the
 * file.
 */
typedef struct {
    u32         Magic;
    u32         Version;            /* Version in hi word, build in lo word */
    u32         Count;
    u32         AreaCodeStart;
    u32         NameIndexStart;
    u32         NameStart;
    u32		AreaCodeLenStart;   /* Version 1.02 and higher */
} PrefixHeader;

/* This is what's really used: */
typedef struct {

    /* The file we read from */
    FILE*       F;

    /* Machine byte order */
    unsigned    ByteOrder;

    /* Stuff from the file header */
    unsigned    Version;
    unsigned    Build;
    u32         Count;
    u32         AreaCodeStart;
    u32         NameIndexStart;
    u32         NameStart;
    u32		AreaCodeLenStart;

    /* Control data */
    long        First;
    long        Last;
    u32*        Table;

} AreaCodeDesc;

/* Translation table for translation CP850 --> ISO-8859-1. To save some space,
 * the table covers only values > 127
 */
#ifdef CHARSET_ISO
static char ISOMap [128] = {
    0xC7, 0xFC, 0xE9, 0xE2, 0xE4, 0xE0, 0xE5, 0xE7,
    0xEA, 0xEB, 0xE8, 0xEF, 0xEE, 0xEC, 0xC4, 0xC5,
    0xC9, 0xE6, 0xC6, 0xF4, 0xF6, 0xF2, 0xFC, 0xF9,
    0xFF, 0xD6, 0xDC, 0xA2, 0xA3, 0xA5, 0x50, 0x66,
    0xE1, 0xED, 0xF3, 0xFA, 0xF1, 0xD1, 0xAA, 0xBA,
    0xBF, 0x2D, 0xAC, 0xC6, 0xBC, 0xA1, 0xAB, 0xBB,
    0xFE, 0xFE, 0xFE, 0x7C, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x7C, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2D, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2D, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B, 0x2B,
    0x2B, 0x2B, 0x2B, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,
    0x61, 0xDF, 0x63, 0x70, 0x5A, 0x73, 0xB5, 0x74,
    0x70, 0x54, 0x4F, 0x64, 0x38, 0x30, 0x65, 0x55,
    0x3D, 0xB1, 0x3E, 0x3C, 0x66, 0x4A, 0xF7, 0x7E,
    0xB0, 0xB7, 0xB7, 0x2F, 0x6E, 0xB2, 0xFE, 0xFF
};
#endif

/* Macro to convert from big endian to little endian format and vice versa.
 * Beware: The macro evaluates its parameter more than once!
 */
#define _ByteSwap(__V) ((((__V) & 0x000000FF) << 24) |  \
                        (((__V) & 0xFF000000) >> 24) |  \
                        (((__V) & 0x0000FF00) <<  8) |  \
                        (((__V) & 0x00FF0000) >>  8))



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static u32 _ByteSwapIfNeeded (u32 D, unsigned ByteOrder)
/* Put the bytes into the correct order according to ByteOrder */
{
    /* Swap bytes if needed and return the result */
    switch (ByteOrder) {
        case boLittleEndian:    return D;
        default:                return _ByteSwap (D);
    }
}



static u32 ByteSwapIfNeeded (u32 D, const AreaCodeDesc* Desc)
/* Put the bytes into the correct order according to ByteOrder in Desc */
{
    /* Swap bytes if needed and return the result */
    return _ByteSwapIfNeeded (D, Desc->ByteOrder);
}



static u32 _Load_u32 (FILE* F, unsigned ByteOrder)
/* Load an u32 from the current file position and swap it if needed */
{
    u32 D;

    /* Read the data from the file */
    fread (&D, sizeof (D), 1, F);

    /* Swap bytes if needed and return the result */
    return _ByteSwapIfNeeded (D, ByteOrder);
}



static u32 Load_u32 (const AreaCodeDesc* Desc)
/* Load an u32 from the current file position and swap it if needed */
{
    return _Load_u32 (Desc->F, Desc->ByteOrder);
}



static unsigned LoadFileHeader (AreaCodeDesc* Desc)
/* Load the header of a data file. Return one of the acXXX codes. */
{
    u32 Version;

    /* Load the magic word in the format used int the file (do not convert) */
    u32 Magic = _Load_u32 (Desc->F, FileByteOrder);

    /* Check what we got from the file, determine the byte order */
    switch (Magic) {

        case BigMagic:
            Desc->ByteOrder = boBigEndian;
            break;

        case LittleMagic:
            Desc->ByteOrder = boLittleEndian;
            break;

        default:
            /* OOPS - the file is probably not a valid data file */
            return acInvalidFile;

    }

    /* Now read the rest of the header data */
    Version               = Load_u32 (Desc);
    Desc->Version         = (Version >> 16);
    Desc->Build           = (Version & 0xFFFF);
    Desc->Count           = Load_u32 (Desc);
    Desc->AreaCodeStart   = Load_u32 (Desc);
    Desc->NameIndexStart  = Load_u32 (Desc);
    Desc->NameStart       = Load_u32 (Desc);
    if (Desc->Version >= 0x101) {
	/* Beginning with version 1.01 we have an additional table that is
	 * ignored by older versions.
	 */
	Desc->AreaCodeLenStart = Load_u32 (Desc);
    }

    /* Check for some error conditions */
    if (ferror (Desc->F)) {
        /* Some sort of file problem */
        return acFileError;
    } else if (feof (Desc->F) || Desc->Count == 0) {
        /* This should not happen on a valid file */
        return acInvalidFile;
    } else if ((Desc->Version & 0xFF00) != acVersion) {
        return acWrongVersion;
    } else {
        /* Data is sane */
        return acOk;
    }
}



static u32 EncodeNumber (const char* Phone)
/* Encode the number we got from the caller into the internally used BCD
 * format.
 */
{
    unsigned I;
    unsigned Len;
    u32 P = 0;          /* Initialize to make gcc happy */

    /* Get the amount of characters to convert */
    Len = strlen (Phone);
    if (Len > 8) {
        Len = 8;
    }

    /* Convert the characters */
    for (I = 0; I < Len; I++) {
        P = (P << 4) | ((unsigned) ((unsigned char) Phone [I]) & 0x0F);
    }

    /* Fill the rest of the number with 0x0F */
    I = 8 - Len;
    while (I--) {
        P = (P << 4) | 0x0F;
    }

    /* Done - return the result */
    return P;
}



static u32 ReadPhone (const AreaCodeDesc* Desc, long Index)
/* Read the phone number that is located at the given index. If we have a
 * part of the table already loaded into memory, use the memory copy, else
 * read the phone number from disk.
 */
{
    if (Desc->Table && Index >= Desc->First && Index <= Desc->Last) {
        /* Use the already loaded table, but don't forget to swap bytes */
        return ByteSwapIfNeeded (Desc->Table [Index - Desc->First], Desc);
    } else {
        /* Load the value from the file */
        fseek (Desc->F, Desc->AreaCodeStart + Index * sizeof (u32), SEEK_SET);
        return Load_u32 (Desc);
    }
}



static void LoadTable (AreaCodeDesc* Desc)
/* Load a part of the table into memory */
{
    u32 SpaceNeeded = (Desc->Last - Desc->First + 1) * sizeof (u32);
    Desc->Table = malloc (SpaceNeeded);
    if (Desc->Table == 0) {
        /* Out of memory. There is no problem with this now since we do
         * not really need the table in core memory (it speeds things up,
         * that's all). In addition to that, the memory requirement halves
         * with each iteration, so maybe we have more luck next time.
         */
        return;
    }

    /* Seek to the correct position in the file */
    fseek (Desc->F, Desc->AreaCodeStart + Desc->First * sizeof (u32), SEEK_SET);

    /* Read the data */
    fread (Desc->Table, SpaceNeeded, 1, Desc->F);
}



static unsigned char CalcCodeLen (u32 Code)
/* Calculate the length of a given (encoded) area code in characters */
{
    u32 Mask;
    unsigned char Len = 0;
    for (Mask = 0xF0000000L; Mask; Mask >>= 4) {
        if ((Code & Mask) != Mask) {
            Len++;
        } else {
            break;
        }
    }

    return Len;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned GetAreaCodeInfo (acInfo* AC, const char* PhoneNumber)
/* Return - if possible - an information for the area code of the given number.
 * The function returns one of the error codes defined in areacode.h. If the
 * returned value is acOk, the AC struct is filled with the data of the
 * area code found. If we did not have an error, but there is no area code
 * that corresponds to the given number, the function returns acOk, but the
 * AC struct is filled with an empty Info field and a AreaCodeLen of zero.
 */
{
    u32           Phone;                /* PhoneNumber encoded in BCD */
    long          First, Last, Current; /* For binary search */
    u32           CurrentVal;           /* The value at Table [Current] */
    unsigned char AreaCodeLen;          /* The length of the area code found */
    unsigned char InfoLen;              /* Length of info string */
    unsigned      RC = acOk;            /* Result code of the function */
    u32           Mask;
    AreaCodeDesc  Desc;


    /* Clear the fields of the AC struct. Write a zero to the last field of
     * Info - this field is never written to by the rest of the code. So by
     * setting this to zero, we will assure a terminated string in case some
     * problem prevents the code below from executing correctly.
     */
    AC->Info [0]  = '\0';
    AC->Info [sizeof (AC->Info) - 1] = '\0';
    AC->AreaCodeLen = 0;

    /* If the number is empty, return immidiately */
    if (strlen (PhoneNumber) == 0) {
        return acOk;
    }

    /* Open the database file, check for errors */
    Desc.F = fopen (acFileName, "rb");
    if (Desc.F == 0) {
        /* We had an error opening the file */
        return acFileError;
    }

    /* Initialize descriptor data where needed */
    Desc.Table = 0;

    /* Read the header from the file */
    RC = LoadFileHeader (&Desc);
    if (RC != acOk) {
        /* Wrong file or file read error */
        goto ExitWithClose;
    }

    /* Convert the phone number into the internal representation */
    Phone = EncodeNumber (PhoneNumber);

    /* Add dead code to work around gcc warnings */
    Current    = 0;
    CurrentVal = 0;

    /* Now do a binary search over the data */
    First   = 0;
    Last    = (long) Desc.Count - 1;
    while (First <= Last) {

        /* If we don't have read the table into memory, check if we can do
         * so now.
         */
        if (Desc.Table == 0) {
            u32 NeedMemory = (Last - First + 1) * sizeof (u32);
            if (NeedMemory <= acMaxMem) {
                /* Ok, the current part of the table is now small enough to
                 * load it into memory.
                 */
                Desc.First = First;
                Desc.Last  = Last;
                LoadTable (&Desc);
            }
        }

        /* Set current to mid of range */
        Current = (Last + First) / 2;

        /* Get the phone number from that place */
        CurrentVal = ReadPhone (&Desc, Current);

        /* Do a compare */
        if (Phone > CurrentVal) {
            First = Current + 1;
        } else {
            Last = Current - 1;
            if (Phone == CurrentVal) {
                /* Set the condition to terminate the loop */
                First = Current;
            }
        }
    }

    /* First is the index of the area code, we eventually found. Put the index
     * into Current and the value into CurrentVal.
     */
    if (Current != First) {
        Current = First;
        CurrentVal = ReadPhone (&Desc, Current);
    }

    /*
     * We may now delete an eventually allocated table space since it is
     * not needed any more.
     */
    free (Desc.Table);
    Desc.Table = 0;

    /* If Current points behind Last, we did not find anything */
    if (Current >= (long) Desc.Count) {
        /* Not found */
        goto ExitWithClose;
    }

    /* Calculate the length of the area code */
    AreaCodeLen = CalcCodeLen (CurrentVal);

    /* Check if the Prefix is actually the first part of the phone number */
    Mask = 0xFFFFFFFFL << ((8 - AreaCodeLen) * 4);
    if ((Phone & Mask) != (CurrentVal & Mask)) {
        /* They are different */
        goto ExitWithClose;
    }

    /* Ok, we have now definitely found the code. Current is the index of the
     * area code. Seek to the corresponding position in the name index, get
     * the name position from there and seek to that place.
     */
    fseek (Desc.F, Desc.NameIndexStart + Current * sizeof (u32), SEEK_SET);
    fseek (Desc.F, Desc.NameStart + Load_u32 (&Desc), SEEK_SET);

    /* Read the length of the name and add the trailing zero to the info
     * field in the result struct.
     */
    fread (&InfoLen, 1, 1, Desc.F);
    AC->Info [InfoLen] = '\0';

    /* Read the info into the result struct */
    fread (AC->Info, 1, InfoLen, Desc.F);

#ifdef CHARSET_ISO
    /* Translate the info to the ISO-8859-1 charset */
    {
        unsigned I;
        for (I = 0; I < InfoLen; I++) {
            unsigned char C = (unsigned char) AC->Info [I];
            if (C >= 128) {
                AC->Info [I] = ISOMap [C - 128];
            }
        }
    }
#endif

    /* If the areacode file is version 1.01 or greater, there is an additional
     * table with the length of the "real" area code. Older versions use the
     * length of the area code. This enables dividing of number spaces, e.g.
     * 49212[0-8] = Solingen, 492129 = Haan. With the old data file, the
     * areacode of Solingen would be 492120 but the official code is just
     * 49212 which needs an additional length byte.
     */
    if (Desc.Version >= 0x101) {
	fseek (Desc.F, Desc.AreaCodeLenStart + Current, SEEK_SET);
	fread (&AreaCodeLen, 1, sizeof (AreaCodeLen), Desc.F);
    }
    AC->AreaCodeLen = AreaCodeLen;

ExitWithClose:
    /* Close the data file */
    fclose (Desc.F);

    /* Done, return the result */
    return RC;
}




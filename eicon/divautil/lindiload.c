
/*
 *
 * Copyright (C) Eicon Technology Corporation, 2000.
 *
 * This source file is supplied for the exclusive use with Eicon
 * Technology Corporation's range of DIVA Server Adapters.
 *
 * Eicon File Revision :    1.10  
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY OF ANY KIND WHATSOEVER INCLUDING ANY 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#include    <memory.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <errno.h>
#include    <fcntl.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    "sys.h"
#include    "pc_maint.h"
#include    "divas.h"
#include    "load.h"
#include    "diva.h"
#include    "diload.h"

#include    "linuxcfg.h"

byte read_dec(char *str,word *val);
byte read_hex(char *str,dword *val);
byte check_oa(char *str);
byte check_osa(char *str);
byte check_spid(char *str);
void usage(char *name);

dia_config_t options;
dia_card_t card;
int load_all_cards = FALSE;
int card_idl;

byte cards[20], num_cards;
byte new_cards[20], new_num_cards = 0;
byte real_card_id[20];
byte flavours[20];
char OSA[8][MAX_ADDR];
char OAD[8][MAX_ADDR];
char SPID[8][MAX_SPID];


char *flavour_str[] = { 
			"etsi",
			"ni",
			"5ess",
			"1tr6",
			"franc",
			"japan" };

int DivaALoad(char *dsp_name, dia_config_t *options, dia_card_t *card, char *msg , int adapter_instance);
static char *card_desc[] = {"DIVA Server PRI",
							"DIVA Server BRI",
							"DIVA Server 4-BRI"};

int main( int argc, char *argv[] )
{
    int rc=0;
    char ret_msg[80];
    char disp_msg[80];
    int i,opt;
    byte stablel2;
    word tei=0xff, channel_no=0, adapter_no=0;
    char c;
    int dev_id = 0;
    byte card_i;
    char afname[10] = "";
	byte byId;
	byte *pParamStr, *pOAD, *pOSA, *pSPID;
	dia_card_list_t *card_list;

	argc=0;

    opt=1;
    bzero(&options, sizeof(dia_config_t));
    bzero(&card, sizeof(dia_card_t));

    printf("divaload: Firmware loader for Eicon DIVA Server ISDN adapters\n");
    if ((dev_id = open(DIVAS_DEVICE, O_RDONLY, 0)) == -1)
    {
        fprintf(stderr, "Couldn't open %s\n", DIVAS_DEVICE);
        return 1;
    }


    if((rc = ioctl(dev_id, DIA_IOCTL_DETECT, &cards)) == -1)
    {
        fprintf(stderr, "%s DETECT Error %d\n", DIVAS_DEVICE, rc);
        (void)close(dev_id);
        return 1;
    }

    card_list = calloc(cards[0], sizeof(dia_card_list_t));


    if((rc = ioctl(dev_id, DIA_IOCTL_GET_LIST, card_list)) == -1)
    {
        fprintf(stderr, "%s COUNT Error %d\n", DIVAS_DEVICE, rc);
        (void)close(dev_id);
        return 1;
    }

    close(dev_id);

    num_cards = cards[0];

	{
        int i;

        for (i=1; i<=num_cards;)
        {
	    new_cards[new_num_cards+1] = cards[i];
	    real_card_id[new_num_cards+1] = i-1;

            if (cards[i] == 2) // 4BRI
            {
               i+=3;
            }
            new_num_cards++;
	    i++;
        }
    }

    num_cards = new_num_cards;


    if (num_cards)
	{
		printf("divaload: %d %s found\n", num_cards,
				num_cards > 1 ? "adapters" : "adapter");
	}
	else
	{
		printf("divaload: no adapters found\n");
	}

    while( argv[opt] )
    { 
        if( argv[opt][0]=='-')
        {
            c=argv[opt][1];
            opt++;
            if(c>='A' &&  c<='Z')
            { 
               c+='a'-'A';
            }

            switch(c)
            {
			case '1':
			case '2':
			case '3': // For 4BRI
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				byId = c - '1';
				opt--; /* Go back to -n??? argument to see if it was oad, osa or spid */

				pParamStr = argv[opt] + 2;	/* Set up pointer to paramater string - oad, osa, spid */

				if (strcmp(pParamStr, "oad") == 0)
				{
					pOAD = pParamStr + 4;
					if (check_oa(pOAD))
					{
							strcpy((char *)OAD[byId], pOAD);
					}
					else
					{
						fprintf(stderr, "Invalid OAD - %s\n", pOAD);
					}
				}
				else if (strcmp(pParamStr, "osa") == 0)
				{
					pOSA = pParamStr + 4;
					if (check_osa(pOSA))
					{
						strcpy((char *)OSA[byId], pOSA);
					}
					else
					{
						fprintf(stderr, "Invalid OSA - %s\n", pOSA);
					}
				}
				else if (strcmp(pParamStr, "spid") == 0)
				{
					pSPID = pParamStr + 5;
					if (check_spid(pSPID))
					{
						strcpy((char *)SPID[byId], pSPID);
					}
					else
					{
						fprintf(stderr, "Invalid SPID - %s\n", pSPID);
					}
				}
				else
				{
					fprintf(stderr, "Invalid sub-parameter: [%s]\n", pParamStr);
					return 1;
				}
				
				opt++; opt++;
				break;

            case 'e':
                options.crc4=TRUE;
                break;

			case 'a': // all?
				opt--;
				if ((argv[opt][2] == 'l') && (argv[opt][3] == 'l') && (argv[opt][4] == '\0') && (options.card_id == 0))
				{
					load_all_cards = TRUE;
				}
				else if (options.card_id != 0)
				{
					fprintf(stderr, "divaload: cannot specify both -all and -c\n");
				}
				else
				{
					fprintf(stderr, "divaload: invalid parameter [%s] (Use 'all')\n", argv[opt]);
					return 1;
				}
				opt++;
				break;

			case 'c': // Card number (starting from 1)
				if(read_dec(argv[opt],(word *) &adapter_no))
				{
            		fprintf(stderr, "divaload: invalid card number specified\n");
					return (1);
				}
				else if (load_all_cards)
				{
	 	    		fprintf(stderr, "divaload: -all specified. Cannot specify both -all and -c");
		    		return 1;
				}
				else if ((adapter_no < 1) || (adapter_no > num_cards))
           		{
            		if (num_cards > 1)
					{
						fprintf(stderr, "divaload: use value between 1 and %d inclusive\n", num_cards);
						return 1;
					}
					else
					{
						fprintf(stderr, "divaload: adapter numbers start at 1\n");
						return 1;
					}
				}
				opt++;
				break;

			case 'f': /* Switch configuration */
				if (!argv[opt])
				{
					fprintf(stderr, "divaload: No switch type specified\n");
					return (1);
				}
				switch((char)argv[opt][0])
				{
				case 'E':	/* ETSI */
				case 'e':
					strcpy(afname, "te_etsi");
					break;

				case 'A':	/* ATEL */
				case 'a':
					strcpy(afname, "te_atel");
					break;

				case 'J':	/* JAPAN */
				case 'j':
					strcpy(afname, "te_japan");
					break;

				case 'N':	/* NI1 */
				case 'n':
					strcpy(afname, "te_ni");
					break;

				case '5':	/* 5ESS */
					strcpy(afname, "te_5ess");
					break;
				case '1':	/* 5ESS */
					strcpy(afname, "te_1tr6");
					break;
				default:
					printf("divaload: Invalid switch type (%s)\n", argv[opt]);
					return (1);
					
				}
				opt++;
				break;

			case 'h':
                usage(argv[0]);
                return (1);
                break;

            case 'l':
                if(read_dec(argv[opt],(word*)&channel_no))
                {
                    printf("Invalid params for channel option.\n");
                    return (1);
                }
                else
                {
					if (channel_no > 30)
					{
                        printf("Invalid channel number (%d)!!\n", channel_no);
                        return (1);
					}
                    options.low_channel = channel_no;
                    opt++;
                }
                break;

            case 'n':
                options.nt2=TRUE;
                break;

            case 'o':
                options.no_order_check=TRUE;
                break;

            case 'p':
                options.permanent=TRUE;
                break;

            case 's':
                options.stable_l2=1;
                if(!argv[opt] || *argv[opt]=='-')
                {
                    break;
                }

                if(read_dec(argv[opt],(word*)&stablel2))
                {
                    printf("Invalid options for Layer2 operation.\n");
                    return (1);
                }
                else
                {
                    if(stablel2>3)
                    {
                        printf("Invalid options for Layer2 operation.\n");
                        return 1;
                    }
                    options.stable_l2=stablel2; 
                    opt++;
                }
                break;

            case 't':
                if(read_dec(argv[opt],(word*)&tei))
                {
                    printf("Invalid params for TEI option.\n");
                    return (1);
                }
                else
                {
                    if(tei>63)
                    { 
                        printf("Invalid tei number !!\n");
                        return (1);
                    }
                    options.tei = tei;
                    opt++;
                }
                break;

				/* Invalid parameter (on Linux anyway) */
			default:
				fprintf(stderr, "divaload: invalid parameter [%c]\n", c);
				return -1;
			}
		}
    }

	if (strlen(afname) == 0)
	{
		fprintf(stderr, "divaload: No switch type specified ('./divaload -h' for help)\n");
		return -1;
	}

    if (load_all_cards)
    {
		int num_adaps, qm;

        for (card_i = 1 ; card_i <= num_cards; card_i++)
        {
/*			announce_card(card_i, cards[card_i]);*/
			fprintf(stderr, "divaload: Loading %s with %s firmware\n", card_desc[cards[card_i]], afname + 3);
				
			options.card_id = card_i - 1;

			if (card_list[options.card_id].state == DIA_RUNNING)
			{
				fprintf(stderr, "divaload: adapter %d already running.\n", card_i);
				continue;
			}

            if (new_cards[card_i] == 0)
            {
/*				strcpy(afname, "te_");
				strcat(afname, flavour_str[flavours[card_i]]);*/
                strcat(afname, ".pm");

				num_adaps = 1;
            }
			else if (new_cards[card_i] == 1)
            {
/*				strcpy(afname, "te_");
				strcat(afname, flavour_str[flavours[card_i]]);*/
                strcat(afname, ".sm");

				num_adaps = 1;
            }
		else if (new_cards[card_i] == 2)
            {
                strcat(afname, ".qm ");

				num_adaps = 4;
            }
            else
            {
              printf("divaload: unknown card type %d\n", new_cards[card_i]);
                return -1;
            }
           
            /*rc=DivaALoad(afname, &options, &card, ret_msg);*/
			options.card_id = real_card_id[card_i];

			for (i=options.card_id, qm = 0; num_adaps; i++, num_adaps--, qm++)
			{
				options.card_id = i;

				if (new_cards[card_i] == 2) //4BRI
				{
					 afname[strlen(afname) - 1] = '0' + qm;
				}


				rc=DivaALoad(afname, &options, &card, ret_msg, num_adaps);

				if( rc != ERR_ETDD_OK )
				{
					switch(rc)
					{
					case ERR_ETDD_ACCESS:
						 sprintf(disp_msg,"cannot access file %s",ret_msg);
						 perror(disp_msg);
						 break;
 
					case ERR_ETDD_DSP:
						 sprintf(disp_msg,"cannot access DSP binary %s",ret_msg);
						 perror(disp_msg);
						 break;

					case ERR_ETDD_IOCTL:
						 sprintf(disp_msg,"error doing ioctl");
						 perror(disp_msg);
						 break;

					case ERR_ETDD_NOMEM:
						 printf("DivaLoad:Not enough memory available.\n");
						 break;

					case ERR_ETDD_OPEN:
						 sprintf(disp_msg,"Cannot open file %s",ret_msg);
						 perror(disp_msg);
						 break;

					case ERR_ETDD_READ:
						 sprintf(disp_msg,"Cannot read from file %s",ret_msg);
						 perror(disp_msg);
						 break;

					default:
						 printf("divaload: unknown error\n");
					}/*switch*/
				}/*if*/

			}/*Next Sub-Adapter*/

			if (strchr(afname, '.'))
			{
				*(strchr(afname, '.')) = '\0';
			}
        }/*Next real adapter*/
    }
    else
    {

		int num_adaps = 0, qm;

		if (adapter_no == 0) // No adapter specified
		{
			fprintf(stderr, "divaload: You need to specify either an adpater (-c n) or use -all\n");
			return -1;
		}

		fprintf(stderr, "divaload: Loading %s with %s firmware\n", card_desc[cards[adapter_no]], afname + 3);
		
		options.card_id = real_card_id[adapter_no];

		if (card_list[options.card_id].state == DIA_RUNNING)
		{
			fprintf(stderr, "divaload: adapter %d already running.\n", adapter_no);
			return -1;
		}


        //options.card_id = adapter_no - 1;

        if (new_cards[adapter_no] == 0)
        {
            strcat(afname, ".pm");
			num_adaps = 1;
        }
         else if (new_cards[adapter_no] == 1)
        {
            strcat(afname, ".sm");
			num_adaps = 1;
        }
        else if (new_cards[adapter_no] == 2)
        {
            strcat(afname, ".qm ");
            num_adaps = 4;
        }
        else
        {
            printf("divaload: unknown adapter type %d", new_cards[adapter_no]);
            return -1;
        }




		// Load num_adaps adapters (4BRI: num_adaps = 4)

		for (i=options.card_id, qm = 0; num_adaps; i++, num_adaps--, qm++)
		{
		options.card_id = i;

		// num_adaps serves as a trigger for download of the combifile
		// Only gets done (in DivaALoad) when num_adaps == 1 (i.e. last one)

			if (new_cards[adapter_no] == 2) // 4BRI
		{
			afname[strlen(afname) - 1] = '0' + qm;
		}

		strcpy((char *)options.terminal[0].oad, OAD[qm*2]);
		strcpy((char *)options.terminal[1].oad, OAD[(qm*2)+1]);

		strcpy((char *)options.terminal[0].osa, OSA[qm*2]);
		strcpy((char *)options.terminal[1].osa, OSA[(qm*2)+1]);

		strcpy((char *)options.terminal[0].spid, SPID[qm*2]);
		strcpy((char *)options.terminal[1].spid, SPID[(qm*2)+1]);

        rc=DivaALoad(afname, &options, &card, ret_msg, num_adaps);
	
        if( rc != ERR_ETDD_OK )
        {
            switch(rc)
            {
            case ERR_ETDD_ACCESS:
                 sprintf(disp_msg,"cannot access file %s",ret_msg);
                 perror(disp_msg);
                 break;
 
            case ERR_ETDD_DSP:
                 sprintf(disp_msg,"cannot access DSP binary %s",ret_msg);
                 perror(disp_msg);
                 break;

            case ERR_ETDD_IOCTL:
                 sprintf(disp_msg,"error doing ioctl");
                 perror(disp_msg);
                 break;

            case ERR_ETDD_NOMEM:
                 printf("divaload: not enough memory available.\n");
                 break;

            case ERR_ETDD_OPEN:
                 sprintf(disp_msg,"cannot open file %s",ret_msg);
                 perror(disp_msg);
                 break;

            case ERR_ETDD_READ:
                 sprintf(disp_msg,"cannot read from file %s",ret_msg);
                 perror(disp_msg);
                 break;

            default:
                 printf("divaload: unknown error\n");
            }
        }
		} // 'for' loop for loading 4BRI adapters
    }
    return(0);
}

/*
 * take in a decimal number from the keyboard
 */
byte read_dec(char *str,word *val)
{
    word c;
    *val=0;
    
    if(!str)
    {
        return TRUE;
    }

    c=*str; 
    while( *str && (c>='0' && c<='9'))
    {
        *val *= 10;
        *val += c-'0';
        str++;
        c=*str;
    }

    if( *str )
    {
        *val=0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*
 * take in a hex number from the keyboard
 */
byte read_hex(char *str,dword *val)
{
    word c;
    *val=0;
    
    if(!str)
    {
        return TRUE;
    }
	if(!strncmp(str, "0x", 2))
	{
		str++; 
		str++; 
	}
	c=*str; 
    while( *str && ((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F')))
    {
        *val *= 0x10;
		if (c>='0' && c<='9') 
		{
			*val += c-'0';
		}
		else
		{
	 		if (c>='a' && c<='f')
			{
				*val += c-'a';
				*val += 0xa;
			}
			else
			{
				*val += c-'A';
				*val += 0xa;
			}
		}
        str++;
        c=*str;
    }

    if( *str )
    {
        *val=0;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
 * validate the Originating Address string 
 */
byte check_oa(char *str)
{
    byte len=0;

    if(!*str)
    {
       return FALSE;
    }

    while(*str && len<MAX_ADDR && (*str>='0' && *str<='9'))
    {
        len++;
        str++;
    }
    if( *str )
    {
        return FALSE;
    }
    else
    {

        return TRUE;
    }
} 

/*
 * validate the Originating Sub-Address string 
 */
byte check_osa(char *str)
{
    byte len=0;

    if(!*str)
    {
       return FALSE;
    }

    while(*str && len<MAX_ADDR && ((*str>='0' && *str<='9')
          || *str=='P' || *str=='p' ))
    {
       len++;
       str++;
    }
    if( *str )
    {
        return FALSE;
    }
    else
    {

        return TRUE;
    }
} 

/*
 * validate the SPID string 
 */
byte check_spid(char *str)
{
    byte len=0;

    if(!*str)
    {
       return FALSE;
    }

    while(*str && len<MAX_SPID && (*str>='0' && *str<='9'))
    {
       len++;
       str++;
    }
    if( *str )
    {
        return FALSE;
    }
    else
    {

        return TRUE;
    }
} 
       
      
/*
 * Usage function 
 */
void usage(char *name)
{
    printf("%s usage:\n",name);
	printf("[-c <n>] [-all] [-f <switch type>] [-h] [-l <n>] [-n] [-o] [-p] [-s <0|1|2>] [-t n] [-<1|2><oad|osa|spid> value]\n");
	printf("Where:\n");
    printf("  -c n where n is the card to start\n");
	printf("  -all to start all cards\n");
	printf("  -e to specify CRC4\n");
	printf("  -f <switch type> where <switch type> can be ETSI ...\n");
	printf("  -h for usage\n");
	printf("  -l <n> where n is the starting channel number (between 0 and 30)\n");
	printf("     By default the allocation of channels is made on a high-to-low basis.\n");
	printf("     By specifying -l you select a low-to-high allocation policy (in addition\n");
	printf("     to specifying the starting channel number)\n");
	printf("  -n to select NT2 mode\n");
	printf("  -o to enforce order checking with information elements\n");
	printf("  -p to establish a permanent connection (e.g. leased line configuration)\n");
	printf("  -s <n> Layer 2 (D-Channel) connection type:\n");
	printf("     0 - Normal operation (only when there is an active B-channel connection)\n");
	printf("     1 - Never deactivated (from initial B-channel connection)\n");
	printf("     2 - Always active (regardless of B-channel connection status)\n");
	printf("  -t <n> specifies a fixed TEI value between 0 and 63. Default is a dynamic TEI assignment\n");
	printf("  -<1|2><oad|osa|spid> value (BRI only)\n");
	printf("     Specify the Originating Address (OAD), Originating Sub-address (OSA) and/or Service\n");
	printf("     Profile Identifier (SPID) for each B-channel (1 or 2)\n");
	printf("     Example:  divaload ... -1oad 123456 -1spid 1234560001\n");
}

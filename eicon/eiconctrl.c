/* $Id: eiconctrl.c,v 1.5 1999/03/29 11:05:23 armin Exp $
 *
 * Eicon-ISDN driver for Linux. (Control-Utility)
 *
 * Copyright 1998    by Fritz Elfert (fritz@wuemaus.franken.de)
 * Copyright 1998,99 by Armin Schindler (mac@melware.de)
 * Copyright 1999    Cytronics & Melware (info@melware.de)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 * $Log: eiconctrl.c,v $
 * Revision 1.5  1999/03/29 11:05:23  armin
 * Installing and Loading the old type Eicon ISA-cards.
 * New firmware in one tgz-file.
 * Commandline Management Interface.
 *
 * Revision 1.4  1999/03/02 11:35:57  armin
 * Change of email address.
 *
 * Revision 1.3  1999/02/25 22:35:14  armin
 * Did not compile with new version.
 *
 * Revision 1.2  1999/01/20 21:16:45  armin
 * Added some debugging features.
 *
 * Revision 1.1  1999/01/01 17:27:57  armin
 * First checkin of new created control utility for Eicon.Diehl driver.
 * diehlctrl is obsolete.
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <ncurses.h>


#include <linux/types.h>
#include <linux/isdn.h>
#include <eicon.h>


#define COMBIFILE "dspdload.bin"

#define MIN(a,b) ((a)>(b) ? (b) : (a))

#define EICON_CTRL_VERSION 1

char *cmd;
int verbose = 0;
eicon_manifbuf *mb;

WINDOW *statwin;
WINDOW *headwin;
static int h_line;
static int stat_y;

int num_directory_entries;
int usage_mask_size;
int download_count;
int directory_size;
unsigned int no_of_downloads = 0;
int total_bytes_in_download = 0;
__u32 download_pos;
t_dsp_download_desc p_download_table[35];

int usage_bit;
int usage_byte;
char *no_of_tables;
__u32 table_count = 0;
int fd;
isdn_ioctl_struct ioctl_s;

char Man_Path[80];
int  man_ent_count;

struct man_s {
	u_char type;
	u_char attribute;
	u_char status;
	u_char var_length;
	u_char path_length;
	char   Name[180];
	char   Var[180];
} man_ent[60];

char *spid_state[] =
{
  "Idle",
  "Up",
  "Disconnecting",
  "Connecting",
  "SPID Initializing",
  "SPID Initialized",
  "UNKNOWN STATE"
};



void usage() {
  fprintf(stderr,"usage: %s add <DriverID> <membase> <irq>              (add card)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] membase [membase-addr]      (get/set memaddr)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] irq   [irq-nr]              (get/set irq)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] [-v] load <protocol> [options]\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] debug [<debug value>]\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] manage [read|exec <path>]   (management-tool)\n",cmd);
  fprintf(stderr,"load firmware:\n");
  fprintf(stderr," basics  : -d <DriverID> ID defined when eicon module was loaded/card added\n");
  fprintf(stderr,"         : -v            verbose\n");
  fprintf(stderr," options : -l[channel#]  channel allocation policy\n");
  fprintf(stderr,"         : -c[1|2]       CRC4 Multiframe usage [off|on]\n");
  fprintf(stderr,"         : -tTEI         use the fixed TEI\n");
  fprintf(stderr,"         : -h            no HSCX 30 mode (PRI only)\n");
  fprintf(stderr,"         : -n            NT2 mode\n");
  fprintf(stderr,"         : -p            leased line D channel\n");
  fprintf(stderr,"         : -s[1|2]       LAPD layer 2 session strategy\n");
  fprintf(stderr,"         : -o            allow disordered info elements\n");
  fprintf(stderr,"         : -z            switch to loopback mode\n");
  exit(-1);
}


/*--------------------------------------------------------------
 * display_combifile_details()
 *
 * Displays the information in the combifile header
 * Arguments: Pointer to the combifile structure in memory
 * Returns:   The address of the begining of directory which is
 *            directly after the file description.
 *-------------------------------------------------------------*/

t_dsp_combifile_directory_entry *display_combifile_details(char *details)
{
        __u32 offset=0;
        t_dsp_combifile_header *file_header;
        char *description;
        t_dsp_combifile_directory_entry *return_ptr = NULL;

        file_header = (t_dsp_combifile_header *)details;

        printf("%s\n", file_header->format_identification);
#ifdef __DEBUGVAR__ 
        printf("\tFormat Version: 0x%.4x\n", file_header->format_version_bcd);
        printf("\tNumber of directory entries : %d\n", file_header->directory_entries);
        printf("\tDownload count: %d\n", file_header->download_count);
#endif

        description = (char *)file_header + (file_header->header_size);

        printf("%s\n", description);

        num_directory_entries = file_header->directory_entries;
        usage_mask_size = file_header->usage_mask_size;
        download_count = file_header->download_count;
        directory_size = file_header->directory_size;

        return_ptr = (t_dsp_combifile_directory_entry *) (unsigned int)file_header ;
        offset  += (file_header->header_size);
        offset  += (file_header->combifile_description_size);
        offset += (__u32)return_ptr;

        return (t_dsp_combifile_directory_entry *)offset;

}


__u32 store_download(char *data, __u16 size, char *store)
{
        __u16 real_size;
        static int first = 1;
        static char* position;
        static char* initial;
        static __u32 addr;
        __u32 data_start_addr;
        __u32 align;

#ifdef __DEBUGVAR__
        printf("Writing Data to memory block\n");
#endif
                if(first)
                {
                addr = 0xa03a0000 + (((sizeof(__u32)+sizeof(p_download_table)) + ~0xfffffffc) & 0xfffffffc);
                position = initial = (char *)store;
                first = 0;
                }

/*Starting address to where the data is written in the download_block*/
	data_start_addr = addr;

        real_size = size;

        align = ((addr + (real_size + ~0xfffffffc)) & 0xfffffffc) -
                        (addr + real_size);

        memcpy(position, data, real_size);

        position += real_size;
        addr += real_size;

        bzero(position, align);

        position += align;
        addr += align;

        total_bytes_in_download = position - initial;

#ifdef __DEBUGVAR__
        printf("total_bytes written so far is 0x%x\n",total_bytes_in_download);
	printf("align value for this download is: %d\n",align);
#endif
        return (data_start_addr);
}



__u32 get_download(char *download_block, char *download_area)
{
        int n, i;
	char Text[100];
        char *usage_mask;
        char test_byte=0;
        __u32 length=0;
        __u32 addr;
    	unsigned int table_index;
        t_dsp_file_header *file_header;

        t_dsp_download_desc *p_download_desc;
        char *data;

	i=0;
        no_of_downloads = 0;
        test_byte = 0x01;
        test_byte <<= usage_bit;
        usage_mask = malloc(usage_mask_size);

        if(!usage_mask)
        {
                printf("Error allocating memory for usage mask");
                return 0;
        }
        bzero(usage_mask, usage_mask_size);

        for(n = 0; n < download_count; n++)
        {
                memcpy(usage_mask, download_area, usage_mask_size);
#ifdef __DEBUGVAR__ 
        printf("  \n");
                printf("Usage mask = 0x%.2x ", usage_mask[0]);
                if(usage_mask_size > 1)
                {
                        for(i=1; i<usage_mask_size; i++)
                        {
                                printf("0x%.2x ", usage_mask[i]);
                        }
                }
#endif
                download_area += usage_mask_size;

                file_header = (t_dsp_file_header *)download_area;
#ifdef __DEBUGVAR__
                printf("%s %d\n", file_header->format_identification,n);
#endif

                if( test_byte & usage_mask[usage_byte] )
                {
                no_of_downloads++;
	        table_index = (no_of_downloads - 1);

#ifdef __DEBUGVAR__
	            printf("*****DSP DOWNLOAD %d REQUIRED******\n", n);
        	    printf("download required count is now %d\n",no_of_downloads);
	            printf("   \n");
#endif


			/*This is the length of the memory to malloc */

                        length += ((__u32)((__u16)((file_header->header_size)
                                                - sizeof(t_dsp_file_header))))
                        + ((__u32)(file_header->download_description_size))
                        + ((__u32)(file_header->memory_block_table_size))
                        + ((__u32)(file_header->segment_table_size))
                        + ((__u32)(file_header->symbol_table_size))
                        + ((__u32)(file_header->total_data_size_dm))
                        + ((__u32)(file_header->total_data_size_pm));

                        if(download_block)
                        {
                        data = (char *)file_header;
                        data += ((__u32)(file_header->header_size));

                                p_download_desc = &(p_download_table[table_index]);
                                p_download_desc->download_id = file_header->download_id;
                                p_download_desc->download_flags = file_header->download_flags;
                                p_download_desc->required_processing_power = file_header->required_processing_power;
                                p_download_desc->interface_channel_count = file_header->interface_channel_count;
                                p_download_desc->excess_header_size = ((__u16)((file_header->header_size) - (__u16)sizeof(t_dsp_file_header)));
                                p_download_desc->memory_block_count = file_header->memory_block_count;
                                p_download_desc->segment_count = file_header->segment_count;
                                p_download_desc->symbol_count = file_header->symbol_count;
                                p_download_desc->data_block_count_dm = file_header->data_block_count_dm;
                                p_download_desc->data_block_count_pm = file_header->data_block_count_pm;

                                p_download_desc->p_excess_header_data = NULL;
                                if (((p_download_desc->excess_header_size) != 0))
                                {
#ifdef __DEBUGVAR__
                        printf("1.store_download called from get_download\n");
#endif
                                        addr = store_download(data, p_download_desc->excess_header_size,
                                                                                        download_block);
                                        p_download_desc->p_excess_header_data = (char *)addr;
                                        data += ((p_download_desc->excess_header_size));
                                }
                                p_download_desc->p_download_description = NULL;
                                if (((file_header->download_description_size) != 0))
                                {
#ifdef __DEBUGVAR__
                        printf("2.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->download_description_size,
                                                                                        download_block);
				/* Showing details of DSP-Task */
			if (verbose) {
				strncpy(Text, data, file_header->download_description_size);
				Text[file_header->download_description_size] = 0;
				printf("\t%s\n", Text);
			}

                                        p_download_desc->p_download_description = (char *)addr;
                                        data += ((file_header->download_description_size));
                                }
                                p_download_desc->p_memory_block_table = NULL;
                                if ((file_header->memory_block_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("3.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->memory_block_table_size,
                                                                                        download_block);
                                        p_download_desc->p_memory_block_table = (t_dsp_memory_block_desc *)addr;
                                        data += ((file_header->memory_block_table_size));
                                }
                                p_download_desc->p_segment_table = NULL;
                                if ((file_header->segment_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("4.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->segment_table_size,
                                                                                        download_block);
                                        p_download_desc->p_segment_table = (t_dsp_segment_desc *)addr;
                                        data += (file_header->segment_table_size);
                                }
                                p_download_desc->p_symbol_table = NULL;
                                if ((file_header->symbol_table_size) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("5.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->symbol_table_size,
                                                                                        download_block);
                                        p_download_desc->p_symbol_table = (t_dsp_symbol_desc *)addr;
                                        data += (file_header->symbol_table_size);
                                }
                                p_download_desc->p_data_blocks_dm = NULL;
                                if ((file_header->total_data_size_dm) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("6.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->total_data_size_dm,
                                                                                        download_block);
                                        p_download_desc->p_data_blocks_dm = (__u16 *)addr;
                                        data += (file_header->total_data_size_dm);
                                }
                                p_download_desc->p_data_blocks_pm = NULL;
                                if ((file_header->total_data_size_pm) != 0)
                                {
#ifdef __DEBUGVAR__
                        printf("7.store_download called from get_download\n");
#endif
                                        addr = store_download(data, file_header->total_data_size_pm,
                                                                                        download_block);
                                        p_download_desc->p_data_blocks_pm = (__u16 *)addr;
                                        data += (file_header->total_data_size_pm);
                                }
                        }
                }

                download_area += ((__u32)((__u16)((file_header->header_size))));
                download_area += ((__u32)((file_header->download_description_size)));
                download_area += ((__u32)((file_header->memory_block_table_size)));
                download_area += ((__u32)((file_header->segment_table_size)));
                download_area += ((__u32)((file_header->symbol_table_size)));
                download_area += ((__u32)((file_header->total_data_size_dm)));
                download_area += ((__u32)((file_header->total_data_size_pm)));


        }

	table_count=no_of_downloads;
	/**no_of_tables=table_count;*/
	bzero(no_of_tables,sizeof(__u32));
	memcpy(no_of_tables,&table_count,sizeof(__u32));

#ifdef __DEBUGVAR__
    printf("***0x%x bytes of memory required for %d downloads***\n", length, no_of_downloads);
#endif
        free(usage_mask);
        return length;
}


eicon_codebuf *load_combifile(int card_type, u_char *protobuf, int *plen)
{
        int fd;
	int tmp[9];
	char combifilename[100];
        int count, j;
        int file_set_number = 0;
        struct stat file_info;
        char *combifile_start;
        char *usage_mask_ptr;
        char *download_block;
        t_dsp_combifile_directory_entry *directory;
        t_dsp_combifile_directory_entry *tmp_directory;
        __u32 download_size;
	eicon_codebuf *cb;

	sprintf(combifilename, "%s/%s", __DATADIR__, COMBIFILE);
        if ((fd = open(combifilename, O_RDONLY, 0)) == -1)
        {
                perror("Error opening Eicon combifile");
                return(0);
        }

        if (fstat(fd, &file_info))
        {
                perror("Error geting file details of Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

        if ( file_info.st_size <= 0 )
        {
                perror("Invalid file length in Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

        combifile_start = malloc(file_info.st_size);

        if(!combifile_start)
        {
                perror("Error allocating memory for Eicon.Diehl combifile");
                close(fd);
                return(0);
        }

#ifdef __DEBUGVAR__
                printf("File mapped to address 0x%x\n", (__u32)combifile_start);
#endif


        if((read(fd, combifile_start, file_info.st_size)) != file_info.st_size)
        {
                perror("Error reading Eicon.Diehl combifile into memory");
                free(combifile_start);
                close(fd);
                return(0);
        }

        close(fd); /* We're done with the file */

        directory = display_combifile_details(combifile_start);

#ifdef __DEBUGVAR__
        printf("Directory mapped to address 0x%x, offset = 0x%x\n", (__u32)directory,
                                                ((unsigned int)directory - (unsigned int)combifile_start));
#endif

        tmp_directory = directory;

        for(count = 0; count < num_directory_entries; count++)
        {
                if((tmp_directory->card_type_number) == card_type)
                {
#ifdef __DEBUGVAR__
                        printf("Found entry in directory slot %d\n", count);
            		printf("Matched Card %d is %d. Fileset number is %d\n", count,
                       		(tmp_directory->card_type_number),
	                        (tmp_directory->file_set_number));

#endif
                        file_set_number = tmp_directory->file_set_number;
                        break;
                }
#ifdef __DEBUGVAR__
                printf("Card %d is %d. Fileset number is %d\n", count,
                                                (tmp_directory->card_type_number),
                                                (tmp_directory->file_set_number));
#endif
                tmp_directory++; 
        }

        if(count == num_directory_entries)
        {
                printf("Card not found in directory\n");
                free(combifile_start);
                return(0);
        }

        usage_bit = file_set_number%8;
        usage_byte= file_set_number/8;

#ifdef __DEBUGVAR__
        printf("Bit field is bit %d in byte %d of the usage mask\n",
                                usage_bit,
                                usage_byte);
#endif

        usage_mask_ptr = (char *)(directory);
        usage_mask_ptr += directory_size;

#ifdef __DEBUGVAR__
        printf("First mask at address 0x%x, offset = 0x%x\n", (__u32)usage_mask_ptr,
                                                (__u32)(usage_mask_ptr - combifile_start));
#endif

        no_of_tables = malloc((sizeof(__u32)));
        download_size = get_download(NULL, usage_mask_ptr);

#ifdef __DEBUGVAR__
        printf("Initial size of download_size is 0x%x\n",download_size);
#endif

        if(!download_size)
        {
                printf("Error getting details on DSP downloads\n");
                free(combifile_start);
                return(0);
        }

        download_block = malloc((download_size + (no_of_downloads * 100)));

#ifdef __DEBUGVAR__
        printf("download_block size = (download_size + alignments) is: 0x%x\n",(download_size + (no_of_downloads*100)));
#endif

        if(!download_block)
        {
                printf("Error allocating memory for download\n");
                free(combifile_start);
                return(0);
        }

#ifdef __DEBUGVAR__
        printf("Calling get_download to write into download_block\n");
#endif

        if(!(get_download(download_block, usage_mask_ptr)))
        {
                printf("Error getting data for DSP download\n");
                free(download_block);
                free(combifile_start);
	        free(no_of_tables);
                return(0);
        }

	tmp[0] = *plen;

	memcpy(protobuf + *plen, download_block, total_bytes_in_download);
	tmp[1] = total_bytes_in_download;
	*plen +=  total_bytes_in_download;

	memcpy(protobuf + *plen, no_of_tables, sizeof(table_count));
	tmp[2] = sizeof(table_count);
	*plen +=  sizeof(table_count);

	memcpy(protobuf + *plen, (char *)p_download_table, sizeof(p_download_table));
	tmp[3] = sizeof(p_download_table);
	*plen +=  sizeof(p_download_table);

	cb = malloc(sizeof(eicon_codebuf) + *plen);
	memset(cb, 0, sizeof(eicon_codebuf));
        memcpy(&cb->pci.code, protobuf, *plen);
    	for (j=0; j < 4; j++) {
		if (j==0) cb->pci.protocol_len = tmp[0];
		else cb->pci.dsp_code_len[j] = tmp[j];
	}
	cb->pci.dsp_code_num = 3;

 free(no_of_tables);
 free(download_block);
 free(combifile_start);
 return (cb);
}

void beep2(void)
{
 beep();
 fflush(stdout);
 refresh();
}

int get_manage_element(char *m_dir, int request)
{
	int i,j,o,k,tmp;
	int len, vlen = 0;
	long unsigned duint;
	u_char buf[100];

	if (strlen(m_dir)) {
		if (m_dir[0] == '\\') m_dir++;
	}
	len = strlen(m_dir);
        mb->count = request;
        mb->pos = 0;
        mb->length[0] = len + 5;
        memset(&mb->data, 0, 690);

	if (len)
	        strncpy(&mb->data[5], m_dir, len);
        mb->data[4] = len;

        ioctl_s.arg = (ulong)mb;
        if (ioctl(fd, EICON_IOCTL_MANIF + IIOCDRVCTL, &ioctl_s) < 0) {
		return(-1);
        }
	if (request == 0x04) return 0;

	mb->pos = 0;
	man_ent_count = mb->count;
        for (i = 0; i < mb->count; i++) {
		man_ent[i].type = mb->data[mb->pos++];
		man_ent[i].attribute = mb->data[mb->pos++];
		man_ent[i].status = mb->data[mb->pos++];
		man_ent[i].var_length = mb->data[mb->pos++];
		man_ent[i].path_length = mb->data[mb->pos++];

		memcpy(man_ent[i].Name, &mb->data[mb->pos] + len, man_ent[i].path_length - len);
		man_ent[i].Name[man_ent[i].path_length - len] = 0;
		if (man_ent[i].Name[0] == '\\') strcpy(man_ent[i].Name, man_ent[i].Name + 1);
		mb->pos += man_ent[i].path_length;

		if (man_ent[i].type &0x80) 
			vlen = man_ent[i].var_length;
		else
			vlen = mb->length[i] - man_ent[i].path_length - 5; 

		memcpy(man_ent[i].Var, &mb->data[mb->pos], vlen); 
		man_ent[i].Var[vlen] = 0;
		mb->pos += (mb->length[i]  - man_ent[i].path_length - 5);
		o = 0;
		if (vlen) {
			switch(man_ent[i].type) {
				case 0x04:
					if (man_ent[i].Var[0])
					{
						j=0;
						do
						{
							buf[o++] = man_ent[i].Var[++j];
							buf[o] = 0;
						}
						while (j < man_ent[i].Var[0]);
						strcpy(man_ent[i].Var, buf);
					}
					break;
				case 0x05:
					if (man_ent[i].Var[0])
					{
						j=0;
						do
						{
							o+=sprintf(&buf[o],"%02x ",(__u8)man_ent[i].Var[++j]);
						}
						while (j < man_ent[i].Var[0]);
						strcpy(man_ent[i].Var, buf);
					}
					break;
				case 0x81:
					duint = man_ent[i].Var[vlen-1]&0x80 ? -1 : 0; 
					for (j=0; j<vlen; j++) ((__u8 *)&duint)[j] = man_ent[i].Var[j];
					sprintf(man_ent[i].Var,"%ld",(long)duint);
					break;
				case 0x82:
					for (j=0,duint=0; j<vlen; j++) 
						duint += ((unsigned char)man_ent[i].Var[j]) << (8 * j);
					sprintf(man_ent[i].Var,"%lu", duint);
					break;
				case 0x83:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char)man_ent[i].Var[j]) << (8 * j);
					sprintf(man_ent[i].Var,"%lx", duint);
					break;
				case 0x84:
					for (j=0; j<vlen; j++) o+=sprintf(&buf[o], "%02x", man_ent[i].Var[j]);
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x85:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char) man_ent[i].Var[j]) << (8 * j);
					if (duint) sprintf(man_ent[i].Var,"TRUE");
					else sprintf(man_ent[i].Var,"FALSE");
					break;
				case 0x86:
					for (j=0; j<vlen; j++)
					{
						if (j) o+=sprintf(&buf[o], ".");
						o+=sprintf(&buf[o], "%03i", man_ent[i].Var[j]);
					}
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x87:
					for (j=0,duint=0; j<vlen; j++)
						duint += ((unsigned char) man_ent[i].Var[j]) << (8 * j);
					for (j=0; j<vlen; j++)
					{
						if (j) o+=sprintf(&buf[o], " ");
						tmp = (__u8)(duint >> (vlen-j-1)*8);
						for (k=0; k<8; k++)
						{
							o+=sprintf(&buf[o], "%d", (tmp >> (7-k)) &0x1);
						}
					}
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x88:
					j = MIN(sizeof(spid_state)/sizeof(__u8 *)-1, man_ent[i].Var[0]);
					sprintf(man_ent[i].Var, "%s", spid_state[j]);
					break;
			}
		}
		else 
		if (man_ent[i].type == 0x02) strcpy(man_ent[i].Var, "COMMAND");
	}
  return(0);
}

void eicon_manage_head(void)
{
	int ctype;
        mvwaddstr(headwin, 0,0,"Management for Eicon DIVA Server cards                      Cytronics & Melware");
        if ((ctype = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 0) {
                return;
        }
        switch (ctype) {
                case EICON_CTYPE_MAESTRAP:
                        mvwaddstr(headwin, 1,0,"Adapter-type is Diva Server PRI/PCI");
                        break;
                case EICON_CTYPE_MAESTRA:
                        mvwaddstr(headwin, 1,0,"Adapter-type is Diva Server BRI/PCI");
                        break;
                default:
                        mvwaddstr(headwin, 1,0,"Adapter-type is unknown");
                        return;
        }
}

void eicon_manage_init_ncurses(void)
{
        initscr();
        noecho();
        nonl();
        refresh();
        cbreak();
        keypad(stdscr,TRUE);
	curs_set(0);
	statwin = newpad(50,80);
	headwin = newpad(5,80);
        start_color();
	
}

void show_man_entries(void)
{
	int i;
	char MLine[80];
	char AttSt[7];

        for(i = 0; i < man_ent_count; i++) {
                if (man_ent[i].type == 0x01) {
                        sprintf(AttSt, "<DIR>");
                } else {
                        sprintf(AttSt,"%c%c%c%c%c",
                                (man_ent[i].attribute &0x01) ? 'w' : '-',
                                (man_ent[i].attribute &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x01) ? 'l' : '-',
                                (man_ent[i].status &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x04) ? 'p' : '-');
                }
                sprintf(MLine, "%-17s %s %s\n",
                        man_ent[i].Name,
                        AttSt,
                        man_ent[i].Var);
                printf(MLine);
        }
}

void eicon_manage_draw(void)
{
	int i;
	int max_line = 0;
	char MLine[80];
	char AttSt[7];

	mvwaddstr(headwin, 2, 0, "                                                                                ");
        mvwaddstr(headwin, 3,0,"Name              Flags Variable");
        mvwaddstr(headwin, 4,0,"-------------------------------------------------------------------------------");

        max_line =  man_ent_count;
        for(i = 0; i < max_line; i++) {
                if (man_ent[i].type == 0x01) {
                        sprintf(AttSt, "<DIR>");
                } else {
                        sprintf(AttSt,"%c%c%c%c%c",
                                (man_ent[i].attribute &0x01) ? 'w' : '-',
                                (man_ent[i].attribute &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x01) ? 'l' : '-',
                                (man_ent[i].status &0x02) ? 'e' : '-',
                                (man_ent[i].status &0x04) ? 'p' : '-');
                }
                sprintf(MLine, "%-17s %s %-55s",
                        man_ent[i].Name,
                        AttSt,
                        man_ent[i].Var);
                if (i == h_line) wattron(statwin, A_REVERSE);
                mvwaddstr(statwin, i, 0, MLine);
                wattroff(statwin, A_REVERSE);
        }
        for(i = max_line; i < 50; i++) {
		mvwaddstr(statwin, i, 0, "                                                                                ");
	}
        prefresh(statwin, stat_y, 0, 5, 0, LINES-4, COLS);
        mvwaddstr(headwin, 2,0,"Directory : ");
        waddstr(headwin, Man_Path);
        prefresh(headwin, 0, 0, 0, 0, 5, COLS);
	refresh();
}

void do_manage_resize(int dummy) {
        endwin();
	eicon_manage_init_ncurses();
	eicon_manage_head();
	eicon_manage_draw();
	eicon_manage_draw();
	refresh();
	signal(SIGWINCH, do_manage_resize);
}

void eicon_management(void)
{
	int Key;
	int i;
	h_line = 0;
	stat_y = 0;

	signal(SIGWINCH, do_manage_resize);

	eicon_manage_init_ncurses();
	eicon_manage_head();
	eicon_manage_draw();
	redraw1:
	eicon_manage_draw();
	Keyboard:
	Key = getch();
	switch(Key) {
		case 27:
		case 'q':
			break;
		case KEY_UP:
			if (h_line) {
				h_line--;
				if (stat_y > h_line) stat_y--;
			}
			goto redraw1;
		case KEY_DOWN:
			if (h_line < man_ent_count - 1) {
				h_line++;
				if ((stat_y + LINES - 9) < h_line) stat_y++;
			}
			goto redraw1;
		case KEY_LEFT:
			if ((strcmp(Man_Path,"\\")) && (strlen(Man_Path) > 1)) {
				for(i=strlen(Man_Path); i >= 0; i--) {
					if (Man_Path[i] == '\\') {
						Man_Path[i] = 0;
						break;
					}
				}
				if (strlen(Man_Path) == 0) strcpy(Man_Path,"\\");
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case 10:
		case 13:
		case KEY_RIGHT:
			if (man_ent[h_line].type == 0x01) { /* Directory */
				if (Man_Path[strlen(Man_Path)-1] != '\\') strcat(Man_Path, "\\");
				strcat(Man_Path, man_ent[h_line].Name);
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;
			}
			if (man_ent[h_line].type == 0x02) { /* Executable function */
				i = strlen(Man_Path);
				if (Man_Path[strlen(Man_Path)-1] != '\\') strcat(Man_Path, "\\");
				strcat(Man_Path, man_ent[h_line].Name);
				if (get_manage_element(Man_Path, 0x04) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				sleep(1);
				Man_Path[i] = 0;
				if (get_manage_element(Man_Path, 0x02) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				h_line = 0;
				stat_y = 0;
				goto redraw1;

			}
			beep2();
			goto Keyboard;
		case 'r':
			if (get_manage_element(Man_Path, 0x02) < 0) {
				clear();
	       	                mvaddstr(0,0, "Error ioctl Management-interface");
				refresh();
       			        return;
			}
			goto redraw1;
		default:
			beep2();
			goto Keyboard;
	}

}

void filter_slash(char *s)
{
	int i;
	for (i=0; i < strlen(s); i++)
		if (s[i] == '/') s[i] = '\\';
}

void load_startup_code(char *startupcode, char *fileext)
{
	FILE *code;
	u_char bootbuf[0x1000];
	char filename[100];
	int tmp;
	eicon_codebuf *cb;

	sprintf(filename, "%s/%s", __DATADIR__, startupcode);

	if (!(code = fopen(filename,"r"))) {
		perror(filename);
		exit(-1);
	}
	if ((tmp = fread(bootbuf, 1, sizeof(bootbuf), code))<1) {
		fprintf(stderr, "Read error on %s\n", filename);
		exit(-1);
	}
	fclose(code);
	cb = malloc(sizeof(eicon_codebuf) + tmp);
	memset(cb, 0, sizeof(eicon_codebuf));
	memcpy(&cb->isa.code, bootbuf, tmp);
	cb->isa.bootstrap_len = tmp;
	cb->isa.boot_opt = EICON_ISA_BOOT_NORMAL;
	printf("Loading Startup Code (%s %d bytes)...\n", startupcode, tmp);
	ioctl_s.arg = (ulong)cb;
	if (ioctl(fd, EICON_IOCTL_LOADBOOT + IIOCDRVCTL, &ioctl_s) < 0) {
		perror("ioctl LOADBOOT");
		exit(-1);
	}
	if ((tmp = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
		perror("ioctl GETTYPE");
		exit(-1);
	}
	switch (tmp) {
		case EICON_CTYPE_S:
			strcpy(fileext,".bin");
			printf("Adapter-type is Eicon-S\n");
			break;
		case EICON_CTYPE_SX:
			strcpy(fileext,".sx");
			printf("Adapter-type is Eicon-SX\n");
			break;
		case EICON_CTYPE_SCOM:
			strcpy(fileext,".sy");
			printf("Adapter-type is Eicon-SCOM\n");
			break;
		case EICON_CTYPE_QUADRO:
			strcpy(fileext,".sq");
			printf("Adapter-type is Eicon-QUADRO\n");
			break;
		case EICON_CTYPE_S2M:
			strcpy(fileext,".p");
			printf("Adapter-type is Eicon-S2M\n");
			break;
		default:
			fprintf(stderr, "Unknown Adapter type %d for ISA-load\n", tmp);
			exit(-1);
	}
}


int main(int argc, char **argv) {
	int tmp;
	int ac;
	int arg_ofs=1;

	cmd = argv[0];
	if (argc > 1) {
		if (!strcmp(argv[arg_ofs], "-d")) {
			arg_ofs++;
			strcpy(ioctl_s.drvid, argv[arg_ofs++]);
			if (!strcmp(argv[arg_ofs], "-v")) {
				arg_ofs++;
				verbose = 1;
			}
		} else {
			ioctl_s.drvid[0] = '\0';
			if (!strcmp(argv[arg_ofs], "-v")) {
				arg_ofs++;
				verbose = 1;
			}
		}
	} else
		usage();
	ac = argc - (arg_ofs - 1);
	if (ac < 2)
		usage();
	fd = open("/dev/isdnctrl",O_RDWR);
	if (fd < 0) {
		perror("/dev/isdnctrl");
		exit(-1);
	}

	if ((tmp = ioctl(fd, EICON_IOCTL_GETVER + IIOCDRVCTL, &ioctl_s)) < 0) {
		fprintf(stderr, "Driver ID %s not found or\n", ioctl_s.drvid);
		fprintf(stderr, "Eicon kernel driver is too old !\n"); 
		exit(-1);
	}
	if (tmp < EICON_CTRL_VERSION) {
		fprintf(stderr, "Eicon kernel driver is older than eiconctrl !\n"); 
		fprintf(stderr, "You need newer version !\n");
		exit(-1);
	}
	if (tmp > EICON_CTRL_VERSION) {
		fprintf(stderr, "Eicon kernel driver is newer than eiconctrl !\n"); 
		fprintf(stderr, "You need newer version !\n");
		exit(-1);
	}

	if (!strcmp(argv[arg_ofs], "add")) {
		eicon_cdef *cdef;
		if (ac != 5) 
			usage();
		cdef = malloc(sizeof(eicon_cdef));
		strcpy(cdef->id, argv[arg_ofs + 1]);
		if (strlen(cdef->id) < 1)
			usage();
		if (sscanf(argv[arg_ofs + 2], "%i", &cdef->membase) !=1 )
			usage();
		if (sscanf(argv[arg_ofs + 3], "%i", &cdef->irq) !=1 )
			usage();
		ioctl_s.arg = (ulong)cdef;
		if (ioctl(fd, EICON_IOCTL_ADDCARD + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl ADDCARD");
			exit(-1);
		}
		printf("Card added.\n");
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "membase")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) !=1 )
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, EICON_IOCTL_SETMMIO + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETMMIO");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, EICON_IOCTL_GETMMIO + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETMMIO");
			exit(-1);
		}
		printf("Shared memory at 0x%x\n", tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "irq")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) != 1)
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, EICON_IOCTL_SETIRQ + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETIRQ");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, EICON_IOCTL_GETIRQ + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETIRQ");
			exit(-1);
		}
		printf("Irq is %d\n", tmp);
		close(fd);
		return 0;
	}

        if ((!strcmp(argv[arg_ofs], "load")) || (!strcmp(argv[arg_ofs], "loadpci"))) {
                FILE *code;
		int isabus = 0;
                int plen = 0;
		int ctype = 0;
		int card_type = 0;
		int tmp, i;
		int tei = 255;
		char fileext[5];
                char protoname[1024];
                char filename[1024];
                u_char protobuf[0x100000];
                eicon_codebuf *cb;

		if (argc <= (arg_ofs + 1))
                       	strcpy(protoname,"etsi");
		else {
			if (argv[arg_ofs + 1][0] == '-')
                       		strcpy(protoname,"etsi");
			else	
	                        strcpy(protoname,argv[++arg_ofs]);
		}

		if ((ctype = ioctl(fd, EICON_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
			perror("ioctl GETTYPE");
			exit(-1);
		}
		switch (ctype) {
			case EICON_CTYPE_MAESTRAP:
				printf("Adapter-type is Diva Server PRI/PCI\n");
				strcpy(fileext, ".pm");
				card_type = 23;
				tei = 1;
				break;
			case EICON_CTYPE_MAESTRA:
				printf("Adapter-type is Diva Server BRI/PCI\n");
				strcpy(fileext, ".sm");
				card_type = 21;
				tei = 0;
				break;
			case EICON_CTYPE_S:
			case EICON_CTYPE_SX:
			case EICON_CTYPE_SCOM:
			case EICON_CTYPE_QUADRO:
			case EICON_CTYPE_ISABRI:
				isabus = 1;
				tei = 0;
				load_startup_code("dnload.bin", fileext);
				break;
			case EICON_CTYPE_S2M:
			case EICON_CTYPE_ISAPRI:
				isabus = 1;
				tei = 1;
				load_startup_code("prload.bin", fileext);
				break;
			default:
				fprintf(stderr, "Adapter type %d not supported\n", ctype);
                          	exit(-1);
		}

	  	sprintf(filename, "%s/te_%s%s", __DATADIR__, protoname, fileext);
               	if (!(code = fopen(filename,"r"))) {
                               	perror(filename);
                               	exit(-1);
               	}
		printf("Protocol File : %s ", filename);
		if ((tmp = fread(protobuf, 1, sizeof(protobuf), code))<1) {
			fclose(code);
			fprintf(stderr, "Read error on %s\n", filename);
       	                exit(-1);
		}
               	fclose(code);
               	printf("(%d bytes)\n", tmp);
		plen += tmp;
			
		if (verbose) {
			if (isabus) {
				printf("Protocol: %s\n", &protobuf[4]);
				plen = (plen % 256)?((plen/256)+1)*256:plen;
			} else {
				strncpy(filename, &protobuf[0x80], 100);
				for (i=0; filename[i] && filename[i]!='\r' && filename[i]!='\n'; i++);
				filename[i] = 0;
				printf("%s\n", filename);
			}
		}

		if (isabus) {
			if(!(cb = malloc(sizeof(eicon_codebuf) + plen ))) {
        	                fprintf(stderr, "Out of Memory\n");
       	        	        exit(-1);
			}
			memset(cb, 0, sizeof(eicon_codebuf));
			memcpy(&cb->isa.code, protobuf, plen);
			cb->isa.firmware_len = plen;
		} else {
			if (!(cb = load_combifile(card_type, protobuf, &plen))) {
        	                fprintf(stderr, "Error loading Combifile\n");
       	        	        exit(-1);
			}
		}

		if (isabus) {
			cb->isa.tei = tei;
			cb->isa.nt2 = 0;
			cb->isa.WatchDog = 0;
			cb->isa.Permanent = 0;
			cb->isa.XInterface = 0;
			cb->isa.StableL2 = 0;
			cb->isa.NoOrderCheck = 0;
			cb->isa.HandsetType = 0;
			cb->isa.LowChannel = 0;
			cb->isa.ProtVersion = 0;
			cb->isa.Crc4 = 0;
			cb->isa.Loopback = 0;
		} else {
			cb->pci.tei = tei;
			cb->pci.nt2 = 0;
			cb->pci.WatchDog = 0;
			cb->pci.Permanent = 0;
			cb->pci.XInterface = 0;
			cb->pci.StableL2 = 0;
			cb->pci.NoOrderCheck = 0;
			cb->pci.HandsetType = 0;
			cb->pci.LowChannel = 0;
			cb->pci.ProtVersion = 0;
			cb->pci.Crc4 = 0;
			cb->pci.Loopback = 0;
			cb->pci.NoHscx30Mode = 0;
		}

		/* parse extented options */
		while(ac > (arg_ofs + 1)) {
			arg_ofs++;
			if (!strncmp(argv[arg_ofs], "-l", 2)) {
				if (isabus) {
					cb->isa.LowChannel = atoi(argv[arg_ofs] + 2);
					if (!cb->isa.LowChannel) cb->isa.LowChannel = 1;
				} else {
					cb->pci.LowChannel = atoi(argv[arg_ofs] + 2);
					if (!cb->pci.LowChannel) cb->pci.LowChannel = 1;
				}
                                continue;
                        }
                        if (!strncmp(argv[arg_ofs], "-t", 2)) {
				if (isabus) {
                                	cb->isa.tei = atoi(argv[arg_ofs] + 2);
	                                cb->isa.tei <<= 1;
        	                        cb->isa.tei |= 0x01;
				} else {
                                	cb->pci.tei = atoi(argv[arg_ofs] + 2);
	                                cb->pci.tei <<= 1;
        	                        cb->pci.tei |= 0x01;
				}
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-z")) {
				if (isabus) 
	                                cb->isa.Loopback = 1;
				else
	                                cb->pci.Loopback = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-p")) {
				if (isabus) 
                                	cb->isa.Permanent = 1;
				else
                                	cb->pci.Permanent = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-w")) {
				if (isabus) 
                        	        cb->isa.WatchDog = 1;
				else
                        	        cb->pci.WatchDog = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c")) {
				if (isabus) 
                	                cb->isa.Crc4 = 1;
				else
                	                cb->pci.Crc4 = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c1")) {
				if (isabus) 
        	                        cb->isa.Crc4 = 1;
				else
        	                        cb->pci.Crc4 = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-c2")) {
				if (isabus) 
	                                cb->isa.Crc4 = 2;
				else
	                                cb->pci.Crc4 = 2;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-n")) {
				if (isabus) 
                                	cb->isa.nt2 = 1;
				else
                                	cb->pci.nt2 = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-p")) {
				if (isabus) 
                        	        cb->isa.Permanent = 1;
				else
                        	        cb->pci.Permanent = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-h")) {
                                if (!isabus) cb->pci.NoHscx30Mode = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-o")) {
				if (isabus) 
                	                cb->isa.NoOrderCheck = 1;
				else
                	                cb->pci.NoOrderCheck = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s")) {
				if (isabus) 
        	                        cb->isa.StableL2 = 1;
				else
        	                        cb->pci.StableL2 = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s1")) {
				if (isabus) 
	                                cb->isa.StableL2 = 1;
				else
	                                cb->pci.StableL2 = 1;
                                continue;
                        }
                        if (!strcmp(argv[arg_ofs], "-s2")) {
				if (isabus) 
                                	cb->isa.StableL2 = 2;
				else
                                	cb->pci.StableL2 = 2;
                                continue;
                        }
		}
		printf("Downloading Code (%d bytes)...\n", plen);
		ioctl_s.arg = (ulong)cb;
		tmp = (isabus) ? EICON_IOCTL_LOADISA : EICON_IOCTL_LOADPCI;
		if (ioctl(fd, tmp + IIOCDRVCTL, &ioctl_s) < 0) {
			printf("\n");
			perror("ioctl LOAD");
			exit(-1);
		}
		printf("completed.\n");
		close(fd);
		return 0;
        }

        if (!strcmp(argv[arg_ofs], "debug")) {
		if (argc <= (arg_ofs + 1))
			ioctl_s.arg = 1;
		else	
			ioctl_s.arg = atol(argv[arg_ofs + 1]);
		if (ioctl(fd, EICON_IOCTL_DEBUGVAR + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl DEBUG VALUE");
			exit(-1);
		}
		return 0;
	}

        if (!strcmp(argv[arg_ofs], "freeit")) {
		if (ioctl(fd, EICON_IOCTL_FREEIT + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl FREEIT");
			exit(-1);
		}
		return 0;
	}

        if (!strcmp(argv[arg_ofs], "manage")) {
		mb = malloc(sizeof(eicon_manifbuf));

		if (argc > (++arg_ofs)) {
			if (!strcmp(argv[arg_ofs], "read")) {
				if (argc <= (arg_ofs + 1)) {
					fprintf(stderr, "Path to be read is missing\n");
					exit(-1);
				}
				strcpy(Man_Path, argv[arg_ofs + 1]);
				filter_slash(Man_Path);
				if (get_manage_element(Man_Path, 0x02) < 0) {
		       	                fprintf(stderr, "Error ioctl Management-interface\n");
               			        exit(-1);
				}
				show_man_entries();
				close(fd);
				return 0;
			}
			if (!strcmp(argv[arg_ofs], "exec")) {
				if (argc <= (arg_ofs + 1)) {
					fprintf(stderr, "Path to be executed is missing\n");
					exit(-1);
				}
				strcpy(Man_Path, argv[arg_ofs + 1]);
				filter_slash(Man_Path);
				if (get_manage_element(Man_Path, 0x04) < 0) {
		       	                fprintf(stderr, "Error ioctl Management-interface\n");
               			        exit(-1);
				}
				close(fd);
				return 0;
			}
			fprintf(stderr, "Unknown command for Management-interface\n");
			exit(-1);
		}

		strcpy (Man_Path, "\\");

		if (get_manage_element(Man_Path, 0x02) < 0) {
       	                fprintf(stderr, "Error ioctl Management-interface\n");
               	        exit(-1);
		}

		eicon_management();
		move(22,0);
		refresh();
		endwin();

                close(fd); 
		return 0;
	}

	fprintf(stderr, "unknown command\n");
	exit(-1);
}

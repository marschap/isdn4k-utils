/* $Id: eiconctrl.c,v 1.4 1999/03/02 11:35:57 armin Exp $
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
#include <ncurses.h>


#include <linux/types.h>
#include <linux/isdn.h>
#include <eicon.h>


#define COMBIFILE "/usr/lib/isdn/dspdload.bin"

#define MIN(a,b) ((a)>(b) ? (b) : (a))

char *cmd;
int verbose = 0;
eicon_manifbuf *mb;

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
  fprintf(stderr,"usage: %s [-d <DriverID>] shmem [shmem-addr]          (get/set memaddr)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] irq   [irq-nr]              (get/set irq)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] load  <bootcode> <protocol> (load firmware)\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] [-v] loadpci <protocol> [options]\n",cmd);
  fprintf(stderr,"   or: %s [-d <DriverID>] debug [<debug value>]\n",cmd);
  fprintf(stderr,"load firmware for PCI cards:\n");
  fprintf(stderr," basics  : -d <DriverID> ID defined when eicon module was loaded\n");
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


diehl_codebuf *load_combifile(int card_type, u_char *protobuf, int *plen)
{
        int fd;
	int tmp[9];
        int count, j;
        int file_set_number = 0;
        struct stat file_info;
        char *combifile_start;
        char *usage_mask_ptr;
        char *download_block;
        t_dsp_combifile_directory_entry *directory;
        t_dsp_combifile_directory_entry *tmp_directory;
        __u32 download_size;
	diehl_codebuf *cb;

        if ((fd = open(COMBIFILE, O_RDONLY, 0)) == -1)
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

	cb = malloc(sizeof(diehl_codebuf) + *plen);
	memset(cb, 0, sizeof(diehl_codebuf));
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

int get_manage_element(char *m_dir)
{
	int i,j,o,k,tmp;
	int len, vlen = 0;
	__u32 duint;
	u_char buf[100];

	if (strlen(m_dir)) {
		if (m_dir[0] == '\\') m_dir++;
	}
	len = strlen(m_dir);
        mb->count = 0;
        mb->pos = 0;
        mb->length[0] = len + 5;
        memset(&mb->data, 0, 690);

	if (len)
	        strncpy(&mb->data[5], m_dir, len);
        mb->data[4] = len;

        ioctl_s.arg = (ulong)mb;
        if (ioctl(fd, DIEHL_IOCTL_MANIF + IIOCDRVCTL, &ioctl_s) < 0) {
		return(-1);
        }

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
					for (j=0,duint=0; j<vlen; j++) duint += (__u32)man_ent[i].Var[j]<<(8*j);
					sprintf(man_ent[i].Var,"%lu",(long unsigned)duint);
					break;
				case 0x83:
					for (j=0,duint=0; j<vlen; j++) duint += (__u32)man_ent[i].Var[j]<<(8*j);
					sprintf(man_ent[i].Var,"%lx",(long unsigned)duint);
					break;
				case 0x84:
					for (j=0; j<vlen; j++) o+=sprintf(&buf[o], "%02x", man_ent[i].Var[j]);
					strcpy(man_ent[i].Var, buf);
					break;
				case 0x85:
					for (j=0,duint=0; j<vlen; j++) duint += (__u32)man_ent[i].Var[j]<<(8*j);
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
					for (j=0,duint=0; j<vlen; j++) duint += (__u32)man_ent[i].Var[j]<<(8*j);
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

void eicon_management(void)
{
	int i, Key, ctype;
	char MLine[80];
	char AttSt[7];
	int hline = 0;
	int gpos = 0;
	int page = 0;
	int max_line = 0;

	initscr();
	noecho();
	nonl(); 
	refresh();
	cbreak();
	keypad(stdscr,TRUE);
	start_color();

	clear();
	mvaddstr(0,0,"Management for Eicon.Diehl DIVA Server                      Cytronics & Melware");
	if ((ctype = ioctl(fd,DIEHL_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 0) {
		return;
	}
	switch (ctype) {
		case DIEHL_CTYPE_MAESTRAP:
			mvaddstr(1,0,"Adapter-type is Diva Server PRI/PCI");
			break;
		case DIEHL_CTYPE_MAESTRA:
			mvaddstr(1,0,"Adapter-type is Diva Server BRI/PCI");
			break;
		default:
			mvaddstr(1,0,"Adapter-type is unknown");
			return;
	}
	redraw1:
	for(i = 2; i < 23; i++) 
		mvaddstr(i,0,"                                                                               ");
	mvaddstr(2,0,"Directory : ");
	addstr(Man_Path);
	mvaddstr(3,0,"Name              Flags Variable");
	mvaddstr(4,0,"-------------------------------------------------------------------------------");
	max_line = (((page+1)*15) < man_ent_count) ? 15 : (man_ent_count - (page * 15));
	for(i = 0; i < max_line; i++) {
		gpos = page * 15 + i;
		if (man_ent[gpos].type == 0x01) {
			sprintf(AttSt, "<DIR>");
		} else {
			sprintf(AttSt,"%c%c%c%c%c", 
				(man_ent[gpos].attribute &0x01) ? 'w' : '-',
				(man_ent[gpos].attribute &0x02) ? 'e' : '-',
				(man_ent[gpos].status &0x01) ? 'l' : '-',
				(man_ent[gpos].status &0x02) ? 'e' : '-',
				(man_ent[gpos].status &0x04) ? 'p' : '-');
		}
		sprintf(MLine, "%-17s %s %-55s",
			man_ent[gpos].Name,
			AttSt,
			man_ent[gpos].Var);
		if (i == hline) attron(A_REVERSE);
		mvaddstr(5+i,0,MLine);
		attroff(A_REVERSE);
	}
	move(hline + 5,0);
	refresh();
	Keyboard:
	Key = getch();
	switch(Key) {
		case 27:
		case 'q':
			break;
		case KEY_PPAGE:
			if (page) {
				page--;
				hline = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case KEY_NPAGE:
			if (page < man_ent_count / 15) {
				page++;
				hline = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case KEY_UP:
			if (hline) hline--;
			goto redraw1;
		case KEY_DOWN:
			if (hline < max_line - 1) hline++;
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
				if (get_manage_element(Man_Path) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				hline = 0;
				page = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case 10:
		case 13:
		case KEY_RIGHT:
			if (man_ent[page * 15 + hline].type == 0x01) {
				if (Man_Path[strlen(Man_Path)-1] != '\\') strcat(Man_Path, "\\");
				strcat(Man_Path, man_ent[page * 15 + hline].Name);
				if (get_manage_element(Man_Path) < 0) {
					clear();
		       	                mvaddstr(0,0, "Error ioctl Management-interface");
					refresh();
               			        return;
				}
				hline = 0;
				page = 0;
				goto redraw1;
			}
			beep2();
			goto Keyboard;
		case 'r':
			if (get_manage_element(Man_Path) < 0) {
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
	if (!strcmp(argv[arg_ofs], "shmem")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) !=1 )
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, DIEHL_IOCTL_SETMMIO + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETMMIO");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, DIEHL_IOCTL_GETMMIO + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETMMIO");
			exit(-1);
		}
		printf("Shared memory at 0x%08lx\n", (unsigned long)tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "irq")) {
		if (ac == 3) {
			if (sscanf(argv[arg_ofs + 1], "%i", &tmp) != 1)
				usage();
			ioctl_s.arg = tmp;
			if (ioctl(fd, DIEHL_IOCTL_SETIRQ + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl SETIRQ");
				exit(-1);
			}
		}
		if ((tmp = ioctl(fd, DIEHL_IOCTL_GETIRQ + IIOCDRVCTL, &ioctl_s)) < 0) {
			perror("ioctl GETIRQ");
			exit(-1);
		}
		printf("Irq is %d\n", tmp);
		close(fd);
		return 0;
	}
	if (!strcmp(argv[arg_ofs], "load")) {
		FILE *code;
		int plen;
		char protoname[1024];
		char bootname[1024];
		u_char protobuf[0x100000];
		u_char bootbuf[0x1000];
		diehl_codebuf *cb;

		if (ac == 4) {
			strcpy(bootname, argv[arg_ofs + 1]);
			strcpy(protoname,argv[arg_ofs + 2]);
			if ((tmp = ioctl(fd,DIEHL_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
				perror("ioctl GETTYPE");
				exit(-1);
			}
			switch (tmp) {
				case DIEHL_CTYPE_S:
					strcat(bootname,".bin");
					printf("Adapter-type is Diehl-S\n");
					break;
				case DIEHL_CTYPE_SX:
					strcat(protoname,".sx");
					printf("Adapter-type is Diehl-SX\n");
					break;
				case DIEHL_CTYPE_SCOM:
					strcat(protoname,".sy");
					printf("Adapter-type is Diehl-SCOM\n");
					break;
				case DIEHL_CTYPE_QUADRO:
					strcat(protoname,".sq");
					printf("Adapter-type is Diehl-QUADRO\n");
					break;
				case DIEHL_CTYPE_PRI:
					strcat(protoname,".p");
					printf("Adapter-type is Diehl-PRI\n");
					break;
				default:
					fprintf(stderr, "Unknown Adapter type %d\n", tmp);
					exit(-1);
			}
			if (!(code = fopen(protoname,"r"))) {
				perror(protoname);
				exit(-1);
			}
			if ((plen = fread(protobuf, 1, sizeof(protobuf), code))<1) {
				fprintf(stderr, "Read error on %s\n", protoname);
				exit(-1);
			}
			plen = (plen % 256)?((plen/256)+1)*256:plen;
			fclose(code);
			if (!(code = fopen(bootname,"r"))) {
				perror(bootname);
				exit(-1);
			}
			if ((tmp = fread(bootbuf, 1, sizeof(bootbuf), code))<1) {
				fprintf(stderr, "Read error on %s\n", bootname);
				exit(-1);
			}
			fclose(code);
			cb = malloc(sizeof(diehl_codebuf) + tmp + plen );
			memset(cb, 0, sizeof(diehl_codebuf));
			memcpy(&cb->isa.code, bootbuf, tmp);
			memcpy(&cb->isa.code[tmp], protobuf, plen);
			cb->isa.bootstrap_len = tmp;
			cb->isa.firmware_len = plen;
			cb->isa.boot_opt = DIEHL_ISA_BOOT_MEMCHK;
			printf("Loading Protocol %s ...\n", &protobuf[4]);
			ioctl_s.arg = (ulong)cb;
			if (ioctl(fd, DIEHL_IOCTL_LOADBOOT + IIOCDRVCTL, &ioctl_s) < 0) {
				perror("ioctl LOADBOOT");
				exit(-1);
			}
			close(fd);
			return 0;
		}
		usage();
	}

        if (!strcmp(argv[arg_ofs], "loadpci")) {
                FILE *code;
                int plen = 0;
		int ctype = 0;
		int card_type = 0;
		int tmp, i;
		int tei = 255;
		char fileext[5];
                char protoname[1024];
                char filename[1024];
                u_char protobuf[0x100000];
                diehl_codebuf *cb;

			if (argc <= (arg_ofs + 1))
                        	strcpy(protoname,"etsi");
			else {
				if (argv[arg_ofs + 1][0] == '-')
                        		strcpy(protoname,"etsi");
				else	
		                        strcpy(protoname,argv[++arg_ofs]);
			}

                        if ((ctype = ioctl(fd,DIEHL_IOCTL_GETTYPE + IIOCDRVCTL, &ioctl_s)) < 1) {
                                perror("ioctl GETTYPE");
                                exit(-1);
                        }
                        switch (ctype) {
                                case DIEHL_CTYPE_MAESTRAP:
                                        printf("Adapter-type is Diva Server PRI/PCI\n");
					strcpy(fileext, ".pm");
					card_type = 23;
					tei = 1;
					break;
                                case DIEHL_CTYPE_MAESTRA:
                                        printf("Adapter-type is Diva Server BRI/PCI\n");
					strcpy(fileext, ".sm");
					card_type = 21;
					tei = 0;
					break;
                                default:
                                        fprintf(stderr, "Adapter type %d not supported\n", ctype);
                                	exit(-1);
			}

		  	sprintf(filename, "/usr/lib/isdn/te_%s%s", protoname, fileext);
                       	if (!(code = fopen(filename,"r"))) {
                                	perror(filename);
                                	exit(-1);
                       	}
			printf("Protocol File : %s ", filename);
			if ((tmp = fread(protobuf, 1, (sizeof(protobuf)-plen), code))<1) {
				fclose(code);
				fprintf(stderr, "Read error on %s\n", filename);
           	                exit(-1);
                        }
                       	fclose(code);
                       	printf("(%d bytes)\n", tmp);
			plen += tmp;
			
			if (verbose) {
				strncpy(filename, &protobuf[0x80], 100);
				for (i=0; filename[i] && filename[i]!='\r' && filename[i]!='\n'; i++);
				filename[i] = 0;
				printf("%s\n", filename);
			}

			if (!(cb = load_combifile(card_type, protobuf, &plen))) {
       	                        fprintf(stderr, "Error loading Combifile\n");
               	                exit(-1);
			}

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

                        /* parse extented options */
                        while(ac > (arg_ofs + 1)) {
                                arg_ofs++;
                                if (!strncmp(argv[arg_ofs], "-l", 2)) {
                                        cb->pci.LowChannel = atoi(argv[arg_ofs] + 2);
					if (!cb->pci.LowChannel) cb->pci.LowChannel = 1;
                                        continue;
                                }
                                if (!strncmp(argv[arg_ofs], "-t", 2)) {
                                        cb->pci.tei = atoi(argv[arg_ofs] + 2);
                                        cb->pci.tei <<= 1;
                                        cb->pci.tei |= 0x01;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-z")) {
                                        cb->pci.Loopback = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-p")) {
                                        cb->pci.Permanent = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-w")) {
                                        cb->pci.WatchDog = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-c")) {
                                        cb->pci.Crc4 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-c1")) {
                                        cb->pci.Crc4 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-c2")) {
                                        cb->pci.Crc4 = 2;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-n")) {
                                        cb->pci.nt2 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-p")) {
                                        cb->pci.Permanent = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-h")) {
                                        cb->pci.NoHscx30Mode = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-o")) {
                                        cb->pci.NoOrderCheck = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-s")) {
                                        cb->pci.StableL2 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-s1")) {
                                        cb->pci.StableL2 = 1;
                                        continue;
                                }
                                if (!strcmp(argv[arg_ofs], "-s2")) {
                                        cb->pci.StableL2 = 2;
                                        continue;
                                }
                        }
                        printf("Downloading Code (%d bytes)...\n", plen);
                        ioctl_s.arg = (ulong)cb;
                        if (ioctl(fd, DIEHL_IOCTL_LOADPCI + IIOCDRVCTL, &ioctl_s) < 0) {
				printf("\n");
                                perror("ioctl LOADPCI");
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
		if (ioctl(fd, DIEHL_IOCTL_DEBUGVAR + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl DEBUG VALUE");
			exit(-1);
		}
		return 0;
	}

        if (!strcmp(argv[arg_ofs], "freeit")) {
		if (argc <= (arg_ofs + 1))
			ioctl_s.arg = 0;
		else	
			ioctl_s.arg = atol(argv[arg_ofs + 1]);
		if (ioctl(fd, DIEHL_IOCTL_FREEIT + IIOCDRVCTL, &ioctl_s) < 0) {
			perror("ioctl FREEIT");
			exit(-1);
		}
		return 0;
	}

        if (!strcmp(argv[arg_ofs], "manage")) {
		mb = malloc(sizeof(eicon_manifbuf));

		strcpy (Man_Path, "\\");

		if (get_manage_element(Man_Path) < 0) {
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

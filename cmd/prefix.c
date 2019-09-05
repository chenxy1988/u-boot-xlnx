// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2013 Google, Inc
 *
 * (C) Copyright 2012
 * Pavel Herrmann <morpheus.ibis@gmail.com>
 */

#include <common.h>
#include <dm-demo.h>
#include <mapmem.h>
#include <asm/io.h>
#include <vsprintf.h>
#include <fs.h>

#define MAGIC_HEAD 0xff0055aa
#define MAGIC_TAIL 0xff55aa00
#define MAX_COUNT	4
#define PREFIX_NAME	"prefix.bin"
#define PREFIX_LOAD_ADDR	0x2000000
#define BANK_LOAD_ADDR	0x2000100
#define BANK_DATA_NAME	"bank.bin"
#define DEBUG_INFO	1


typedef struct prefix_s{
	unsigned int magic_head;
	unsigned int debug_mode;
	unsigned int counter;
	unsigned int reboot_reason;
	unsigned int magic_tail;
}PREFIX_T;

typedef struct bank_s{
	unsigned int magic_head;
	unsigned int current;
	unsigned int bank_1_version;
	unsigned int bank_2_version;
	unsigned int magic_tail;
}BANK_T;
static unsigned char prefix_buf[64];
static unsigned char bank_buf[32];

static int search_prefix()
{
	int ret;
	PREFIX_T *prefix = (PREFIX_T *)prefix_buf;
	BANK_T *bank =(BANK_T *)bank_buf;
	loff_t len_read = 0;
	char name[16];

#ifdef DEBUG_INFO
	printf("Starting search prefix prefix addr = %x,bankaddr = %x... \n",prefix,bank);
#endif
	memset(prefix,0x0,sizeof(PREFIX_T));
	fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
	ret = fs_read(PREFIX_NAME,prefix,0,sizeof(PREFIX_T),&len_read);
#ifdef DEBUG_INFO
	printf("debug = %d magic_head = %x,magic_tail = %x,counter = %d,reason = %d \n",prefix->debug_mode,prefix->magic_head,\
		prefix->magic_tail,prefix->counter,prefix->reboot_reason);
#endif
	if((prefix->magic_head != MAGIC_HEAD) || (prefix->magic_tail != MAGIC_TAIL)){
#ifdef DEBUG_INFO
		printf("=======No prefix header found,create a new one \n");
#endif
		prefix->magic_head = MAGIC_HEAD;
		prefix->magic_tail = MAGIC_TAIL;
		prefix->counter = 1;
		fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
		if(fs_write(PREFIX_NAME,prefix,0,sizeof(PREFIX_T),&len_read) < 0){
			printf("READ ONLY,use default paremeter \n");
		}
	}

#ifdef DEBUG_INFO
	printf("Searching available bank \n");
#endif
	memset(bank,0x0,sizeof(BANK_T));
	fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
	ret = fs_read(BANK_DATA_NAME,bank,0,sizeof(BANK_T),&len_read);
#ifdef DEBUG_INFO
	printf("bank magic head = %x,magic tail = %x,current = %d\n",bank->magic_head,bank->magic_tail,bank->current);
#endif
	if((bank->magic_head != MAGIC_HEAD) || (bank->magic_tail != MAGIC_TAIL)){
#ifdef DEBUG_INFO
		printf("Creating bank data \n");
#endif
		bank->magic_head = MAGIC_HEAD;
		bank->magic_tail = MAGIC_TAIL;
		bank->current = 0;
		fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
		if(fs_write(BANK_DATA_NAME,bank,0,sizeof(BANK_T),&len_read) < 0){
			printf("READ ONLY,use default bank \n");
		}
	}
	
	if(prefix->counter > MAX_COUNT){
		if(prefix->debug_mode == 1){
			printf("Debug mode, cancel to switch \n");
			prefix->counter = 0;
		}else{
#ifdef DEBUG_INFO
			printf("Switching current bank from %d ",bank->current);
#endif
			if(bank->current == 0){
				bank->current = 1;
			}else{
				bank->current = 0;
			}
#ifdef DEBUG_INFO
			printf("to %d \n",bank->current);
#endif
			prefix->counter = 0;
			fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
			fs_write(BANK_DATA_NAME,bank,0,sizeof(BANK_T),&len_read);
		}
	}else{
		prefix->counter++;
	}

	sprintf(name,"uImage%d\n",bank->current);
	env_set("kernel_image", name);
	fs_set_blk_dev("mmc","0",FS_TYPE_ANY);
	fs_write(PREFIX_NAME,prefix,0,sizeof(PREFIX_T),&len_read);


	return ret;
}

static int do_search_prefix(cmd_tbl_t *cmdtp, int flag, int argc,
                         char * const argv[])
{
	search_prefix();
	return 0;
}

U_BOOT_CMD(
        prefix,  CONFIG_SYS_MAXARGS,     1,      do_search_prefix,
        "prefix init",
        NULL
);

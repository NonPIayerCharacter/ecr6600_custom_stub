/**
 * @file env.c
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-9-11
 * @version V0.1
 * @par Copyright by http://eswin.com/.
 * @par History 1:
 *      Date:
 *      Version:
 *      Author:
 *      Modification:
 *
 * @par History 2:
 */


/*--------------------------------------------------------------------------
*												Include files
--------------------------------------------------------------------------*/
	
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common_def.h"
#include "env.h"
#include "drv_spiflash.h"

/*--------------------------------------------------------------------------
* 	                                           	Local Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

#define DATA_BEGIN	"0x"

/*--------------------------------------------------------------------------
* 	                                           	Local Types
--------------------------------------------------------------------------*/

typedef struct _partion_dev partion_dev;

/*--------------------------------------------------------------------------
* 	                                           	Local Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                           	Local Function Prototypes
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          	Global Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          	Global Variables
--------------------------------------------------------------------------*/
/**  Description of global variable  */

/*--------------------------------------------------------------------------
* 	                                          	Global Function Prototypes
--------------------------------------------------------------------------*/
	
static partion_dev partionDev = {0};

#ifdef STUB
extern unsigned char partition_backup[0x1000]; // 对于flash加密烧录时，保存明文状态的分区表
#endif

/*--------------------------------------------------------------------------
* 	                                          	Function Definitions
--------------------------------------------------------------------------*/
// modified by lvhuan
static void read_mem_partition_backup(unsigned int addr, unsigned char * buf, unsigned int len)
{
#ifdef STUB
	if(IN32(SOC_PD_APB_ENCRYPT_EN) && (0xFFFFFF00U == *((unsigned int *)partition_backup)))
	{
		memcpy((void *)buf, (void *)(addr + partition_backup), len);
	}
	else
#endif
	{
		spiFlash_api_read( addr, buf,  len);
	}
}

//static int set_status(unsigned char status_table[], int status_num, int status_index)
//{
//	int byte_index = ~0UL;
//	/*
//	* | write garn |       status0       |       status1       |      status2         |
//	* ---------------------------------------------------------------------------------
//	* |    1bit    | 0xFF                | 0x7F                |  0x3F                |
//	* |    8bit    | 0xFFFF              | 0x00FF              |  0x0000              |
//	* |   32bit    | 0xFFFFFFFF FFFFFFFF | 0x00FFFFFF FFFFFFFF |  0x00FFFFFF 00FFFFFF |
//	*/
//	memset(status_table, 0xFF, STATUS_TABLE_SIZE(status_num));
//
//	if (status_index > 0)
//	{
//		byte_index = (status_index - 1) * (EF_WRITE_GRAN / 8);
//		status_table[byte_index] = 0x00;
//	}
//
//	return byte_index;
//}


static int get_status(unsigned char status_table[], unsigned char status_num)
{
	int i = 0, status_num_bak = --status_num;

	while (status_num --)
	{
		/* get the first 0 position from end address to start address */
		if (status_table[status_num * EF_WRITE_GRAN / 8] == 0x00)
		{
			break;
		}
		i++;
	}

	return status_num_bak - i;
}


//static int write_status(unsigned int  addr, unsigned char status_table[], int status_num, int status_index)
//{
//	int result = 0;
//	int byte_index;
//
//	/* set the status first */
//	byte_index = set_status(status_table, status_num, status_index);
//
//	/* the first status table value is all 1, so no need to write flash */
//	if (byte_index == ~0UL)
//	{
//		return 0;
//	}
//
//	/* write the status by write granularity
//	* some flash (like stm32 onchip) NOT supported repeated write before erase */
//	result = spiFlash_api_write(addr + byte_index, (unsigned char *) &status_table[byte_index], EF_WRITE_GRAN / 8);
//
//	return result;
//}



static int read_status(unsigned int addr, unsigned char status_table[], int total_num)
{
	read_mem_partition_backup(addr, (unsigned char *) status_table, STATUS_TABLE_SIZE(total_num));
	return get_status(status_table, total_num);
}



static unsigned int get_next_env_addr(sector_meta_data_t sector, env_meta_data_t pre_env)
{
	unsigned char status_table[ENV_STATUS_TABLE_SIZE];
	unsigned int addr = FAILED_ADDR;

	if (sector->status.store == SECTOR_STORE_EMPTY)
	{
		return FAILED_ADDR;
	}

	if (pre_env->addr.start == FAILED_ADDR)
	{
		/* the first ENV address */
		if(sector->addr == 0x00)
		{
			addr = sector->addr + 0x400; // 跳过bootrom使用的前1K分区表
		}
		else
		{
			addr = sector->addr + SECTOR_HDR_DATA_SIZE;
		}
	} 
	else
	{
		if (pre_env->addr.start <= sector->addr + SECTOR_SIZE)
		{
			/* next ENV address */
			addr = pre_env->addr.start + pre_env->len;
			if (addr > sector->addr + SECTOR_SIZE || pre_env->len == 0)
			{
				return FAILED_ADDR;
			}
		} 
		else
		{
			return FAILED_ADDR;		/* no ENV */
		}
	}

	/* check ENV status, it's ENV_UNUSED when not using */
	if (read_status(addr, status_table, ENV_STATUS_NUM) != ENV_UNUSED)
	{
		return addr;
	} 
	else
	{
		return FAILED_ADDR;		/* no ENV */
	}
}

static int read_env(env_meta_data_t env)
{
	struct env_hdr_data env_hdr;
	unsigned int env_name_addr;

	/* read ENV header raw data */
	read_mem_partition_backup(env->addr.start, (unsigned char *)&env_hdr, sizeof(struct env_hdr_data));
	env->status = (env_status_t) get_status(env_hdr.status_table, ENV_STATUS_NUM);
	env->len = env_hdr.len;

	if(env->len > SECTOR_SIZE - SECTOR_HDR_DATA_SIZE)
	{
		return -2;
	}

	/* the name is behind aligned ENV header */
	env_name_addr = env->addr.start + ENV_HDR_DATA_SIZE;
	read_mem_partition_backup(env_name_addr, (unsigned char *) env->name, EF_WG_ALIGN(env_hdr.name_len));

	/* the value is behind aligned name */
	env->addr.value = env_name_addr + EF_WG_ALIGN(env_hdr.name_len);
	env->value_len = env_hdr.value_len;
	env->name_len = env_hdr.name_len;

	return 0;
}


static int read_sector_meta_data(unsigned addr, sector_meta_data_t sector)
{
	struct sector_hdr_data sec_hdr;

	/* read sector header raw data */
	read_mem_partition_backup(addr, (unsigned char *)&sec_hdr, sizeof(struct sector_hdr_data));
	sector->addr = addr;
	sector->magic = sec_hdr.magic;

	/* check magic word */
	
    if ((sector->addr == 0x00) && (sector->magic != SECTOR_MAGIC_WORD)) {
        return -7;
    }
	
    if ((sector->addr > 0x1000) && (sector->magic != SECTOR_MAGIC_WORD_NV) && (sector->magic != SECTOR_MAGIC_WORD)) {
        return -7;
    }
	

	/* get other sector meta data */
	sector->combined = sec_hdr.combined;
	sector->status.store = (sector_store_status_t) get_status(sec_hdr.status_table.store, SECTOR_STORE_STATUS_NUM);

	return 0;
}


//static unsigned int get_next_sector_addr(sector_meta_data_t pre_sec)
//{
//	unsigned int next_addr;
//
//	if (pre_sec->addr == FAILED_ADDR)
//	{
//		return partionDev.partion[partionDev.index_nv_usr].base;
//	} 
//	else
//	{
//		/* check ENV sector combined */
//		if (pre_sec->combined == SECTOR_NOT_COMBINED)
//		{
//			next_addr = pre_sec->addr + SECTOR_SIZE;
//		} 
//		else
//		{
//			next_addr = pre_sec->addr + pre_sec->combined * SECTOR_SIZE;
//		}
//
//		/* check range */
//		if (next_addr < partionDev.partion[partionDev.index_nv_usr].base + partionDev.partion[partionDev.index_nv_usr].length)
//		{
//			return next_addr;
//		} 
//		else
//		{
//			return FAILED_ADDR;	/* no sector */
//		}
//	}
//}


//static int find_env(const char *key, env_meta_data_t env)
//{
//	unsigned sec_addr;
//	struct sector_meta_data sector;
//
//	sector.addr = FAILED_ADDR;
//	sector.combined = SECTOR_NOT_COMBINED; // just avoid warning
//
//	/* search all sectors */
//	while ((sec_addr = get_next_sector_addr(&sector)) != FAILED_ADDR)
//	{
//		if (read_sector_meta_data(sec_addr, &sector) != 0)
//		{
//			continue;
//		}
//
//		/* sector has ENV */
//		if (sector.status.store == SECTOR_STORE_USING || sector.status.store == SECTOR_STORE_FULL)
//		{
//			env->addr.start = FAILED_ADDR;
//			/* search all ENV */
//			while ((env->addr.start = get_next_env_addr(&sector, env)) != FAILED_ADDR)
//			{
//				if(read_env(env) == 0)
//				{
//					if(env->status == ENV_WRITE && !strncmp(env->name, key, strlen(key)))
//					{
//						return 1;
//					}
//				}
//				else
//				{
//					return 0;
//				}
//			}
//		}
//	}
//
//	return 0;
//}

//static int env_del(const char *key)
//{
//#if (ENV_STATUS_TABLE_SIZE >= DIRTY_STATUS_TABLE_SIZE)
//	unsigned char status_table[ENV_STATUS_TABLE_SIZE];
//#else
//	unsigned char status_table[DIRTY_STATUS_TABLE_SIZE];
//#endif
//	unsigned int dirty_status_addr;
//
//	struct env_meta_data env;
//
//	if (find_env(key, &env))
//	{
//		write_status(env.addr.start, status_table, ENV_STATUS_NUM, ENV_DELETED);
//
//		dirty_status_addr = EF_ALIGN_DOWN(env.addr.start, SECTOR_SIZE) + SECTOR_DIRTY_OFFSET;
//
//		return write_status(dirty_status_addr, status_table, SECTOR_DIRTY_STATUS_NUM, SECTOR_DIRTY_TRUE);
//	}
//
//	return -1;	
//}

//static int env_get(const char *key, void * value, unsigned int value_len)
//{
//	struct env_meta_data env;
//
//	if (find_env(key, &env))
//	{
//		if(value_len < env.value_len)
//		{
//			return -1;
//		}
//
//		read_mem_partition_backup(env.addr.value, (unsigned char*)value, env.value_len);
//		return 0;
//	}
//
//	return -1;
//}

static void partion_parse(struct env_meta_data * env)
{
	char buf[64] = {0};
	char * data, *end;

	struct partion_info * pPartion = &partionDev.partion[partionDev.num++];

	strlcpy(pPartion->key, env->name, MIN(env->name_len+1, KEY_MAX_LEN));
				
	read_mem_partition_backup(env->addr.value, (unsigned char *)buf, env->value_len);

	data = strstr(buf, DATA_BEGIN);
	if (data == NULL)
	{
		return;
	}
	else
	{
		pPartion->base = strtol(data, &end, 0);
	}

	data = strstr(end, DATA_BEGIN);
	if (data == NULL)
	{
		return;
	}
	else
	{
		pPartion->length = strtol(data, NULL, 0);
	}

	if (!strncmp(pPartion->key, PARTITION_NAME_NV_CUSTOMER, strlen(PARTITION_NAME_NV_CUSTOMER)))
	{
		partionDev.index_nv_usr = partionDev.num - 1;
	}
}


// efuse api
void boot_partion_info_get( char * key, unsigned int *addr, unsigned int * length)
{
	int i;
	unsigned int base = 0, len = 0;
	struct partion_info * pPartion;

	if (partionDev.num != 0)
	{
		for (i=0; i<partionDev.num; i++)
		{
			pPartion = &partionDev.partion[i];
			if (!strncmp(key, pPartion->key, strlen(key)))
			{
				base = pPartion->base;
				len = pPartion->length;
				break;
			}
		}
	}
	
	if (addr)
	{
		*addr = base;
	}

	if (length)
	{
		*length = len;
	}
}

//modified by lvhuan
unsigned char boot_partion_init(void)
{
	struct sector_meta_data sector;
	struct env_meta_data env;
	
	//printf("uboot partion_init\r\n");
	if (read_sector_meta_data(PARTITION_BASE_PARTITION, &sector) != 0)
	{
		partionDev.partion[0].base = PARTITION_BASE_PARTITION;
		partionDev.partion[0].length = PARTITION_SIZE_PARTITION;
		//printf("no partition table!\r\n");
		return 1;
	}

	/* sector has ENV */
	if (sector.status.store == SECTOR_STORE_USING || sector.status.store == SECTOR_STORE_FULL)
	{
		env.addr.start = FAILED_ADDR;
		/* search all ENV */
		while ((env.addr.start = get_next_env_addr(&sector, &env)) != FAILED_ADDR)
		{
			if (read_env(&env) == 0)
			{
				if (env.status != ENV_WRITE)
				{
					//printf("nv status abnormal!\r\n");
					return 2;
				}

				partion_parse(&env);
			}
			else
			{
				//printf("which NV length exceeds data space of partition table!\r\n");
				return 3;
			}
		}
		
		//printf("partition table ok!\r\n");
		return 0;
	}
	
	//printf("partition no nv!\r\n");	
	return 4;
}
//SWTR6260-753 end


/**
 * @file env.h
 * @brief env for uboot
 * @details This is the detail description
 * @author liuyong
 * @date 2021-1-21
 * @version V0.1
 * @par Copyright by http://eswin.com/.
 * @par History 1:
 *      Date:2021-9-11
 *      Version:V0.2
 *      Author:liuyong1@eswin.com
 *      Modification:for eco3
 *
 * @par History 2:
 */


#ifndef _ENV_H
#define _ENV_H


/*--------------------------------------------------------------------------
* 	                                        	Include files
--------------------------------------------------------------------------*/
#include "soc_top_reg.h"

/*--------------------------------------------------------------------------
* 	                                        	Macros
--------------------------------------------------------------------------*/
/** nv key max size */
#define KEY_MAX_LEN						(16)
#define SECTOR_MAGIC_WORD				(0x30344645)
#define SECTOR_MAGIC_WORD_NV			(0x31344645)

#define SECTOR_NOT_COMBINED				(0xFFFFFFFF)
#define SECTOR_SIZE						(0x1000)
#define ENV_AREA_SIZE					(0x4000)
#define EF_WRITE_GRAN					(32)
#define FAILED_ADDR						(0xFFFFFFFF)

/** PARTITION_VALUE */
#define PARTITION_BASE_PARTITION		0x00
#define PARTITION_SIZE_PARTITION		0x1000


/** PARTITION_NAME */
#define PARTITION_NAME_PARTITION 		"partition"
#define PARTITION_NAME_UBOOT			"uboot"
#define PARTITION_NAME_CPU				"cpu"
#define PARTITION_NAME_OTA_STATUS		"ota_status"
#define PARTITION_NAME_NV_CUSTOMER		"nv_customer"
#define PARTITION_NAME_NV_DEVELOP		"nv_develop"
#define PARTITION_NAME_NV_AMT			"nv_amt"

/** PARTITION error state */
#define RESPONSE_NO_PARTITION           (0xEA)
#define RESPONSE_PARTITION_ERROR        (0xEB)


// VMA
//#define ENV_DL_FLAG					"DlFlag"	
#define OTA_DIFF_MAGIC_NUMBER         	"FotaPKG"
//#define PARTITION_VMA_UBOOT           EILM_2_BASE
#define PPARTITION_VMA_ANDES            EILM_0_BASE
#define XIP_BASE                      	0x807100


#define EF_ALIGN(size, align)			(((size) + (align) - 1) & ~((align) - 1))
#define EF_WG_ALIGN(size)				(EF_ALIGN(size, (EF_WRITE_GRAN + 7)/8))
#define EF_ALIGN_DOWN(size, align)		((size) & ~((align) - 1))


#define SECTOR_HDR_DATA_SIZE			(EF_WG_ALIGN(sizeof(struct sector_hdr_data)))
#define SECTOR_DIRTY_OFFSET				((unsigned long)(&((struct sector_hdr_data *)0)->status_table.dirty))

#define STATUS_TABLE_SIZE(status_number)	(((status_number - 1) * EF_WRITE_GRAN + 7)/8)
#define STORE_STATUS_TABLE_SIZE				STATUS_TABLE_SIZE(SECTOR_STORE_STATUS_NUM)
#define DIRTY_STATUS_TABLE_SIZE				STATUS_TABLE_SIZE(SECTOR_DIRTY_STATUS_NUM)
#define ENV_STATUS_TABLE_SIZE				STATUS_TABLE_SIZE(ENV_STATUS_NUM)

/*--------------------------------------------------------------------------
* 	                                        	Types
--------------------------------------------------------------------------*/
/**
 * @brief The brief description
 * @details The detail description
 */
enum sector_store_status
{
	SECTOR_STORE_UNUSED,
	SECTOR_STORE_EMPTY,
	SECTOR_STORE_USING,
	SECTOR_STORE_FULL,
	SECTOR_STORE_STATUS_NUM,
};
typedef enum sector_store_status sector_store_status_t;

enum sector_dirty_status
{
	SECTOR_DIRTY_UNUSED,
	SECTOR_DIRTY_FALSE,
	SECTOR_DIRTY_TRUE,
	SECTOR_DIRTY_GC,
	SECTOR_DIRTY_STATUS_NUM,
};
typedef enum sector_dirty_status sector_dirty_status_t;

enum env_status
{
	ENV_UNUSED,
	ENV_PRE_WRITE,
	ENV_WRITE,
	ENV_PRE_DELETE,
	ENV_DELETED,
	ENV_ERR_HDR,
	ENV_STATUS_NUM,
};
typedef enum env_status env_status_t;

struct sector_hdr_data 
{
	struct 
	{
		unsigned char store[STORE_STATUS_TABLE_SIZE];	/**< sector store status @see sector_store_status_t */
		unsigned char dirty[DIRTY_STATUS_TABLE_SIZE];	/**< sector dirty status @see sector_dirty_status_t */
	} status_table;
	unsigned int magic; 							/**< magic word(`E`, `F`, `4`, `0`) */
	unsigned int combined;							/**< the combined next sector number, 0xFFFFFFFF: not combined */
	unsigned int reserved;
};
//typedef struct sector_hdr_data *sector_hdr_data_t;

struct sector_meta_data
{
//	bool check_ok;					/**< sector header check is OK */
	struct
	{
		sector_store_status_t store;		/**< sector store status @see sector_store_status_t */
		sector_dirty_status_t dirty;		/**< sector dirty status @see sector_dirty_status_t */
	} status;
	unsigned int addr;					/**< sector start address */
	unsigned int magic; 			/**< magic word(`E`, `F`, `4`, `0`) */
	unsigned int combined;			/**< the combined next sector number, 0xFFFFFFFF: not combined */
	int remain; 					/**< remain size */
	unsigned int empty_env; 		/**< the next empty ENV node start address */
};
typedef struct sector_meta_data *sector_meta_data_t;

struct env_hdr_data
{
	unsigned char status_table[ENV_STATUS_TABLE_SIZE];		/**< ENV node status, @see node_status_t */
	unsigned int len;										/**< ENV node total length (header + name + value), must align by EF_WRITE_GRAN */
	unsigned int crc32; 								/**< ENV node crc32(name_len + data_len + name + value) */
	unsigned char name_len; 							/**< name length */
	unsigned int value_len; 							/**< value length */
};
//typedef struct env_hdr_data *env_hdr_data_t;
#define ENV_HDR_DATA_SIZE		(EF_WG_ALIGN(sizeof(struct env_hdr_data)))


struct env_meta_data 
{
//	bool crc_is_ok; 			/**< ENV node CRC32 check is OK */
	env_status_t status;			/**< ENV node status, @see node_status_t */
	unsigned char name_len; 	/**< name length */
	unsigned int len;				/**< ENV node total length (header + name + value), must align by EF_WRITE_GRAN */
	unsigned int value_len; 	/**< value length */
	char name[32];				/**< name */
	struct
	{
		unsigned int start; 		/**< ENV node start address */
		unsigned int value; 	/**< value start address */
	} addr;
};
typedef struct env_meta_data *env_meta_data_t;

struct partion_info
{
	char  key[KEY_MAX_LEN];
	unsigned int base;
	unsigned int length;
};

#define PARTION_INDEX_MAX  		(32)
struct _partion_dev
{
	unsigned int num;
	unsigned int index_nv_usr;
	struct partion_info partion[PARTION_INDEX_MAX];
};


/*--------------------------------------------------------------------------
* 	                                        	Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                        	Global  Variables
--------------------------------------------------------------------------*/
/**  Description of global variable  */

/*--------------------------------------------------------------------------
* 	                                        	Function Prototypes
--------------------------------------------------------------------------*/
unsigned char boot_partion_init(void);
void boot_partion_info_get(char * key, unsigned int *addr, unsigned int * length);
//void partion_parse(struct env_meta_data * env);
//int env_get(const char *key, void * value, unsigned int value_len);
//int env_del(const char *key);
//int find_env(const char *key, env_meta_data_t env);
//unsigned int get_next_sector_addr(sector_meta_data_t pre_sec);
//int read_sector_meta_data(unsigned addr, sector_meta_data_t sector);
//int read_env(env_meta_data_t env);
//unsigned int get_next_env_addr(sector_meta_data_t sector, env_meta_data_t pre_env);



#endif/*_ENV_H*/


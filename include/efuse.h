/**
 * @file efuse.h
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-6-12
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


#ifndef _EFUSE_H
#define _EFUSE_H


/*--------------------------------------------------------------------------
* 	                                        	Include files
--------------------------------------------------------------------------*/

//efuse
#define EFUSE_CTRL_OFFSET						(0x20)

#define EFUSE_JTAG_CTRL_ADDR_OFFSET				(0x20)
#define JTAG_CTRL_VAL(x)						((x) & 0x3)
#define EFUSE_JTAG_CTRL_EN						(0x0)
//#define EFUSE_JTAG_CTRL_AUTO					(0x1)  // delete for eco3
#define EFUSE_JTAG_CTRL_DIS						(0x3)
#define AON_JTAG_ENABLE                  		(0x1)
#define AON_JTAG_DISABLE                		(0x0)
	
#define EFUSE_SECURE_BOOT_EN					(0x1 << 0x2)
#define EFUSE_CTRL_FLASH_ENCRYPT_EN				(0x1 << 0x3)
#define EFUSE_DIG_TEST_SHORT_DVDD_SW_EN			(0x1 << 0x4)
#define EFUSE_CTRL_AES_KEY_READ_DIS		 		(0x1 << 0x5)
#define EFUSE_CTRL_AES_KEY_WRITE_DIS			(0x1 << 0x6)
#define EFUSE_CTRL_Reserved_263_1		 		(0x1 << 0x7)
#define EFUSE_CTRL_FLASH_ENCRYPT_CNT			(0x1 << 0x8)
#define EFUSE_CRYSTAL_26M						(0x0 << 16)
#define EFUSE_CRYSTAL_40M						(0x1 << 16)
#define EFUSE_CRYSTAL_OFFSET					(0x1 << 16)
#define EFUSE_LOG_MEMORY						(0x0 << 17)
#define EFUSE_LOG_PRINTF						(0x1 << 17)
#define EFUSE_LOG_OFFSET						(0x1 << 17)
#define EFUSE_NSEL_160M							(0x0 << 18)
#define EFUSE_SEL_160M							(0x1 << 18)
#define EFUSE_DIRECT_START_MODE					(0x0 << 19)
#define EFUSE_TRUTH_TABLE_MODE					(0x1 << 19)
#define EFUSE_BOOTSELECT_MODE_OFFSET			(0x1 << 19)

#define EFUSE_FLASH_ENCRYPTION_ASE_KEY_OFFSET	(0x24)
#define EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK		(0x0000FF00)

#define EFUSE_ECC_KEY_ADDR_OFFSET				(0x34)
#define ECC_KEY_LEN								(0x8)
	

/*--------------------------------------------------------------------------
* 	                                        	Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

/*--------------------------------------------------------------------------
* 	                                        	Types
--------------------------------------------------------------------------*/
/**
 * @brief The brief description
 * @details The detail description
 */

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
// in bootrom
unsigned int efuse_read(unsigned int addr);
void efuse_read_series(unsigned int addr,unsigned int * value, unsigned int length);
void efuse_cfg_160m(void);

// add int uboot
void efuse_write(unsigned int addr, unsigned int value, unsigned int mask);
void efuse_write_series(unsigned int addr, unsigned int *value, unsigned int mask, unsigned int length);


#endif/*_EFUSE_H*/


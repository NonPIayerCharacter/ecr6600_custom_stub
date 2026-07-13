/**
 * @file efuse.c
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
 * @par History 2:
 */

/****************************************************************************
* 	                                           Include files
****************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "efuse.h"
#include "common_def.h"
#include "soc_top_reg.h"
#include "clk.h"
#include "pit.h"
#include "uart.h"
#include "uboot_init.h"

/****************************************************************************
* 	                                           Local Macros
****************************************************************************/

#define  MEM_BASE_EFUSE					SOC_EFUSE_BASE
/* efuse controller register*/
#define EFUSE_PGM_TIMING1              	(MEM_BASE_EFUSE + 0x00)
#define EFUSE_PGM_TIMING2               (MEM_BASE_EFUSE + 0x04)
#define EFUSE_PGM_TIMING3               (MEM_BASE_EFUSE + 0x08)
#define EFUSE_RD_TIMING1              	(MEM_BASE_EFUSE + 0x10)
#define EFUSE_RD_TIMING2              	(MEM_BASE_EFUSE + 0x18)
#define EFUSE_INIT_DONE              	(MEM_BASE_EFUSE + 0x1C)
#define EFUSE_RELOAD              		(MEM_BASE_EFUSE + 0x20)
#define EFUSE_PGM_MASK              	(MEM_BASE_EFUSE + 0x24)
#define EFUSE_INFO              		(MEM_BASE_EFUSE + 0x100)		//registers to store efuse information
#define EFUSE_PGM              			(MEM_BASE_EFUSE + 0x200)		//program efuse through write into the address
#define EFUSE_READ              		(MEM_BASE_EFUSE + 0x300)		//read raw efuse value from efuse macro



/* 0x00 - EFUSE_PGM_TIMING1 register */
#define T_SUP_A(x)      				(((x) & 0xFF) << 24)		//A9-A0 to STROBE setup time in PGM mode (17.345ns)
#define T_SUP_CS(x)       				(((x) & 0xFF) << 16)		//CSB to STROBE setup time into PGM mode (7.831ns)
#define T_SUP_PS(x)      				(((x) & 0xFF) << 8)			//PS to STROBE setup time in PGM mode (76.148ns)
#define T_SUP_PD(x)      				(((x) & 0xFF) << 0)			//PD to STROBE setup time in read mode  (1120.765ns)

/* 0x04 - EFUSE_PGM_TIMING2 register */
#define T_PGM(x)      					(((x) & 0xFFFF) << 0)		//typical program strobe pulse width (12us)

/* 0x08 - EFUSE_PGM_TIMING3 register */
#define T_HP_PD(x)      				(((x) & 0xFF) << 24)		//PD to STROBE hold time in PGM mode (94.599ns)
#define T_HP_PS(x)       				(((x) & 0xFF) << 16)		//PS to STROBE hold time in PGM mode (75.005ns)
#define T_HP_CS(x)      				(((x) & 0xFF) << 8)			//PS to CSB hold time in PGM mode (7.311)
#define T_HP_A(x)      					(((x) & 0xFF) << 0)			//A9-A0 to STROBE Hold time in PGM mode (16.708ns)

/* 0x10 - EFUSE_RD_TIMING1 register */
#define T_RD(x)      					(((x) & 0xFF) << 24)		//read strobe pulse width (200ns)
#define T_SUR_A(x)       				(((x) & 0xFF) << 16)		//A4-A0 to STROBE setup time in read mode (17.345ns)
#define T_SUR_CS(x)      				(((x) & 0xFF) << 8)			//CSB to STROBE setup time into read mode (7.831)
#define T_SUR_PD(x)      				(((x) & 0xFF) << 0)			//PD to STROBE setup time in read mode (1052.448)

/* 0x18 - EFUSE_RD_TIMING2 register */
#define T_HR_PD(x)       				(((x) & 0xFF) << 16)		//PD to STROBE hold time in read mode (26.905)
#define T_HR_CS(x)      				(((x) & 0xFF) << 8)			//CSB to STROBE hold time out of read mode (7.311) 
#define T_HR_A(x)      					(((x) & 0xFF) << 0)			//A4-A0 to STROBE hold time in read mode (16.708)

/* 0x1C - EFUSE_INIT_DONE register */
#define EFUSE_INIT_DONE_FLAG      		BIT(0)		//				//efuse initialize done,all bits stored into registers

/* 0x20 - EFUSE_RELOAD register */
#define RELOAD      					BIT(0)		//				//reload flag

/* 0x24 - EFUSE_PGM_MASK register */
//reserved



/****************************************************************************
* 	                                           Local Types
****************************************************************************/

/****************************************************************************
* 	                                           Local Constants
****************************************************************************/

/****************************************************************************
* 	                                           Local Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Global Constants
****************************************************************************/

/****************************************************************************
* 	                                          Global Variables
****************************************************************************/

/****************************************************************************
* 	                                          Global Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Function Definitions
****************************************************************************/

/*******************************************************************************
 * Function:     hal_efuse_read
 * Description:  
 * Parameters: 
 *   Input:
 *
 *   Output:
 *
 * Returns: 
 *
 *
 * Others: 
 ********************************************************************************/
//unsigned int efuse_read(unsigned int addr)
//{
//	int i = 100;
//	while(((inw(EFUSE_INIT_DONE)) & 0x1) == 0)
//	{
//		udelay(2);
//		if(i-- == 0)
//			break;
//	}
//
//	
//	if(addr >= 0x20 && addr <= 0x30){
//		return inw(EFUSE_INFO + addr);
//	}else{
//		return inw(EFUSE_READ + addr);
//	}
//	
//}

void efuse_write(unsigned int addr, unsigned int value, unsigned int mask)
{
	int i = 100; 	
	while(((inw(EFUSE_INIT_DONE)) & 0x1) == 0)
	{
		boot_usdelay(2);
		if(i-- == 0)
			break;
	}
	
	outw(EFUSE_PGM_MASK,mask);//MASK
	outw(EFUSE_PGM + addr ,value);
}


void  efuse_read_series(unsigned int addr,unsigned int * value, unsigned int length)
{
	unsigned int i= 100;
	int temp = 0;
	
	while(((inw(EFUSE_INIT_DONE)) & 0x1) == 0)
	{
		udelay(2);
		if(i-- == 0)
			break;
	}
			
	for(i = 0; i < length; i++)
	{
		if((addr + 4*i) >= 0x20 && (addr + 4*i) <= 0x30){
			temp = inw(EFUSE_INFO + addr + 4*i);
		}else{
			temp = inw(EFUSE_READ + addr + 4*i);
		}
		memcpy(&value[i], &temp, 4);
	}
	
}

void efuse_write_series(unsigned int addr, unsigned int *value, unsigned int mask, unsigned int length)
{
	unsigned int i = 0;
	int tmp = 0;

	while(((inw(EFUSE_INIT_DONE)) & 0x1) == 0)
	{
		boot_usdelay(2);
		if(i-- == 0)
			break;
	}
	
	outw(EFUSE_PGM_MASK,mask);//MASK

	for(i = 0; i < length; i++)
	{
		memcpy(&tmp, &value[i], 4);
		outw((EFUSE_PGM + addr +(4*i)),tmp);
	}
}

//void efuse_init(void)
//{
//	//enable clk
//	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, EFUSE_CLK_BIT | EFUSE_APB_CLK_BIT);
//}


//void efuse_cfg_160m(void)
//{
//	outw(EFUSE_PGM_TIMING1, 0x101042d);
//	outw(EFUSE_PGM_TIMING2, 0x1e0);
//	outw(EFUSE_PGM_TIMING3, 0x5040101);
//	outw(EFUSE_RD_TIMING1, 0x801012C);
//	outw(EFUSE_RD_TIMING2, 0x20101);
//}

//void show_all_efuse_data(void)
//{
//	unsigned int chip_data[8];
//	unsigned int efuse_ctrl = 0;
//	unsigned int flash_encryption[4];
//	unsigned int ecc_public_key[8];
//	unsigned int reserved[11];
//	
//	efuse_read_series(0, chip_data, 8);
//	efuse_ctrl = efuse_read(0x20);
//	efuse_read_series(0x24, flash_encryption, 4);
//	efuse_read_series(0x34, ecc_public_key, 8);
//	efuse_read_series(0x54, reserved, 11);
//
//	printf("chip data 0x00-0x1f\r\n0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\r\n\r\n",
//		chip_data[0],chip_data[1],chip_data[2],chip_data[3],chip_data[4],chip_data[5],chip_data[6],chip_data[7]);
//	
//	printf("efuse_ctrl  0x%08x\r\n\r\n",efuse_ctrl);
//	
//	printf("flash_encryption 0x24-0x33\r\n0x%08x  0x%08x  0x%08x  0x%08x\r\n\r\n",
//		flash_encryption[0],flash_encryption[1],flash_encryption[2],flash_encryption[3]);
//	
//	printf("ecc_public_key 0x34-0x53\r\n0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\r\n\r\n",
//		ecc_public_key[0],ecc_public_key[1],ecc_public_key[2],ecc_public_key[3],
//		ecc_public_key[4],ecc_public_key[5],ecc_public_key[6],ecc_public_key[7]);
//	
//	printf("chip data 0x54-0x7f\r\n0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\r\n\r\n",
//		reserved[0],reserved[1],reserved[2],reserved[3],
//		reserved[4],reserved[5],reserved[6],reserved[7],		
//		reserved[8],reserved[9],reserved[10]);
//}
//
//void efuse_reload(void)
//{
//
//	while(IN32(EFUSE_INIT_DONE) == 0);
//
//	OUT32(0x202014,0x6fffffff);
//	IN32(0x202014);					//Necessary operation
//	IN32(0x202014);					//Necessary operation
//	
//	OUT32(0x202014,0xefffffff);
//	
//	while(IN32(EFUSE_INIT_DONE) == 0);
//}
//
//void efuse_test(void)
//{
//	unsigned char ch;
//	unsigned char addr = 0x54;
//	while(1)
//	{
//		printf("0:show all\r\n");
//		printf("1-8:write bit\r\n");
//		printf("9:addr a++\r\n");
//		printf("a:EFUSE_LOG_MEMORY\r\n");
//		printf("b:efuse_write public Key\r\n");
//		printf("c:efuse_write EFUSE_SECURE_BOOT_EN\r\n");
//		printf("d:efuse_write EFUSE_CTRL_FLASH_ENCRYPT_EN\r\n");
//		printf("e:efuse_write aesKey\r\n");
//		printf("f:efuse_write EFUSE_CTRL_AES_KEY_READ_DIS\r\n");
//		printf("g:efuse_write EFUSE_CTRL_AES_KEY_WRITE_DIS\r\n");
//		printf("h:efuse_write EFUSE_JTAG_CTRL_AUTO\r\n");
//		printf("i:efuse_write EFUSE_JTAG_CTRL_DIS\r\n");
//		printf("q:exit\r\n");
//		uart_getc(SOC_UART0_BASE, (unsigned char *)&ch,0xFFFFFFFF);
//		if(ch == '0')
//		{
//			printf("input 0: show all\r\n");
//			show_all_efuse_data();
//		}
//		else if(ch == '1')
//		{
//			printf("input 1: addr=0x%02x, bit 0 write 1\r\n", addr);
//			efuse_write(addr, 0x00000001, 0xFFFFFFFE);
//			printf("write over\r\n");
//		}
//		else if(ch == '2')
//		{
//			printf("input 1: addr=0x%02x, bit 1 write 1\r\n", addr);
//			efuse_write(addr, 0x00000002, 0xFFFFFFFD);
//			printf("write over\r\n");
//		}
//		else if(ch == '3')
//		{
//			printf("input 1: addr=0x%02x, bit 2 write 1\r\n", addr);
//			efuse_write(addr, 0x00000004, 0xFFFFFFFB);
//			printf("write over\r\n");
//		}
//		else if(ch == '4')
//		{
//			printf("input 1: addr=0x%02x, bit 3 write 1\r\n", addr);
//			efuse_write(addr, 0x00000008, 0xFFFFFFF7);
//			printf("write over\r\n");
//		}
//		else if(ch == '5')
//		{
//			printf("input 1: addr=0x%02x, bit 4 write 1\r\n", addr);
//			efuse_write(addr, 0x00000010, 0xFFFFFFEF);
//			printf("write over\r\n");
//		}
//		else if(ch == '6')
//		{
//			printf("input 1: addr=0x%02x, bit 5 write 1\r\n", addr);
//			efuse_write(addr, 0x00000020, 0xFFFFFFDF);
//			printf("write over\r\n");
//		}
//		else if(ch == '7')
//		{
//			printf("input 1: addr=0x%02x, bit 6 write 1\r\n", addr);
//			efuse_write(addr, 0x00000040, 0xFFFFFFBF);
//			printf("write over\r\n");
//		}
//		else if(ch == '8')
//		{
//			printf("input 1: addr=0x%02x, bit 7 write 1\r\n", addr);
//			efuse_write(addr, 0x00000080, 0xFFFFFF7F);
//			printf("write over\r\n");
//		}
//		else if(ch == '9')
//		{
//			printf("input 8: addr++\r\n");
//			addr+=1;
//			printf("now addr=0x%02x\r\n", addr);
//		}
//		else if(ch == 'a')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_LOG_MEMORY, 0);
//			printf("EFUSE_LOG_MEMORY\r\n");
//		}
//		else if(ch == 'b')
//		{
//			unsigned char ecc_public_Key[32];
//			printf("input efuse_write public Key\r\n");
//			for(int i = 0; i < 32; i++)
//			{
//				uart_getc(SOC_UART0_BASE, (unsigned char *)&ecc_public_Key[i],0xFFFFFFFF);
//			}
//			efuse_write_series(EFUSE_ECC_KEY_ADDR_OFFSET, (unsigned int *)ecc_public_Key, 0, 8);
//			printf("efuse_write public Key\r\n");
//		}
//		else if(ch == 'c')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_SECURE_BOOT_EN, 0);
//			printf("efuse_write EFUSE_SECURE_BOOT_EN\r\n");
//		}
//		else if(ch == 'd')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_CTRL_FLASH_ENCRYPT_EN, 0);
//			printf("efuse_write EFUSE_CTRL_FLASH_ENCRYPT_EN\r\n");
//		}
//		else if(ch == 'e')
//		{
//			unsigned char aesKey[16];
//			printf("input efuse_write aesKey\r\n");
//			for(int i = 0; i < 16; i++)
//			{
//				uart_getc(SOC_UART0_BASE, (unsigned char *)&aesKey[i],0xFFFFFFFF);
//			}
//			efuse_write_series(EFUSE_FLASH_ENCRYPTION_ASE_KEY_OFFSET, (unsigned int *)aesKey, 0, 4);
//			printf("efuse_write aesKey\r\n");
//		}
//		else if(ch == 'f')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_CTRL_AES_KEY_READ_DIS, 0);
//			printf("efuse_write EFUSE_CTRL_AES_KEY_READ_DIS\r\n");
//		}
//		else if(ch == 'g')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_CTRL_AES_KEY_WRITE_DIS, 0);
//			printf("efuse_write EFUSE_CTRL_AES_KEY_WRITE_DIS\r\n");
//		}
//		else if(ch == 'h')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_JTAG_CTRL_AUTO, 0);
//			printf("efuse_write EFUSE_JTAG_CTRL_AUTO\r\n");
//		}
//		else if(ch == 'i')
//		{
//			efuse_write(EFUSE_CTRL_OFFSET, EFUSE_JTAG_CTRL_DIS, 0);
//			printf("efuse_write EFUSE_JTAG_CTRL_DIS\r\n");
//		}
//		else if(ch == 'q')
//		{
//			printf("input q: return\r\n");
//			return;
//		}
//		else
//		{
//			printf("input 0x%02x %c:\r\n", ch, ch);
//		}
//	}
//}



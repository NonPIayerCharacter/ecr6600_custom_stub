/*******************************************************************************
 * Copyright by Transa Semi.
 *
 * File Name:  drv_spiflash.h  
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       v0.1
 * Author:        wangchao
 * Date:          2019-1-4
 * History 1:      
 *     Date: 
 *     Version:
 *     Author: 
 *     Modification:  
 * History 2: 
  ********************************************************************************/

#ifndef	_DRV_SPIFLASH_H
#define _DRV_SPIFLASH_H

/****************************************************************************
* 	                                        Include files
****************************************************************************/
#include "spi.h"
#include "common_def.h"

/****************************************************************************
* 	                                        Macros
****************************************************************************/
#define SPI_TRANSCTRL_CMD_EN			(1<<30)
#define SPIFLASH_TIMEOUT_COUNT	0xFFFFFFFF

#define SPIFLASH_PAGE_SIZE		0x100	//256B
#define SPIFLASH_SECTOR_SIZE	0x1000 	// 4KB
#define SPIFLASH_HALF_BLOCK_SIZE 0x8000	// 32KB
#define SPIFLASH_BLOCK_SIZE		0x10000	// 64KB


#define BASE_FLASH_MEM			0x00800000
#define SPIFLASH_MEM_LENGTH	(0x00200000)  //2MB


/* 0x30 - spi control register */
#define SPI_CONTROL_SPIRST				BIT(0)
#define SPI_CONTROL_RXFIFORST			BIT(1)
#define SPI_CONTROL_TXFIFORST			BIT(2)
#define SPI_CONTROL_RXDMAEN				BIT(3)
#define SPI_CONTROL_TXDMAEN				BIT(4)

/* 0x34 - spi status register */
#define SPI_STATUS_BUSY					BIT(0)
#define SPI_STATUS_RXENPTY				BIT(14)
#define SPI_STATUS_TXFULL				BIT(23)

/* 0x38 - spi intr enable register */
#define SPI_INTR_EN_END					BIT(4)

/* 0x3C - spi intr status register */
#define SPI_INTR_STS_END				BIT(4)


/* 0x20 - spi transfer control register*/
#define SPI_TRANSCTRL_RCNT(x)			(((x) & 0x1FF) << 0)
#define SPI_TRANSCTRL_WCNT(x)			(((x) & 0x1FF) << 12)
#define SPI_TRANSCTRL_DUALQUAD(x)		(((x) & 0x3) << 22)
#define SPI_TRANSCTRL_TRAMODE(x)		(((x) & 0xF) << 24)

#define SPI_TRANSCTRL_DUALQUAD_REGULAR	SPI_TRANSCTRL_DUALQUAD(0)
#define SPI_TRANSCTRL_DUALQUAD_DUAL		SPI_TRANSCTRL_DUALQUAD(1)
#define SPI_TRANSCTRL_DUALQUAD_QUAD		SPI_TRANSCTRL_DUALQUAD(2)

#define SPI_TRANSCTRL_TRAMODE_WRCON		SPI_TRANSCTRL_TRAMODE(0)	/* w/r at the same time */
#define SPI_TRANSCTRL_TRAMODE_WO		SPI_TRANSCTRL_TRAMODE(1)	/* write only		*/
#define SPI_TRANSCTRL_TRAMODE_RO		SPI_TRANSCTRL_TRAMODE(2)	/* read only		*/
#define SPI_TRANSCTRL_TRAMODE_WR		SPI_TRANSCTRL_TRAMODE(3)	/* write, read		*/
#define SPI_TRANSCTRL_TRAMODE_RW		SPI_TRANSCTRL_TRAMODE(4)	/* read, write		*/
#define SPI_TRANSCTRL_TRAMODE_WDR		SPI_TRANSCTRL_TRAMODE(5)	/* write, dummy, read	*/
#define SPI_TRANSCTRL_TRAMODE_RDW		SPI_TRANSCTRL_TRAMODE(6)	/* read, dummy, write	*/
#define SPI_TRANSCTRL_TRAMODE_NONE		SPI_TRANSCTRL_TRAMODE(7)	/* none data */
#define SPI_TRANSCTRL_TRAMODE_DW		SPI_TRANSCTRL_TRAMODE(8)	/* dummy, write	*/
#define SPI_TRANSCTRL_TRAMODE_DR		SPI_TRANSCTRL_TRAMODE(9)	/* dummy, read	*/



#define SPI_TRANSCTRL_ADDR_EN			(1<<29)
#define SPI_TRANSCTRL_ADDR_FMT			(1<<28)
#define SPI_TRANSCTRL_TOKEN_EN			(1<<21)

#define SPI_TRANSCTRL_DUMMY_CNT_1		(0<<9)
#define SPI_TRANSCTRL_DUMMY_CNT_2		(1<<9)
#define SPI_TRANSCTRL_DUMMY_CNT_3		(2<<9)
#define SPI_TRANSCTRL_DUMMY_CNT_4		(3<<9)

/* SPI-FLASH COMMAND SET  */
#define SPIFLASH_CMD_WREN			0x06
#define SPIFLASH_CMD_RDID			0x9F
#define SPIFLASH_CMD_RDSR_1			0x05
#define SPIFLASH_CMD_RDSR_2			0x35
#define SPIFLASH_CMD_WRSR			0x01
#define SPIFLASH_CMD_WRSR_2			0x31

// Program instructions
#define SPIFLASH_CMD_PP				0x02
#define SPIFLASH_CMD_QUAD_INPUT_PP	0x32
// Erase instructions
#define SPIFLASH_CMD_SE				0x20
#define SPIFLASH_CMD_HE				0x52
#define SPIFLASH_CMD_BE				0xD8
#define SPIFLASH_CMD_CE				0xC7
// Read instructions
#define SPIFLASH_CMD_READ_NORMAL					0x03
#define SPIFLASH_CMD_READ_FAST						0x0B
#define SPIFLASH_CMD_READ_FAST_DUAL_OUTPUT			0x3B
#define SPIFLASH_CMD_READ_FAST_DUAL_IO				0xBB
#define SPIFLASH_CMD_READ_FAST_QUAD_OUTPUT			0x6B
#define SPIFLASH_CMD_READ_FAST_QUAD_IO				0xEB


#define SPIFLASH_CMD_ENTER_QPI		0x38
#define SPIFLASH_CMD_EXIT_QPI		0xFF


/* SPI-FLASH STATUS-1 REG */
#define SPIFLASH_STATUS_WIP			0x01
#define SPIFLASH_STATUS_WEL			0x02
#define SPIFLASH_STATUS_BPX			0x7C

/* SPI-FLASH STATUS-2 REG */
#define SPIFLASH_STATUS_QE			0x0200
#define SPIFLASH_STATUS_2_QE		0x02


/* SPI-FLASH STATUS-2 REG */
#define SPIFLASH_STATUS_RSTEN		0x66
#define SPIFLASH_STATUS_RST			0x99




/* FM */
#define FM25QH16B			0x1540a1 

/* GD */
#define GD25Q80E			0x1440c8
#define GD25LQ80C			0x1460C8
#define GD25Q16E			0x1540C8

/* huabang */
#define W25Q32JV			0x1640ef

/* PUYA */
#define P25Q80HD			0x146085
#define P25Q16H				0x156085
#define P25Q16SH			0x154285
#define P25Q32H				0x166085

/* TH */
#define TH25Q16HB			0x1560EB

/* XM */
#define XM25QH80B			0x144020
#define XM25QH16C			0x154020

/* XT */
#define XT25F08B			0x14400B
#define XT25F16F			0x15400B 

/* ZB */
#define ZB25VQ80			0x14605E
#define ZB25VQ16			0x15605E
#define ZB25VQ32			0x16405E

/* EN */
#define EN25Q16B			0x15701C
#define EN25QH32B			0x16701C


/* OTP instructions */
#define SPIFLASH_CMD_OTP_SE 			0x44
#define SPIFLASH_CMD_OTP_PP 			0x42
#define SPIFLASH_CMD_OTP_RD 			0x48
#define SPIFLASH_CMD_OTP_ENTRY			0x3A
#define SPIFLASH_CMD_OTP_EXIT			0x04



/****************************************************************************
* 	                                        Constants
****************************************************************************/
typedef enum
{
	SPI_MODE_STANDARD = 0,
	SPI_MODE_DUAL,
	SPI_MODE_QUAD,
	SPI_MODE_QPI,
	SPI_MODE_UNKNOW
} E_SPI_MODE;


typedef enum
{
	SPIFLASH_READ_CMD_03 = 0,
	SPIFLASH_READ_CMD_0B,
	SPIFLASH_READ_CMD_3B,
	SPIFLASH_READ_CMD_6B,
	SPIFLASH_READ_CMD_BB,
	SPIFLASH_READ_CMD_EB,
	SPIFLASH_READ_CMD_MAX
} E_SPIFLASH_READ_CMD;
	
typedef enum
{
	SPIFLASH_SCLK_DIV_0 = 0xFF,
	SPIFLASH_SCLK_DIV_2 = 0x00,
	SPIFLASH_SCLK_DIV_4 = 0x01,
	SPIFLASH_SCLK_DIV_6 = 0x02,
	SPIFLASH_SCLK_DIV_8 = 0x03,
	SPIFLASH_SCLK_DIV_10 = 0x04
} E_SPIFLASH_SCLK_DIV;
/****************************************************************************
* 	                                        Global  Variables
****************************************************************************/
extern unsigned int g_flash_id;

/****************************************************************************
* 	                                        Function Prototypes
****************************************************************************/
int spiFlash_format_none(int cmd);
void spiFlash_format_wr(int cmd, int data, int wr_len);
int spiFlash_format_rd(int cmd, int rd_len);




E_SPI_MODE spiFlash_api_init(E_SPI_SCLK_DIV clk_div, E_SPIFLASH_READ_CMD xip_rd_cmd);
int spiFlash_api_write(unsigned int addr, unsigned char * buf, unsigned int len);
void spiFlash_api_read(unsigned int addr, unsigned char * buf, unsigned int len);
int spiFlash_api_erase(unsigned int addr,  unsigned int len);
int spiFlash_api_erase_block(unsigned int addr, unsigned int len);
int spiFlash_api_erase_half_block(unsigned int addr, unsigned int len);
void spiFlash_api_chiperase(void);

int spi_bus_ready(unsigned int baseAddr);
int spi_fifo_reset(unsigned int baseAddr);
int spi_rx_ready(unsigned int baseAddr);
int spiFlash_rdid();
void spiFlash_cmd_se(unsigned int addr);
void spiFlash_cmd_otp_se(unsigned int addr);


//int spiFlash_OTP_Read(int addr,int length,unsigned char *pdata);
//void spiFlash_OTP_Lock(int LB);
//int spiFlash_OTP_Se(unsigned int addr);
//int spifiash_OTPWrite(unsigned int addr, unsigned int len, unsigned char * buf);
//void spiFlash_xip_read_mem(unsigned int addr, unsigned char * buf, unsigned int len);
//void hal_spiflash_init(void);
//void hal_spiflash_exit(void);
//void spiFlash_Qmode(void);


#endif/*_DRV_SPIFLASH_H*/


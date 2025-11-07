
#include <string.h>
#include <stdio.h>
#include "common_def.h"
#include "soc_top_reg.h"
#include "soc_pin_mux.h"
#include "drv_spiflash.h"
#include "spi.h"
#include "pit.h"
#include "clk.h"
#include "dbglog.h"
#include "uboot_init.h"
#include "flash_encrypt.h"
#include "uart.h"


/***********************************************************************
 *
 *							SPI -FLASH INFO
 *
 ************************************************************************/

// not supported by gd25lg80
//#define SPIFLASH_CMD_WRSR_2		0x31




E_SPI_MODE spi_mode = SPI_MODE_STANDARD;
unsigned int key[4] = {0x01234567,0x89abcdef,0x01234567,0x89abcdef};

void mdelay(void)
{
	unsigned int i =0;
	unsigned int j =0;
	
	for(i=0;i<50;i++)
	{
		j++;
	}
}


int spiFlash_format_none(int cmd)
{
	//wait spi-ilde
	if(!spi_bus_ready(SOC_SPI0_BASE))
	{
		return -1;
	}

	//clear rx & tx fifo
	if(spi_fifo_reset(SOC_SPI0_BASE))
	{
		return -2;
	}

	if (spi_mode == SPI_MODE_QPI)
	{
		outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_TRAMODE_WO | SPI_TRANSCTRL_DUALQUAD_QUAD );
		outw(SOC_SPI0_BASE+0x2C, cmd);
	}
	else
	{
		outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_TRAMODE_NONE);
		outw(SOC_SPI0_BASE+0x24, cmd);
	}

	return 0;
}


int spiFlash_format_rd(int cmd, int rd_len)
{
	unsigned int encrypt_en = IN32(SOC_PD_APB_ENCRYPT_EN);
	OUT32(SOC_PD_APB_ENCRYPT_EN, APB_WRIT_NOT_ENCRYPT);
	//wait spi-ilde
	if(!spi_bus_ready(SOC_SPI0_BASE))
	{
		OUT32(SOC_PD_APB_ENCRYPT_EN, encrypt_en);
		return -1;
	}

	//clear rx & tx fifo
	if(spi_fifo_reset(SOC_SPI0_BASE))
	{
		OUT32(SOC_PD_APB_ENCRYPT_EN, encrypt_en);
		return -2;
	}

	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_TRAMODE_RO|SPI_TRANSCTRL_RCNT(rd_len-1));
	outw(SOC_SPI0_BASE+0x24, cmd);

	//wait data ready
	if(!spi_rx_ready(SOC_SPI0_BASE))
	{
		OUT32(SOC_PD_APB_ENCRYPT_EN, encrypt_en);
		return -3;
	}
	
	OUT32(SOC_PD_APB_ENCRYPT_EN, encrypt_en);
	return inw(SOC_SPI0_BASE+0x2C);
}


void spiFlash_format_wr(int cmd, int data, int wr_len)
{
	char status;
	unsigned int encrypt_en = IN32(SOC_PD_APB_ENCRYPT_EN);
	OUT32(SOC_PD_APB_ENCRYPT_EN, APB_WRIT_NOT_ENCRYPT);
		
	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);

	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_TRAMODE_WO |SPI_TRANSCTRL_WCNT(wr_len-1));
	outw(SOC_SPI0_BASE+0x24, cmd);
	outw(SOC_SPI0_BASE+0x2C, data);

	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);
	
	OUT32(SOC_PD_APB_ENCRYPT_EN, encrypt_en);
}


/*****************************************************************************************
 *
 *   							 SPI-FLASH  CMD
 * 
******************************************************************************************/


void spiFlash_cmd_wrEnable(void)
{
	spiFlash_format_none(SPIFLASH_CMD_WREN);
}

int spiFlash_cmd_entry_QPI(void)
{
	return spiFlash_format_none(SPIFLASH_CMD_ENTER_QPI);
}

int spiFlash_cmd_exit_QPI(void)
{
	return spiFlash_format_none(SPIFLASH_CMD_EXIT_QPI);
}

int spiFlash_cmd_rdID(void)
{
	return spiFlash_format_rd(SPIFLASH_CMD_RDID, 3);
}

char spiFlash_cmd_rdSts0(void)
{
	return spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
}

char spiFlash_cmd_rdSts1(void)
{
	return spiFlash_format_rd(SPIFLASH_CMD_RDSR_2, 1);
}

void spiFlash_cmd_wrSts(int data)
{
	spiFlash_format_wr(SPIFLASH_CMD_WRSR, data, 2);
}
void spiFlash_cmd_wrSts1(int data)
{
	spiFlash_format_wr(SPIFLASH_CMD_WRSR_2, data, 1);
}

void spiFlash_cmd_se(unsigned int addr)
{

	char status;

	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);

	outw(SOC_SPI0_BASE+0x28, addr);	
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_SE);

	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);

}
void spiFlash_cmd_he(unsigned int addr)
{

	char status;

	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);

	outw(SOC_SPI0_BASE+0x28, addr);	
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_HE);

	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);

}


void spiFlash_cmd_be(unsigned int addr)
{

	char status;

	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);

	outw(SOC_SPI0_BASE+0x28, addr);	
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_BE);

	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);

}


void spiFlash_cmd_otp_se(unsigned int addr)
{
/*
	char status;

	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);
*/
	outw(SOC_SPI0_BASE+0x28, addr);	
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_ADDR_EN | SPI_TRANSCTRL_TRAMODE_NONE);
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_SE);
/*
	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);
*/
}


void spiFlash_cmd_ce(void)
{
	spiFlash_cmd_wrEnable();

//	spiFlash_format_none(SPIFLASH_CMD_WREN);
	
	spiFlash_format_none(SPIFLASH_CMD_CE);
}


int spiFlash_cmd_page_program(unsigned int addr, unsigned char * buf, unsigned int length)
{
	unsigned int i, left, spi_status;
//	unsigned int rdata = 0;
//	unsigned int k =0;
//	unsigned int j =0;

	spiFlash_format_none(SPIFLASH_CMD_WREN);

	//wait spi-ilde
	if(!spi_bus_ready(SOC_SPI0_BASE))
		return -1;
	
	//clear rx & tx fifo
	if(spi_fifo_reset(SOC_SPI0_BASE))
	{
		return -2;
	}

	if (spi_mode == SPI_MODE_QPI)
	{
		i = ((addr & 0xFF)<<24) | (((addr>>8)&0xFF)<<16) |(((addr>>16)&0xFF)<<8) | SPIFLASH_CMD_PP;
		outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_TRAMODE_WO |SPI_TRANSCTRL_WCNT(length + 3));
		outw(SOC_SPI0_BASE+0x24, 0);
		outw(SOC_SPI0_BASE+0x2C, i);
	}
	else
	{	
		outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_TRAMODE_WO |SPI_TRANSCTRL_WCNT(length - 1));
		outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_PP);
	}

	if((unsigned int)buf%4==0)
	{
		unsigned int *pdata = (unsigned int * )buf;
  		left = length/4;

		if(length%4)
			left++;

		for(i = 0; i< left; i++)
		{
			do
			{
				spi_status = inw(SOC_SPI0_BASE+0x34);
			} while(spi_status & SPI_STATUS_TXFULL);
			
			outw(SOC_SPI0_BASE+0x28, addr + i*4);
			outw(SOC_SPI0_BASE+0x2C, *pdata++);
			
		}
	}
	return 0;
}




int spiFlash_cmd_quad_input_page_program(unsigned int addr, unsigned char * buf, unsigned int length)
{
	unsigned int i, left, spi_status;
	spiFlash_format_none(SPIFLASH_CMD_WREN);

	//wait spi-ilde
	if(!spi_bus_ready(SOC_SPI0_BASE))
		return -1;

	//clear rx & tx fifo
	if(spi_fifo_reset(SOC_SPI0_BASE))
		return -2;

	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_TRAMODE_WO |SPI_TRANSCTRL_DUALQUAD_QUAD |SPI_TRANSCTRL_WCNT(length - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_QUAD_INPUT_PP);

	if((unsigned int)buf%4==0)
	{
		unsigned int *pdata = (unsigned int * )buf;
  		left = length/4;

		if(length%4)
			left++;

		for(i = 0; i< left; i++)
		{
			do
			{	
				spi_status = inw(SOC_SPI0_BASE+0x34);
			} while(spi_status & SPI_STATUS_TXFULL);
			
			outw(SOC_SPI0_BASE+0x28, addr + i*4);
			outw(SOC_SPI0_BASE+0x2C, *pdata++);
		}
	}

	return 0;
}





void spiFlash_cmd_read_normal(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_TRAMODE_RO |SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_NORMAL);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
	
}



void spiFlash_cmd_read_fast(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_TRAMODE_DR|SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_FAST);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
	
}


void spiFlash_cmd_read_fast_dual_output(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_TRAMODE_DR|SPI_TRANSCTRL_DUALQUAD_DUAL | SPI_TRANSCTRL_DUMMY_CNT_2 | SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_FAST_DUAL_OUTPUT);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
	
}

void spiFlash_cmd_read_fast_dual_io(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN | SPI_TRANSCTRL_ADDR_EN |SPI_TRANSCTRL_ADDR_FMT |SPI_TRANSCTRL_TRAMODE_DR | SPI_TRANSCTRL_DUALQUAD_DUAL | SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_FAST_DUAL_IO);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
}


void spiFlash_cmd_read_fast_quad_output(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN 					\
							| SPI_TRANSCTRL_ADDR_EN 				\
							| SPI_TRANSCTRL_TRAMODE_DR				\
							| SPI_TRANSCTRL_DUALQUAD_QUAD			\
							| SPI_TRANSCTRL_DUMMY_CNT_4			\
							| SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_FAST_QUAD_OUTPUT);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
	
}


void spiFlash_cmd_read_fast_quad_io(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int i,rx_num;
	unsigned int data = (len + 3)/4;	//word aligned
	unsigned int * rxBuf = (unsigned int *)buf;

	spi_fifo_reset(SOC_SPI0_BASE);
	
	outw(SOC_SPI0_BASE+0x28, addr);
	outw(SOC_SPI0_BASE+0x20, SPI_TRANSCTRL_CMD_EN 					\
							| SPI_TRANSCTRL_ADDR_EN 				\
							| SPI_TRANSCTRL_ADDR_FMT				\
							| SPI_TRANSCTRL_TRAMODE_DR				\
							| SPI_TRANSCTRL_DUALQUAD_QUAD			\
							| SPI_TRANSCTRL_DUMMY_CNT_3			\
							| SPI_TRANSCTRL_RCNT(len - 1));
	outw(SOC_SPI0_BASE+0x24, SPIFLASH_CMD_READ_FAST_QUAD_IO);

	while((inw(SOC_SPI0_BASE + 0x34) & SPI_STATUS_BUSY) == 1)
	{
		rx_num = ((inw(SOC_SPI0_BASE + 0x34)) >> 8) & 0x1F;
		rx_num = MIN(rx_num , data);

		for(i=0; i<rx_num; i++)
		{
			*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
		}

		data -= rx_num;

		if(data == 0)
		{
			return;
		}
	}

	for(i=0; i<data; i++)
	{
		*rxBuf++ = inw(SOC_SPI0_BASE + 0x2C);
	}
	
}


void spiFlash_xip_read_cfg(E_SPIFLASH_READ_CMD xip_rd_cmd)
{
	outw(SOC_SPI0_BASE + 0x50, xip_rd_cmd);
}

void spiFlash_xip_read_mem(unsigned int addr, unsigned char * buf, unsigned int len)
{
	memcpy((void *)buf, (void *)(addr + BASE_FLASH_MEM), len);
}

/*****************************************************************************************
 *
 *   							 SPI-FLASH  API
 * 
******************************************************************************************/
int spiFlash_api_erase(unsigned int addr, unsigned int len)
{
	char status;
	unsigned int eraseSectorCnt, i;

	if(addr%SPIFLASH_SECTOR_SIZE)
	{
		return 1;
	}

	eraseSectorCnt = (unsigned int)((len+SPIFLASH_SECTOR_SIZE-1)/SPIFLASH_SECTOR_SIZE);
	
	for(i=0; i<eraseSectorCnt; i++)
	{
		do
		{
			spiFlash_format_none(SPIFLASH_CMD_WREN);
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WEL) == 0);

		spiFlash_cmd_se(addr + i * SPIFLASH_SECTOR_SIZE);

		do
		{
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WIP) == 1);
	}
	return 0;
}

int spiFlash_api_erase_half_block(unsigned int addr, unsigned int len)
{
	char status;
	unsigned int eraseSectorCnt, i;

	if(addr%SPIFLASH_HALF_BLOCK_SIZE)
	{
		return 1;
	}

	eraseSectorCnt = (unsigned int)((len+SPIFLASH_HALF_BLOCK_SIZE-1)/SPIFLASH_HALF_BLOCK_SIZE);
	
	for(i=0; i<eraseSectorCnt; i++)
	{
		do
		{
			spiFlash_format_none(SPIFLASH_CMD_WREN);
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WEL) == 0);

		spiFlash_cmd_he(addr + i * SPIFLASH_HALF_BLOCK_SIZE);

		do
		{
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WIP) == 1);
	}
	return 0;
}


int spiFlash_api_erase_block(unsigned int addr, unsigned int len)
{
	char status;
	unsigned int eraseSectorCnt, i;

	if(addr%SPIFLASH_BLOCK_SIZE)
	{
		return 1;
	}

	eraseSectorCnt = (unsigned int)((len+SPIFLASH_BLOCK_SIZE-1)/SPIFLASH_BLOCK_SIZE);
	
	for(i=0; i<eraseSectorCnt; i++)
	{
		do
		{
			spiFlash_format_none(SPIFLASH_CMD_WREN);
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WEL) == 0);

		spiFlash_cmd_be(addr + i * SPIFLASH_BLOCK_SIZE);

		do
		{
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WIP) == 1);
	}
	return 0;
}



void spiFlash_api_chiperase(void)
{
	char status;

	do
	{
		spiFlash_format_none(SPIFLASH_CMD_WREN);
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WEL) == 0);
#if 1
	spiFlash_cmd_ce();
#else
	OUT32(SPI2_BASE+0x20, SPI_TRANSCTRL_CMD_EN |SPI_TRANSCTRL_TRAMODE_NONE);
	OUT32(SPI2_BASE+0x24, SPIFLASH_CMD_CE);
#endif

	do
	{
		status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
	} while((status & SPIFLASH_STATUS_WIP) == 1);
}

void spiFlash_api_read(unsigned int addr, unsigned char * buf, unsigned int len)
{
#if 1
	spiFlash_xip_read_mem(addr, (unsigned char *)buf, len);
#else
	unsigned int curr = 0;
	unsigned int curr_off = 0;
	unsigned int curr_len = 0;

	curr = len;
	do
	{
		curr_len = MIN(0x200, curr);
		spiFlash_cmd_read_fast_quad_io(addr + curr_off, (unsigned char *)(&buf[curr_off]), curr_len);
		curr -= curr_len;
		curr_off += curr_len;
	}while(curr);
#endif
}

int spiFlash_api_write(unsigned int addr, unsigned char * buf, unsigned int len)
{
	int ret = -1;
	unsigned int address, length;
	char status;

	if(len == 0)
	{
		return 0;
	}

	length = addr % SPIFLASH_PAGE_SIZE;

	if(len > (SPIFLASH_PAGE_SIZE - length))
		length = SPIFLASH_PAGE_SIZE - length;
	else
		length = len;

	address = addr;

	do {

		do
		{
			spiFlash_format_none(SPIFLASH_CMD_WREN);
			
			if(g_flash_id == EN25QH32B || g_flash_id == EN25Q16B)
			{
				status = SPIFLASH_STATUS_WEL;
				//break;
			}
			else
			{
				status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
			}
		} while((status & SPIFLASH_STATUS_WEL) == 0);

		switch (spi_mode)
		{
			case SPI_MODE_STANDARD:
			case SPI_MODE_DUAL:
			case SPI_MODE_QPI:
				ret = spiFlash_cmd_page_program(address, buf+(address - addr), length);
				break;

			case SPI_MODE_QUAD:
				ret = spiFlash_cmd_quad_input_page_program(address, buf+(address - addr), length);
				break;

			default:
			//	printf("==>>spiflash write in unknow mode\r\n");
				while(1);
		}

		
		if (ret != 0)
		{
			return ret;
		}

		len -= length;
		address += length;

		if(len > SPIFLASH_PAGE_SIZE)
			length = SPIFLASH_PAGE_SIZE;
		else
			length = len;

		do
		{
			status = spiFlash_format_rd(SPIFLASH_CMD_RDSR_1, 1);
		} while((status & SPIFLASH_STATUS_WIP) == 1);
		
	}while(length);

	return 0;
}

//void spiFlash_api_select_mode(E_SPI_MODE mode)
//{
//	int status = 0;
//
//	if (spi_mode == mode)
//	{
//		return;
//	}
//
//	if (mode == SPI_MODE_QPI)
//	{
//		if (spi_mode != SPI_MODE_QUAD)
//		{
//			status |= (SPIFLASH_STATUS_QE<<8);
//			spiFlash_format_wr(SPIFLASH_CMD_WRSR, status, 2);
//		}
//
//		spiFlash_format_none(SPIFLASH_CMD_ENTER_QPI);
//	}
//	else
//	{
//		if (spi_mode == SPI_MODE_QPI)
//		{
//			spiFlash_format_none(SPIFLASH_CMD_EXIT_QPI);
//			spi_mode = SPI_MODE_QUAD;
//		}
//
//		if (mode == SPI_MODE_STANDARD ||mode == SPI_MODE_DUAL)
//		{
//			if (spi_mode == SPI_MODE_QUAD)
//			{
//				spiFlash_format_wr(SPIFLASH_CMD_WRSR, status, 2);
//			}
//			
//			//printf("(((((((((((((((((((((spiflash STAND mode\r\n");
//		}
//		else if (mode == SPI_MODE_QUAD)
//		{
//			if (spi_mode == SPI_MODE_QUAD)
//			{
//				return;
//			}
//			//printf("(((((((((((((((((((((spiflash Q mode\r\n");
//			spiFlash_cmd_wrEnable();
//			status |= (SPIFLASH_STATUS_QE<<8);
//			spiFlash_format_wr(SPIFLASH_CMD_WRSR, status, 2);
//			
//			//printf("status = 0x%x\r\n",status);
//			//printf("==>> spiflash sts-1: 0x%x\r\n", spiFlash_cmd_rdSts0());
//			//printf("==>> spiflash sts-2: 0x%x\r\n", spiFlash_cmd_rdSts1());
//		}
//	}
//		
//	spi_mode = mode;
//	return;
//}


unsigned int g_flash_id = 0;

E_SPI_MODE spiFlash_api_init(E_SPI_SCLK_DIV clk_div, E_SPIFLASH_READ_CMD xip_rd_cmd)
{
	//char status;
	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, SPI0_APB_CLK_BIT | FLASH_AHB_CLK_BIT);
	OUT32(SOC_PD_SMU_BASE + 0x18, 0x80);	// spi0 work clk choose ahb clk

	PIN_FUNC_SET(IO_MUX0_GPIO7, FUNC_GPIO7_MSPI_MOSI);
	PIN_FUNC_SET(IO_MUX1_GPIO8, FUNC_GPIO8_MSPI_HOLD);
	PIN_FUNC_SET(IO_MUX1_GPIO9, FUNC_GPIO9_MSPI_CLK);
	PIN_FUNC_SET(IO_MUX1_GPIO10, FUNC_GPIO10_MSPI_CS0);
	PIN_FUNC_SET(IO_MUX1_GPIO11, FUNC_GPIO11_MSPI_MISO);
	PIN_FUNC_SET(IO_MUX1_GPIO12, FUNC_GPIO12_MSPI_WP);

	/* flash reset */
	spiFlash_format_none(SPIFLASH_STATUS_RSTEN);
	spiFlash_format_none(SPIFLASH_STATUS_RST);
	/* udelay 4ms for TH25Q16HB */
	boot_usdelay(30);
	
	/* cfg spi clk */
	OUT32(SOC_SPI0_BASE + 0x40, (IN32(SOC_SPI0_BASE + 0x40) & 0xFFFFFF00) | clk_div);

	/* cfg xip read-mem-cmd */
	// CONFIG_STANDARD_SPI:0B  CONFIG_DUAL_SPI:BB  CONFIG_QUAD_SPI:EB
	OUT32(SOC_SPI0_BASE + 0x50, (IN32(SOC_SPI0_BASE + 0x50)&0xf) | xip_rd_cmd);  
	
	g_flash_id = spiFlash_cmd_rdID() & 0x00FFFFFF;
	
	if(xip_rd_cmd == SPIFLASH_READ_CMD_EB && g_flash_id!=0xFFFFFF)
	{
		//				P25Q80HD				GD25Q80E	   			GD25Q16E				XT25F16F				XT25F08B				TH25Q16HB	
		if(g_flash_id == 0x146085 || g_flash_id == 0x1440c8 || g_flash_id == 0x1540C8 || g_flash_id == 0x15400b || g_flash_id == 0x14400b || g_flash_id == 0x1560EB) 
		{
			spiFlash_cmd_wrSts(SPIFLASH_STATUS_QE);
		}
		//					P25Q16H
		else if(g_flash_id == 0x156085)  // Special adaptation 
		{
			spiFlash_cmd_wrSts(SPIFLASH_STATUS_QE);
			spiFlash_cmd_wrSts1(SPIFLASH_STATUS_2_QE);
		}
		//					EN25QH32B
		else if(g_flash_id == 0x16701C)  // Special adaptation 
		{
			;
		}
		else
		{
			spiFlash_cmd_wrSts1(SPIFLASH_STATUS_2_QE);
		}
		spi_mode = SPI_MODE_QUAD;
	}
	else
	{
		spi_mode = SPI_MODE_STANDARD;
	}

#if 0
	while(flash_id==0xFFFFFF)
	{
		uart_write((unsigned char *)"no flash!", 9);
		boot_usdelay(1000000);
	}
#else
	if(g_flash_id==0xFFFFFF)
	{
		uart_write((unsigned char *)"no flash!", 9);
	}
#endif

	return spi_mode;
}




//void spiFlash_Qmode(void)
//{
//	unsigned int j, data;
//	spi_reg * pSpiReg = (spi_reg *)SOC_SPI0_BASE;
//	
//	for(j = 0; j<SPIFLASH_TIMEOUT_COUNT; j++)
//	{
//		spiFlash_format_none(SPIFLASH_CMD_WREN);
//		data = spiFlash_cmd_rdSts0();
//		if(data & SPIFLASH_STATUS_WEL)
//			break;
//	}
//
//	if((data & SPIFLASH_STATUS_WEL) == 0)
//	{
//		DBGLOG_ERROR("Flash_mode fail!\n");
//	}
//	
//	int FlashID = spiFlash_rdid() & (~(0xFF<<24));
//	//add XT25F16b 0x15400b
//	if(FlashID == 0x146085 || FlashID == 0x1440c8 || FlashID == 0x15400b || FlashID == 0x156085 || FlashID == 0x14400b)//P25Q80H & GD25Q80E & P25Q16H & XT25F08 & XT25F16
//	{
//		pSpiReg->transCtrl = SPI_TRANSCTRL_CMD_EN 
//					|SPI_TRANSCTRL_TRAMODE_WO
//					|SPI_TRANSCTRL_WCNT(1);
//		pSpiReg->cmd = SPIFLASH_CMD_WRSR;
//	
//		SPI_WAIT_TX_READY(pSpiReg->status);
//		pSpiReg->data = 0x0200;
//	}
//	else
//	{
//		pSpiReg->transCtrl = SPI_TRANSCTRL_CMD_EN 
//					|SPI_TRANSCTRL_TRAMODE_WO
//					|SPI_TRANSCTRL_WCNT(0);
//		pSpiReg->cmd = SPIFLASH_CMD_WRSR_2;
//
//		SPI_WAIT_TX_READY(pSpiReg->status);
//		pSpiReg->data = 0x02;
//	}
//	for(j = 0; j<SPIFLASH_TIMEOUT_COUNT; j++)
//	{
//		data = spiFlash_cmd_rdSts0();
//		if((data & SPIFLASH_STATUS_WIP) == 0)
//			break;
//	}	
//	if(data & SPIFLASH_STATUS_WIP)
//	{
//		DBGLOG_ERROR("Flash_mode fail!\n");
//	}	
//	
//}



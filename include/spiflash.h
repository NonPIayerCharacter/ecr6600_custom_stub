/*******************************************************************************
 * Copyright by http://eswin.com/.
 *
 * File Name:    
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       V0.1
 * Author:        liuyong
 * Date:          2020-12-28
 * History 1:      
 *     Date: 
 *     Version:
 *     Author: 
 *     Modification:  
 * History 2: 
  ********************************************************************************/

#ifndef _SPIFLASH_H
#define _SPIFLASH_H


/****************************************************************************
* 	                                        Include files
****************************************************************************/
#include "spi.h"
#include "drv_spiflash.h"


/****************************************************************************
* 	                                        Macros
****************************************************************************/


/****************************************************************************
* 	                                        Types
****************************************************************************/

/****************************************************************************
* 	                                        Constants
****************************************************************************/

/****************************************************************************
* 	                                        Global  Variables
****************************************************************************/

/****************************************************************************
* 	                                        Function Prototypes
****************************************************************************/
int spi_bus_ready(unsigned int baseAddr);
int spi_fifo_reset(unsigned int baseAddr);
int spi_rx_ready(unsigned int baseAddr);
int spiFlash_rdid(void);
int spiFlash_rdsr(void);
int spiFlash_wren(void);
int spiFlash_se(unsigned int addr);
int xip_read(unsigned int addr, unsigned char * buf, unsigned int len);
int spiFlash_RDP(void);
void spiFlash_init(E_SPI_SCLK_DIV clk_div, E_SPIFLASH_READ_CMD rdCmd);
int spiflash_erase(unsigned int addr,  unsigned int len);



#endif/*_SPIFLASH_H*/


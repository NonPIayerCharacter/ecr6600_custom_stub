/*******************************************************************************
 * Copyright by Transa Semi.
 *
 * File Name: uart.h   
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       v0.1
 * Author:        wangxia
 * Date:          2019-5-29
 * History 1:      
 *     Date: 
 *     Version:
 *     Author: 
 *     Modification:  
 * History 2: 
  ********************************************************************************/

#ifndef _UART_H
#define _UART_H


/****************************************************************************
* 	                                        Include files
****************************************************************************/
#include "soc_top_reg.h"

/****************************************************************************
* 	                                        Macros
****************************************************************************/
#define UART_BAUD_76800					76800
#define UART_BAUD_115200				115200
#define UART_BAUD_380400				380400
#define UART_BAUD_460800				460800
#define UART_BAUD_576000				576000
#define UART_BAUD_691200				691200
#define UART_BAUD_806400				806400
#define UART_BAUD_921600				921600
#define UART_BAUD_1500000				1500000
#define UART_BAUD_2000000				2000000

#define UART_ID_0						0
#define UART_ID_1						1
#define UART_ID_2						2

#define Uart_base 						SOC_UART0_BASE

/* SD_LCR_DLAB == 0 */
#define UARTC_IDREV_REG	       			(Uart_base + 0x00)
#define UARTC_HWCFGR_REG		      	(Uart_base + 0x10)
#define UARTC_OSCR_REG        			(Uart_base + 0x14)
#define UARTC_RBR_REG                	(Uart_base + 0x20) /* receiver biffer register */
#define UARTC_DLL_REG	         		(Uart_base + 0x20)
#define UARTC_THR_REG                	(Uart_base + 0x20) /* transmitter holding register */
#define UARTC_IER_REG	                (Uart_base + 0x24) /* interrupt enable register */
#define UARTC_DLM_REG	         		(Uart_base + 0x24)
#define UARTC_IIR_REG	                (Uart_base + 0x28) /* interrupt identification register */
#define UARTC_FCR_REG	                (Uart_base + 0x28) /* FIFO control register */
#define UARTC_LCR_REG	                (Uart_base + 0x2c) /* line control regitser */
#define UARTC_MCR_REG	                (Uart_base + 0x30) /* modem control register */
#define UARTC_LSR_REG	                (Uart_base + 0x34) /* line status register */
#define UARTC_TST_REG	                (Uart_base + 0x34) /* testing register */
#define UARTC_MSR_REG	                (Uart_base + 0x38) /* modem status register */
#define UARTC_SPR_REG	                (Uart_base + 0x3c) /* scratch pad register */

#define DRV_UART_FCR_FIFORST			0x00000007	/** FIFO RESET*/

/* SD_LCR_DLAB == 0 */
#define UARTC_DLL_REG	                (Uart_base + 0x20) /* baudrate divisor latch LSB */
#define UARTC_DLM_REG	                (Uart_base + 0x24) /* baudrate divisor latch MSB */
#define UARTC_PSR_REG                	(Uart_base + 0x28) /* prescaler register */


/************************************
 *  TARGET UART CONSTANTS           *
 ************************************/
#define TX_FIFO_MAX 					(16)
#define UART_CMD_WAIT_TIMEOUT_MAX    	0xFFFFFFFF


/* UART register offsets (4~8-bit width) */
#define UARTC_LSR_RDR                   0x1 /* Data Ready */
#define UARTC_LSR_THRE                  0x20 /* THR/FIFO Empty */
#define UARTC_LSR_TEMT                  0x40 /* THR/TSR Empty */


#define UART_INT_NONE					0
#define UART_INT_ERROR					1
#define UART_INT_TIMEOUT				2
#define UART_INT_RX_DONE				3
#define UART_INT_TX_EMPTY				4
#define UART_INT_MAX					5


#define UART_DATA_BIT_5					0
#define UART_DATA_BIT_6					1
#define UART_DATA_BIT_7					2
#define UART_DATA_BIT_8					3

#define UART_PARITY_NONE				0
#define UART_PARITY_ODD					1
#define UART_PARITY_EVEN				2


#define UART_STOP_BIT_1					0


/*  The num of stop bit is based on the databits setting. 
  *      When databits = 0, stop bit is 1.5bits 
  *      When databits = 1,2,3, stop bit is 2bits
  */
#define UART_STOP_BIT_OTHER				1  
#define UART_INT_ENABLE					1
#define UART_INT_DISABLE				0


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
unsigned char uart_get_intType(unsigned int uart_base);
void uart_set_baudrate(unsigned int uart_base, unsigned int baud, unsigned int cond);
void uart_set_lineControl(unsigned int uart_base, unsigned int databits, unsigned int parity, unsigned int stopbits, unsigned int bc);
void uart_set_fifoControl(unsigned int uart_base, unsigned int tFifoRst, unsigned int rFifoRst,unsigned int fifoEn);
void uart_put_char(unsigned int uart_base, const char buf);
int uart_data_tstc(unsigned int uart_base);
int uart_getc(unsigned int uart_base, unsigned char *s, unsigned int timeout);
void uart_read(char *pBuf, int len);
void uart_data_write(unsigned int uart_base, unsigned char * pBuf, unsigned int len);
void uart_write(unsigned char * pBuf, unsigned int len);
void uart_init(unsigned int uart_base);


#endif/*_UART_H*/


/**
 * @file filename
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-3-18
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


#ifndef _UBOOT_INIT_H
#define _UBOOT_INIT_H


/*--------------------------------------------------------------------------
* 	                                        	Include files
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                        	Macros
--------------------------------------------------------------------------*/
/** Description of the macro */
#if 1
#define CPU_CLK_SET_160M

#ifdef CPU_CLK_SET_160M
	#define PIT_DIVISION 40U
#else
	#define PIT_DIVISION 6.5F
#endif

#else
#define CPU_CLK_SET_240M

#ifdef CPU_CLK_SET_240M
	#define PIT_DIVISION 60U
#else
	#define PIT_DIVISION 6.5F
#endif

#endif



/*--------------------------------------------------------------------------
* 	                                        	Types
--------------------------------------------------------------------------*/
/**
 * @brief The brief description
 * @details The detail description
 */
 
#define BOOT_REASON_ADDR 	(0x201234)
#define BOOT_REASON_BIT 	(0x0)
#define BOOT_REASON_MASK 	(0xF)

typedef enum {
    RST_TYPE_POWER_ON = 0,
    RST_TYPE_FATAL_EXCEPTION, 
    RST_TYPE_SOFTWARE_REBOOT,
    RST_TYPE_HARDWARE_REBOOT,
    RST_TYPE_OTA,
    RST_TYPE_WAKEUP,
    RST_TYPE_SOFTWARE, // start from app, GPIO holdon
    RST_TYPE_HARDWARE_WDT_TIMEOUT, //reserve
    RST_TYPE_SOFTWARE_WDT_TIMEOUT, //reserve
	RST_TYPE_UNKOWN
} RST_TYPE;

/*--------------------------------------------------------------------------
* 	                                        	Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                        	Global  Variables
--------------------------------------------------------------------------*/
/**  Description of global variable  */
extern unsigned int g_efuse_ctrl;

/*--------------------------------------------------------------------------
* 	                                        	Function Prototypes
--------------------------------------------------------------------------*/
int count_one_bits(unsigned int n);
void boot_soc_update_cpu_clk(void);
void boot_uart_set_baudrate(unsigned int uart_base, unsigned int baud, unsigned int cond);
void flash_encrypt_write_check();
void boot_usdelay(unsigned us);
void efuse_data_in_agc_memory_refresh(void);







#endif/*_UBOOT_INIT_H*/


/**
 * @file main(stub)
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-1-20
 * @version V1.0
 * @par Copyright by http://eswin.com/.
 * @par History 1:
 *      Date:2021-9-11
 *      Version:V0.2
 *      Author:liuyong1@eswin.com
 *      Modification:for eco3
 * 
 * @par History 2:
 */


/*--------------------------------------------------------------------------
* 	                                           Include files
--------------------------------------------------------------------------*/
#include "common_def.h"
#include "uboot_init.h"
#include "uboot_cmd.h"
#include "uart.h"
#include "clk.h"
#include "pit.h"


/*--------------------------------------------------------------------------
* 	                                           Local Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

/*--------------------------------------------------------------------------
* 	                                           Local Types
--------------------------------------------------------------------------*/
/**
 * @brief The brief description
 * @detailsThe detail description
 */

/*--------------------------------------------------------------------------
* 	                                           Local Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                           Local Function Prototypes
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          Global Constants
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          Global Variables
--------------------------------------------------------------------------*/
/**  Description of global variable  */

/*--------------------------------------------------------------------------
* 	                                          Global Function Prototypes
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          Function Definitions
--------------------------------------------------------------------------*/
/**
 * @brief mian.
 * @details The detail function description.
 * @note Description of function note
 */
int boot_main(void)
{
#if 0
	while(1)
	{
		uart_write((unsigned char *)"stub", 4);
		udelay(10000);
	}
#else
#if defined(CPU_CLK_SET_160M) || defined(CPU_CLK_SET_240M)
	// 提升主频至 160M 或 240M
	boot_soc_update_cpu_clk();
#endif



	// flash加密写检测
	flash_encrypt_write_check();



	// 工具命令解析
	uart_cmd_parser();
#endif


	while(1);
	return 0;
}


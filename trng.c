/**
 * @file filename
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
#include "trng.h"
#include "soc_top_reg.h"
#include "clk.h"
#include "pit.h"
#include "uboot_init.h"


/*--------------------------------------------------------------------------
* 	                                           	Local Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

/*--------------------------------------------------------------------------
* 	                                           	Local Types
--------------------------------------------------------------------------*/
/**
 * @brief The brief description
 * @details The detail description
 */

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
T_TRNG_REG_MAP *trng_reg_base = (T_TRNG_REG_MAP *)MEM_BASE_CNT_TRNG;


/*--------------------------------------------------------------------------
* 	                                          	Function Definitions
--------------------------------------------------------------------------*/

/** @brief	  Get a true random number
*	@details  After reading the random number for the first time, and abandon
*	@return   The true random number is 32 bits
*/
unsigned int boot_trng_get(void)
{	
	volatile unsigned int tmpdata = 0;

	/**1.enable clk*/
	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, TRNG_CLK_BIT);
	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, TRNG_APB_CLK_BIT);
	trng_reg_base->trng_clk_gating_en = DRV_TRNG_ENABLE;  ///< enable gating clk
 
	/**2.read data once,and abandon*/
	trng_reg_base->trng_trng_val;
	boot_usdelay(1);

	/**3.read data*/
	tmpdata = trng_reg_base->trng_trng_val;
	
	/**4.disabel clk*/
	trng_reg_base->trng_clk_gating_en = DRV_TRNG_DISABLE; ///< disable gating clk

	CLK_DISABLE(SOC_PD_CLK_EN_BASE0, TRNG_APB_CLK_BIT);
	CLK_DISABLE(SOC_PD_CLK_EN_BASE0, TRNG_CLK_BIT);

	return tmpdata;
}





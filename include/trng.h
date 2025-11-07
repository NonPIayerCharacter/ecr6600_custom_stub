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


#ifndef _TRNG_H
#define _TRNG_H


/*--------------------------------------------------------------------------
* 	                                        	Include files
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                        	Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

#define MEM_BASE_CNT_TRNG 	(SOC_TRNG_BASE + 0x40)

/**enable&disable trng work clock*/
#define DRV_TRNG_ENABLE   	0x1
#define DRV_TRNG_DISABLE  	0x0

/*--------------------------------------------------------------------------
* 	                                        	Types
--------------------------------------------------------------------------*/

/**
*@brief   Random number register structure 
*/
typedef  volatile struct _T_TRNG_REG_MAP
{
	/**0x00，trng work clock enable*/
	unsigned int trng_clk_gating_en;   
	unsigned int trng_trng_val; 	   ///< 0x04，The value of a true random number
	unsigned int trng_prng_val; 	   ///< 0x08，The value of a Pseudo-random number
	unsigned int process_sensor_shift_val;///< 0x0c，process sensor shift val
	unsigned int dig_ps_hdsw_en;		///< 0x10，process sensor enable
} T_TRNG_REG_MAP;

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
unsigned int boot_trng_get(void);



#endif/*_TRNG_H*/


/**
 * @file filename
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-8-11
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


#ifndef _FLASH_ENCRYPT_H
#define _FLASH_ENCRYPT_H


/*--------------------------------------------------------------------------
* 	                                        	Include files
--------------------------------------------------------------------------*/
#include "stdio.h"

/*--------------------------------------------------------------------------
* 	                                        	Macros
--------------------------------------------------------------------------*/
/** Description of the macro */

#define APB_WRIT_ENCRYPT			(0x1)
#define APB_WRIT_NOT_ENCRYPT		(0x0)

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
void flash_unencrypted_to_encrypt_check();




#endif/*_FLASH_ENCRYPT_H*/


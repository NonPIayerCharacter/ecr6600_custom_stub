/*******************************************************************************
 * Copyright by Transa Semi.
 *
 * File Name:soc_pin_mux.h    
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       v0.1
 * Author:        wangxia
 * Date:          2018-12-19
 * History 1:      
 *     Date: 2019-3-6
 *     Version:
 *     Author: wangxia
 *     Modification: add TR6260S/TR6260_3 pin_mux define 
 * History 2: 
  ********************************************************************************/
#ifndef _SOC_PIN_MUX_H
#define _SOC_PIN_MUX_H


/****************************************************************************
* 	                                        Include files
****************************************************************************/

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
#define GPIO_IDREV_OFFSET       0x00
#define GPIO_CONFIG_OFFSET      0x10
#define GPIO_DIN_OFFSET         0x20
#define GPIO_DOUT_OFFSET        0x24
#define GPIO_DIR_OFFSET         0x28
#define GPIO_DCLEAR_OFFSET      0x2C
#define GPIO_DSET_OFFSET        0x30
#define GPIO_PE_OFFSET          0x40
#define GPIO_PT_OFFSET          0x44
#define GPIO_IE_OFFSET          0x50
#define GPIO_IEMODE_OFFSET(N)   (0x54 + (N) * 4)
#define GPIO_IS_OFFSET          0x64
#define GPIO_BE_OFFSET          0x70    /* BounceEnable */
#define GPIO_BC_OFFSET          0x74    /* BounceControl */

#define GPIO_USED_GCOV          0x02    /* Which GPIO to do GCOV */
#define GPIO_USED_MASK          0xFFFF    /* Which GPIOs to use */


#define  GPIO_0                 0
#define  GPIO_1                 1
#define  GPIO_2                 2
#define  GPIO_3                 3
#define  GPIO_4                 4
#define  GPIO_5                 5
#define  GPIO_6                 6
#define  GPIO_7                 7
#define  GPIO_8                 8
#define  GPIO_9                 9
#define  GPIO_10                10
#define  GPIO_11                11
#define  GPIO_12                12
#define  GPIO_13                13
#define  GPIO_14                14
#define  GPIO_15                15
#define  GPIO_16                16
#define  GPIO_17                17
#define  GPIO_18                18
#define  GPIO_19                19
#define  GPIO_20                20
#define  GPIO_21                21
#define  GPIO_22                22
#define  GPIO_23                23
#define  GPIO_24                24
#define  GPIO_25                25

//------------------------IO_MUX_0-----------------------------------
//GPIO0
#define IO_MUX0_GPIO0_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO0_BITS			7 
#define IO_MUX0_GPIO0_OFFSET		0 //bit2:bit0
#define FUNC_GPIO0_TCK				  0
#define FUNC_GPIO0_GPIO0			  1
#define FUNC_GPIO0_UART2_TXD		2
#define FUNC_GPIO0_SPI1_CLK			3
#define FUNC_GPIO0_PWM_CTRL0 		4
#define FUNC_GPIO0_I2S_TXSCK		5


//GPIO1
#define IO_MUX0_GPIO1_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO1_BITS			7 
#define IO_MUX0_GPIO1_OFFSET		4 //bi6:bit4
#define FUNC_GPIO1_TMS				  0
#define FUNC_GPIO1_GPIO1			  1
#define FUNC_GPIO1_UART1_RXD		2
#define FUNC_GPIO1_SPI1_CS0			3
#define FUNC_GPIO1_PWM_CTRL1		4
#define FUNC_GPIO1_I2S_RXD  		5

//GPIO2
#define IO_MUX0_GPIO2_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO2_BITS			7 
#define IO_MUX0_GPIO2_OFFSET		8 //bit10:bit8
#define FUNC_GPIO2_TDO				  0
#define FUNC_GPIO2_GPIO2			  1
#define FUNC_GPIO2_UART1_TXD		2
#define FUNC_GPIO2_SPI1_MOSI		3
#define FUNC_GPIO2_PWM_CTRL2		4
#define FUNC_GPIO2_I2C_SCL			5

//GPIO3
#define IO_MUX0_GPIO3_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO3_BITS			7 
#define IO_MUX0_GPIO3_OFFSET		12 //bit14:bit12
#define FUNC_GPIO3_TDI				  0
#define FUNC_GPIO3_GPIO3			  1
#define FUNC_GPIO3_UART0_CTS		2
#define FUNC_GPIO3_SPI1_MISO		3
#define FUNC_GPIO3_PWM_CTRL3		4
#define FUNC_GPIO3_I2C_SDA			5


//GPIO4
#define IO_MUX0_GPIO4_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO4_BITS			7 
#define IO_MUX0_GPIO4_OFFSET		16//bit18:bit16
#define FUNC_GPIO4_TRST				  0
#define FUNC_GPIO4_GPIO4			  1
#define FUNC_GPIO4_UART0_RTS  	2
#define FUNC_GPIO4_SPI1_CS1			3
#define FUNC_GPIO4_PWM_CTRL4		4
#define FUNC_GPIO4_I2S_MCLK			5

//GPIO5
#define IO_MUX0_GPIO5_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO5_BITS			7 
#define IO_MUX0_GPIO5_OFFSET		20//bit22:bit20
#define FUNC_GPIO5_UART0_RXD		0
#define FUNC_GPIO5_GPIO5			  1
#define FUNC_GPIO5_40M_CLK_OUT	2
#define FUNC_GPIO5_IR_OUT   		3
#define FUNC_GPIO5_I2S_RXWS 		4


//GPIO6
#define IO_MUX0_GPIO6_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO6_BITS			7 
#define IO_MUX0_GPIO6_OFFSET		24//bit26:bit24
#define FUNC_GPIO6_UART0_TXD		0
#define FUNC_GPIO6_GPIO6			  1
#define FUNC_GPIO6_COLD_RESET		2
#define FUNC_GPIO6_32K_CLK_OUT	3
#define FUNC_GPIO6_I2S_RXSCK		4


//GPIO7
#define IO_MUX0_GPIO7_REG			SOC_PD_PIN_MUX0
#define IO_MUX0_GPIO7_BITS			7 
#define IO_MUX0_GPIO7_OFFSET		28//bit30:bit28
#define FUNC_GPIO7_SD_DATA0			0
#define FUNC_GPIO7_GPIO7			  1
#define FUNC_GPIO7_MSPI_MOSI		2
//------------------------IO_MUX_1-----------------------------------

//GPIO8
#define IO_MUX1_GPIO8_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO8_BITS			7 
#define IO_MUX1_GPIO8_OFFSET		0//bit2:bit0
#define FUNC_GPIO8_SD_DATA3			0
#define FUNC_GPIO8_GPIO8				1
#define FUNC_GPIO8_MSPI_HOLD		2


//GPIO9
#define IO_MUX1_GPIO9_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO9_BITS			7 
#define IO_MUX1_GPIO9_OFFSET		4//bit6:bit4
#define FUNC_GPIO9_SD_CLK 			0
#define FUNC_GPIO9_GPIO9			  1
#define FUNC_GPIO9_MSPI_CLK 		2

//GPIO10
#define IO_MUX1_GPIO10_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO10_BITS			7 
#define IO_MUX1_GPIO10_OFFSET		8//bit10:bit8
#define FUNC_GPIO10_SD_CMD  		0
#define FUNC_GPIO10_GPIO10			1
#define FUNC_GPIO10_MSPI_CS0		2


//GPIO11
#define IO_MUX1_GPIO11_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO11_BITS			7 
#define IO_MUX1_GPIO11_OFFSET		12//bit14:bit12
#define FUNC_GPIO11_SD_DATA1		0
#define FUNC_GPIO11_GPIO11			1
#define FUNC_GPIO11_MSPI_MISO		2


//GPIO12
#define IO_MUX1_GPIO12_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO12_BITS			7 
#define IO_MUX1_GPIO12_OFFSET		16//bit18:bit16
#define FUNC_GPIO12_SD_DATA2		0
#define FUNC_GPIO12_GPIO12			1
#define FUNC_GPIO12_MSPI_WP 		2


//GPIO13
#define IO_MUX1_GPIO13_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO13_BITS			7 
#define IO_MUX1_GPIO13_OFFSET		20//bit22:bit20
#define FUNC_GPIO13_SD_H_CLK		0
#define FUNC_GPIO13_GPIO13			1
#define FUNC_GPIO13_MSPI_CS1		2
#define FUNC_GPIO13_I2C_SCL 		3
#define FUNC_GPIO13_I2S_RXD		  4
#define FUNC_GPIO13_IR_OUT  		5


//GPIO14
#define IO_MUX1_GPIO14_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO14_BITS			7 
#define IO_MUX1_GPIO14_OFFSET		24//bit26:bit24
#define FUNC_GPIO14_BOOTMODE0		0
#define FUNC_GPIO14_GPIO14			1
#define FUNC_GPIO14_TOUT2			  2
#define FUNC_GPIO14_XTAL_I_32k	3
#define FUNC_GPIO14_IR_OUT  		4
#define FUNC_GPIO14_I2S_TXD			5

//GPIO15
#define IO_MUX1_GPIO15_REG			SOC_PD_PIN_MUX1
#define IO_MUX1_GPIO15_BITS			7 
#define IO_MUX1_GPIO15_OFFSET		28//bit30:bit28
#define FUNC_GPIO15_BOOTMODE1		0
#define FUNC_GPIO15_GPIO15			1
#define FUNC_GPIO15_TOUT3			  2
#define FUNC_GPIO15_XTAL_O_32k	3
#define FUNC_GPIO15_PWM_CTRL5		4
#define FUNC_GPIO15_I2S_TXWS		5

//GPIO16
#define IO_MUX2_GPIO16_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO16_BITS			7 
#define IO_MUX2_GPIO16_OFFSET		0//bit2:bit0
#define FUNC_GPIO16_TESTMODE		0
#define FUNC_GPIO16_GPIO16			1
#define FUNC_GPIO16_UART1_CTS		2
#define FUNC_GPIO16_MSPI_WP   	3


//GPIO17
#define IO_MUX2_GPIO17_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO17_BITS			7 
#define IO_MUX2_GPIO17_OFFSET		4//bit6:bit4
#define FUNC_GPIO17_WAKEUP  		0
#define FUNC_GPIO17_GPIO17			1
#define FUNC_GPIO17_UART2_RXD		2
#define FUNC_GPIO17_SPI1_WP   	3
#define FUNC_GPIO17_PWM_CTRL5		4
#define FUNC_GPIO17_I2S_TXWS		5


//GPIO18
#define IO_MUX2_GPIO18_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO18_BITS			7 
#define IO_MUX2_GPIO18_OFFSET		8//bit10:bit8
#define FUNC_GPIO18_RESET_B			0
#define FUNC_GPIO18_GPIO18			1
#define FUNC_GPIO18_UART1_RTS		2
#define FUNC_GPIO18_SPI1_HOLD		3
#define FUNC_GPIO18_I2C_SCL 		4
#define FUNC_GPIO18_I2S_TXD 		5

//GPIO19
#define IO_MUX2_GPIO19_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO19_BITS			7 
#define IO_MUX2_GPIO19_OFFSET		12//bit14:bit12
#define FUNC_GPIO19_I2C_SCL			0
#define FUNC_GPIO19_GPIO19			1
#define FUNC_GPIO19_TOUT0			  2

//GPIO20
#define IO_MUX2_GPIO20_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO20_BITS			7 
#define IO_MUX2_GPIO20_OFFSET		16//bit18:bit16
#define FUNC_GPIO20_I2C_SDA			0
#define FUNC_GPIO20_GPIO20			1
#define FUNC_GPIO20_TOUT1   		2

//GPIO21
#define IO_MUX2_GPIO21_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO21_BITS			7 
#define IO_MUX2_GPIO21_OFFSET		20//bit22:bit20
#define FUNC_GPIO21SD_H_CMD     0
#define FUNC_GPIO21_GPIO21			1
#define FUNC_GPIO21_UART0_RXD		2
#define FUNC_GPIO21_I2C_SDA 		3
#define FUNC_GPIO21_I2S_TXD 		4
#define FUNC_GPIO21_BT_ACTIVE		5


//GPIO22
#define IO_MUX2_GPIO22_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO22_BITS			7 
#define IO_MUX2_GPIO22_OFFSET		24//bit26:bit24
#define FUNC_GPIO22_SD_H_DATA0  0
#define FUNC_GPIO22_GPIO22			1
#define FUNC_GPIO22_UART0_TXD		2
#define FUNC_GPIO22_PWM_CTRL0		3
#define FUNC_GPIO22_I2S_TXWS		4
#define FUNC_GPIO22_BT_PRIORITY		5


//GPIO23
#define IO_MUX2_GPIO23_REG			SOC_PD_PIN_MUX2
#define IO_MUX2_GPIO23_BITS			7 
#define IO_MUX2_GPIO23_OFFSET		28//bit30:bit28
#define FUNC_GPIO23_SD_H_DATA1  0
#define FUNC_GPIO23_GPIO23			1
#define FUNC_GPIO23_UART1_RTS		2
#define FUNC_GPIO23_PWM_CTRL1		3
#define FUNC_GPIO23_I2S_TXSCK		4
#define FUNC_GPIO23_W_ACTIVE		5

//GPIO24
#define IO_MUX3_GPIO24_REG			SOC_PD_PIN_MUX3
#define IO_MUX3_GPIO24_BITS			7 
#define IO_MUX3_GPIO24_OFFSET		0//bit2:bit0
#define FUNC_GPIO24_SD_H_DATA2	0
#define FUNC_GPIO24_GPIO24			1
#define FUNC_GPIO24_UART1_CTS		2
#define FUNC_GPIO24_PWM_CTRL2		3
#define FUNC_GPIO24_I2S_MCLK		4
#define FUNC_GPIO24_W_PRIORTY		5


//GPIO25
#define IO_MUX3_GPIO25_REG			SOC_PD_PIN_MUX3
#define IO_MUX3_GPIO25_BITS			7 
#define IO_MUX3_GPIO25_OFFSET		4//bit6:bit4
#define FUNC_GPIO25_SD_H_DATA3  0
#define FUNC_GPIO25_GPIO25			1
//#define FUNC_GPIO25_PHY_ENTRX		2
#define FUNC_GPIO25_PWM_CTRL3		3
#define FUNC_GPIO25_PHY_ENTRX		4
#define FUNC_GPIO25_I2C_SDA     5

#define PIN_MUX_SET(reg,bits,offset,fun) outw(reg, (inw(reg) & (~(bits<<offset))) |(fun<<offset))
#define PIN_FUNC_SET(PIN_NAME,PIN_FUNC) \
		PIN_MUX_SET(PIN_NAME##_REG, PIN_NAME##_BITS, PIN_NAME##_OFFSET, PIN_FUNC)

#define PIN_PAD_MODE(x, gpiomode)   { \
                                      if(gpiomode) \
                                        outw(SOC_AON_PAD_MOD, inw(SOC_AON_PAD_MOD) | (x)); \
                                      else{ \
                                        outw(SOC_AON_PAD_MOD, inw(SOC_AON_PAD_MOD) & ~(x)); \
                                      } \
                                    }

#define   PIN_SIGNALS_FROM_GPIO_MODULE     0x1
#define   PIN_SIGNALS_FROM_PDCORE_PINMUX   0x0

#endif/*_SOC_PIN_MUX_H*/



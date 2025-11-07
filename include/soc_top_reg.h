/*******************************************************************************
 * Copyright by Transa Semi.
 *
 * File Name:soc_top_reg.h    
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       v0.1
 * Author:        wangchao
 * Date:          2018-8-19
 * History 1:      
 *     Date: 2018-12-19
 *     Version:
 *     Author: wangxia
 *     Modification: add mod top reg 
 * History 2: 
 *     Date: 2021-1-6
 *     Version:
 *     Author: liuyong
 *     Modification: modification and format arrangement
  ********************************************************************************/
#ifndef _SOC_TOP_REG_H
#define _SOC_TOP_REG_H

/****************************************************************************
* 	                                           Include files
****************************************************************************/


/****************************************************************************
* 	                                           Local Macros
****************************************************************************/

/****************************************************************************
* 	                                           Local Types
****************************************************************************/

/****************************************************************************
* 	                                           Local Constants
****************************************************************************/

/****************************************************************************
* 	                                           Local Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Global Constants
****************************************************************************/

/****************************************************************************
* 	                                          Global Variables
****************************************************************************/

/****************************************************************************
* 	                                          Global Function Prototypes
****************************************************************************/

/****************************************************************************
* 	                                          Function Definitions
****************************************************************************/
#define _IO_(addr)              		(addr)


/* Device base address */
#define SOC_PCU_BASE				    _IO_(0x00201000)
#define SOC_RTC_BASE				    _IO_(0x00201100)
#define SOC_AON_SMU_BASE				_IO_(0x00201200)
#define SOC_GPIO_BASE               	_IO_(0x00201300)
#define SOC_PD_SMU_BASE					_IO_(0x00202000)
#define SOC_RFC_BASE					_IO_(0x00203000)
#define SOC_UART0_BASE					_IO_(0x00204000)
#define SOC_UART1_BASE					_IO_(0x00205000)
#define SOC_UART2_BASE					_IO_(0x00206000)
#define SOC_PIT0_BASE                	_IO_(0x00207000)
#define SOC_PIT1_BASE				    _IO_(0x00208000)
#define SOC_WDT_BASE				    _IO_(0x00209000)
#define SOC_I2C_BASE				    _IO_(0x0020A000)
#define SOC_SPI0_BASE					_IO_(0x0020B000) //spiflash
#define SOC_SPI1_BASE					_IO_(0x0020C000)
#define SOC_TRNG_BASE				    _IO_(0x0020D000)
#define SOC_DUMP_BASE					_IO_(0x0020E000)
#define SOC_EFUSE_BASE				    _IO_(0x0020F000)
#define SOC_IR_BASE	                    _IO_(0x00210000)
#define SOC_I2S_BASE					_IO_(0x00280000)
#define SOC_DMAC_BASE					_IO_(0x00400000)
#define SOC_ECC_BASE					_IO_(0x00410000)
#define SOC_AES_BASE					_IO_(0x00420000)
#define SOC_HASH_BASE					_IO_(0x00430000)
#define SOC_SDIO_SLAVE_BASE				_IO_(0x00440000)
#define SOC_SDIO_HOST_BASE				_IO_(0x00450000)
#define SOC_BLE_BASE					_IO_(0x00460000)
#define SOC_DFE_BASE					_IO_(0x00470000)
#define SOC_XIP_BASE					_IO_(0x00800000)
#define SOC_PSRAM_BASE					_IO_(0x00C00000)


/* External memory mapping */
#define ROM_BASE						_IO_(0x00000000)
#define EILM_0_BASE						_IO_(0x00010000)
#define EILM_1_BASE						_IO_(0x00020000)
#define EDLM_0_BASE						_IO_(0x00040000)
#define EDLM_1_BASE						_IO_(0x00060000)
#define IRAM0_BASE						_IO_(0x00080000)
#define IRAM1_BASE						_IO_(0x000A0000)

#define ROM_SIZE						_IO_(0x10000)
#define ILM0_SIZE						_IO_(0x10000)
#define ILM1_SIZE						_IO_(0x20000)
#define DLM0_SIZE						_IO_(0x20000)
#define DLM1_SIZE						_IO_(0x20000)
#define IRAM0_SIZE						_IO_(0x20000)
#define IRAM1_SIZE						_IO_(0x10000)


#define BOOTMODE_MASK					0x00000003
#define BOOTMODE_LATCH_MASK				0x0000000C
#define BOOTMODE_LATCH_OFFSET			0x2


/* SOC_PCU */
#define  SOC_PCU_PMU_BUCK_CTRL_REG3		(SOC_PCU_BASE + 0xC)
#define	SOC_PCU_CTRL_REG9				(SOC_PCU_BASE + 0x20)
#define  PS_HOLD						(SOC_PCU_BASE + 0x28)
#define	SOC_PCU_CTRL_REG21				(SOC_PCU_BASE + 0x54)


// AON_SMU
#define SOC_AON_CHIP_ID					_IO_(SOC_AON_SMU_BASE)    
#define SOC_AON_WAKEUP_ADD				(SOC_AON_SMU_BASE + 0x4)  /*default:0  0:wakeup from 0x0 address 1:wakeup from 64KB address*/
#define SOC_AON_32K_SEL					(SOC_AON_SMU_BASE + 0x8)  /*0:analog 1:pad*/
#define SOC_AON_WAKEUP_EN				(SOC_AON_SMU_BASE + 0xC)  /**0--sel from gpio13  1--sel from wakeup pin**/
#define SOC_AON_ILM_CACHE_SWITCH		(SOC_AON_SMU_BASE+0x10) /**1:ilm   0:cache**/
#define SOC_AON_TEST_MOD				(SOC_AON_SMU_BASE+0x14)
#define SOC_AON_PAD_PE					(SOC_AON_SMU_BASE+0x18) /**map to pullup**/
#define SOC_AON_PAD_PS					(SOC_AON_SMU_BASE+0x1C) /**map to pulldown**/
#define SOC_AON_PAD_IE					(SOC_AON_SMU_BASE+0x20) /**input enable**/
#define SOC_AON_PAD_MOD					(SOC_AON_SMU_BASE+0x24) /**0--from pd_core   1--from aon_smu**/
#define SOC_AON_PAD_SR					(SOC_AON_SMU_BASE+0x28)
#define SOC_AON_DUMMY_REG				(SOC_AON_SMU_BASE+0x34)
#define SOC_AON_FLASH_ENCRYPT			(SOC_AON_SMU_BASE+0x38)
#define SOC_AON_BOOTMODE_BASE			(SOC_AON_SMU_BASE+0x3C)


// PD_SMU
#define SOC_PD__CLK_MUX_BASE			_IO_(SOC_PD_SMU_BASE + 0x4)

#define SOC_PD_CLK_EN_BASE0				_IO_(SOC_PD_SMU_BASE + 0xC)
#define SOC_PD_CLK_EN_BASE1				_IO_(SOC_PD_SMU_BASE + 0x10)
#define SOC_PD_SW_RESET_BASE			_IO_(SOC_PD_SMU_BASE + 0x14)
#define SOC_PD_SPI_CONFIG				_IO_(SOC_PD_SMU_BASE + 0x18)
#define SOC_PD_SDIO_CONFIG1				_IO_(SOC_PD_SMU_BASE + 0x1c)
#define SOC_PD_SDIO_CONFIG2				_IO_(SOC_PD_SMU_BASE + 0x20)
#define SOC_PD_SDIO_CONFIG3				_IO_(SOC_PD_SMU_BASE + 0x24)
#define SOC_PD_SDIO_HOST				_IO_(SOC_PD_SMU_BASE + 0x28)
#define SOC_PD_SDIO_SLAVE				_IO_(SOC_PD_SMU_BASE + 0x2C)
#define SOC_PD_PIN_MUX0					_IO_(SOC_PD_SMU_BASE + 0x30)
#define SOC_PD_PIN_MUX1					_IO_(SOC_PD_SMU_BASE + 0x34)
#define SOC_PD_PIN_MUX2					_IO_(SOC_PD_SMU_BASE + 0x38)
#define SOC_PD_PIN_MUX3					_IO_(SOC_PD_SMU_BASE + 0x70)
#define SOC_PD_AUX_ADC_CONFIG			_IO_(SOC_PD_SMU_BASE + 0x3C)
#define SOC_PD_AUX_ADC_OUT				_IO_(SOC_PD_SMU_BASE + 0x40)
#define SOC_PD_RTC_CAL_CONFIG			_IO_(SOC_PD_SMU_BASE + 0x44)
#define SOC_PD_RTC_CNT					_IO_(SOC_PD_SMU_BASE + 0x48)
#define SOC_PD_I2S_MOD					_IO_(SOC_PD_SMU_BASE + 0x4C)
#define SOC_PD_UART_CLK_SEL				_IO_(SOC_PD_SMU_BASE + 0x50)/**uart_clk_src_sel 0:clk_xo_40_26m 1:uart_clk_src_160m_div2**/
#define SOC_PD_WIFI_BLE_SEL				_IO_(SOC_PD_SMU_BASE + 0x54)
#define SOC_PD_ENDIAN					_IO_(SOC_PD_SMU_BASE + 0x58) /**bitO:ecc bit1:trng  bit2:hash**/
#define SOC_PD_HASH_DONE				_IO_(SOC_PD_SMU_BASE + 0x5C) /**BIT0:done计算结束标志，高脉冲**/
#define SOC_PD_I2S_DIV_INT				_IO_(SOC_PD_SMU_BASE + 0x60) /**BIT0-7: i2s div integer number，整数分频因子**/
#define SOC_PD_I2S_DIV_FRAC				_IO_(SOC_PD_SMU_BASE + 0x64) /**BIT0-15:i2s div fra div，小数分频因子的分子, BIT16-31:i2s div fra base，小数分频因子分母**/
#define SOC_PD_TRX_BYPASS				_IO_(SOC_PD_SMU_BASE + 0x68) /**BIT0:RX ADC BYPASS, BIT1:TX DAC BYPASS**/
#define SOC_PD_IVIC_TRIQ_TYPE			_IO_(SOC_PD_SMU_BASE + 0x6C)
#define SOC_PD_CLK_DIV_EN				_IO_(SOC_PD_SMU_BASE + 0x74)
#define SOC_PD_APB_ENCRYPT_EN			_IO_(SOC_PD_SMU_BASE + 0x78)
#define SOC_PD_ECC_AND_HASH_MEM			_IO_(SOC_PD_SMU_BASE + 0x7C)


// RF
#define SOC_RFC_DIG_PLL1				_IO_(SOC_RFC_BASE + 0x84)
#define SOC_RFC_DIG_PLL2				_IO_(SOC_RFC_BASE + 0x88)
#define SOC_RFC_DIG_PLL3				_IO_(SOC_RFC_BASE + 0x8C)


//dfe_pll
#define SOC_DIG_PLL_REG12				_IO_(SOC_RFC_BASE + 0x130)
#define SOC_DIG_PLL_REG13				_IO_(SOC_RFC_BASE + 0x134)
#define SOC_DIG_PLL_REG14				_IO_(SOC_RFC_BASE + 0x138)
#define SOC_DIG_PLL_REG18				_IO_(SOC_RFC_BASE + 0x148)
#define SOC_DIG_PLL_REG20				_IO_(SOC_RFC_BASE + 0x150)


// SOC_PD_ECC_AND_HASH_MEM
#define ECC_MEMORY						0x1
#define HASH_MEMORY						0x0


// aes regs
#define SKE_LP_CTRL_OFFSET              0x0
#define SKE_LP_CFG_OFFSET           	0x4
#define SKE_LP_SR_1_OFFSET          	0x8
#define SKE_LP_SR_2_OFFSET          	0xc
#define SKE_LP_KEY_OFFSET            	0x80   //0x80--0x8c
#define SKE_LP_MDIN_OFFSET          	0x200  //0x200--0x20c
#define SKE_LP_MDOUT_OFFSET          	0x210  //0x210--0x21c
//ske_lp_ctrl
#define SKE_LP_START             		0x1
//ske_lp_cfg
#define SKE_LP_DEC                  	(0x1 << 8)
#define SKE_LP_IRQEN                	(0x1 << 24)
//ske_lp_sr_1
#define SKE_LP_BUSY                		0x1
//ske_lp_sr_2
#define SKE_LP_STATUS_DONE           	0x1

//SOC_AON_DUMMY_REG
#define COLD_RESET						0x00
#define DEEPSLEEP_RESET                 0x01
#define WDT_RESET                       0x02
#define SOFT_RESET                      0x03

#define WAKEUP_ADDR_VALUE_0x0				0	//< After waking up, start from address 0 
#define WAKEUP_ADDR_VALUE_0x10000			1	//< After waking up, start from address 0x10000 

#endif


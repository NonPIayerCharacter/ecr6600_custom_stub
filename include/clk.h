#ifndef __CLK_H__
#define __CLK_H__

#include "soc_top_reg.h"
#include "common_def.h"

/* System Clock */
#define MHz                     1000000
#define CPUFREQ                 (160 * MHz)
#define HCLKFREQ                (CPUFREQ/2)
#define PCLKFREQ                (UCLKFREQ/2)

//CLK_ENABLE_VTRL_REG 0x20200C
#define SMU_APB_CLK_BIT		        		(1<<0)
#define UART0_UCLK_BIT						(1<<1)
#define UART0_APB_CLK_BIT					(1<<2)
#define UART1_UCLK_BIT		       			(1<<3)
#define UART1_APB_CLK_BIT					(1<<4)
#define UART2_UCLK_BIT						(1<<5)
#define UART2_APB_CLK_BIT					(1<<6)
#define SPI0_APB_CLK_BIT					(1<<7)
#define SPI1_APB_CLK_BIT					(1<<8)
#define I2C_APB_CLK_BIT						(1<<9)
#define FREF_CLK_EN_BIT						(1<<10)
#define IR_APB_CLK_BIT						(1<<11)
#define AES_APB_CLK_BIT		        		(1<<12)
#define WDT_APB_CLK_BIT						(1<<13)
#define WDT_EXT_CLK_32K_BIT					(1<<14)
#define TRNG_CLK_BIT    	        		(1<<15)
#define TRNG_APB_CLK_BIT					(1<<16)
#define EFUSE_CLK_BIT						(1<<17)
#define EFUSE_APB_CLK_BIT					(1<<18)
#define RF_REGF_APB_CLK_BIT					(1<<19)
#define PIT0_APB_CLK_BIT            		(1<<20)
#define PIT1_APB_CLK_BIT            		(1<<21)
#define IQ_DUMP_APB_CLK_BIT         		(1<<23)
#define SDIO_S_AHB_CLK_BIT          		(1<<24)
#define SDIO_H_AHB_CLK_BIT          		(1<<25)
#define IRAM0_AHB_CLK_BIT           		(1<<26)
#define IRAM1_AHB_CLK_BIT           		(1<<27)
#define FLASH_AHB_CLK_BIT           		(1<<28)
#define I2S_AHB_CLK_BIT             		(1<<29)
#define PHY_AHB_CLK_BIT             		(1<<30)
#define MAC_AHB_CLK_BIT             		(1<<31)

//CLK_ENABLE_VTRL_REG 0x205010
#define BLE_AHB_HCLK_BIT		    		(1<<0)
#define ECC_AHB_CLK_BIT    					(1<<1)
#define HASH_AHB_CLK_BIT					(1<<2)
#define AES_AHB_CLK_BIT		        		(1<<3)
#define DMA_AHB_CLK_BIT						(1<<4)
#define I2S_CLK_BIT 						(1<<5)
#define WLAN_CLK_EN_BIT						(1<<8)
#define RX_CLK_EN_BIT						(1<<9)
#define BLE_CLK_BIT 						(1<<10)
#define SMIH_LP_CLK_BIT						(1<<11)
#define DOT_CLK_BIT 		        		(1<<12)
#define DOT_RX_CLK_BIT						(1<<13)
#define RF_REG_40M_CLK_EN_BIT				(1<<14)
#define AHB2_APB_CLK_EN_BIT    	    		(1<<15)
#define DFE_AHB_CLK_BIT						(1<<16)
#define ADC_AUX_40M_CLK_BIT					(1<<17)
#define TRNG_RBG_AHB_CLK_BIT				(1<<18)


#define UART_CLK_SEL_40M                    (0x0)
#define UART_CLK_SEL_160M_DIV2              (0x1)

#define CLK_MUX_EXT_CLK_40M                 (0x1 << 4)
#define CLK_MUX_CRYSTAL_40M                 (0x0 << 4)
#define CPU_CLK_FREQ_DIV2                   (0x0 << 2)
#define CPU_CLK_FREQ_DIV0                   (0x1 << 2)
#define CPU_CLK_FREQ_DIV4                   (0x2 << 2)
#define CPU_CLK_FREQ_DIV8                   (0x3 << 2)
#define CPU_CLK_SEC_40M_26M                 (0x0 << 0)
#define CPU_CLK_SEC_240M                    (0x1 << 0)
#define CPU_CLK_SEC_160M                    (0x2 << 0)
#define CPU_CLK_SEC_32K                     (0x3 << 0)

// add by liuyong, from freertos  20210317
#define CHIP_SMU_PD_CLK_UART_TRNG_80M_EN		(1<<2)
#define CHIP_SMU_PD_CLK_DIV3_EN					(1<<1)
#define CHIP_SMU_PD_CLK_DIV2_EN					(1<<0)


//#define CLK_ENABLE(x)	      ((*((volatile unsigned int *)CLK_EN_BASE0)) = (unsigned int)((*((volatile unsigned int *)CLK_EN_BASE0))|(x)))
//#define CLK_DISABLE(x)	      ((*((volatile unsigned int *)CLK_EN_BASE0)) = (unsigned int)((*((volatile unsigned int *)CLK_EN_BASE0))&(~(x))))
#define CLK_ENABLE(base, x)		outw(base, inw(base) | (x))                         
#define CLK_DISABLE(base, x)	outw(base, inw(base) & ~(x))

#define CLK_BASE(N)             (SOC_PD_CLK_EN_BASE0 + ((N) / 0x20) * 0x4)

//typedef enum 
//{
//	SOC_MOD_CLK_DISABLE,
//	SOC_MOD_CLK_ENABLE,
//	SOC_MOD_CLK_ALL_DISABLE,
//	SOC_MOD_CLK_ALL_ENABLE,
//
//	SOC_MOD_CLK_MAX
//}SOC_MOD_CLK_CTRL;


//typedef enum{
//    SMU_APB_CLK,
//    UART0_UCLK,
//    UART0_APB_CLK,
//    UART1_UCLK,
//    UART1_APB_CLK,
//    UART2_UCLK,
//    UART2_APB_CLK,
//    SPI0_APB_CLK,
//    SPI1_APB_CLK,
//    I2C_APB_CLK,
//    FREF_CLK,
//    IR_APB_CLK,
//    AES_APB_CLK,
//    WDT_APB_CLK,
//    WDT_EXT_CLK_32K,
//    TRNG_CLK,
//    TRNG_APB_CLK,
//    EFUSE_CLK,
//    EFUSE_APB_CLK,
//    RF_REGF_APB_CLK,
//    PIT0_APB_CLK,
//    PIT1_APB_CLK,
//    RESERVED_GPIO_APB_CLK,
//    IQ_DUMP_APB_CLK,
//    SDIO_S_AHB_CLK,
//    SDIO_H_AHB_CLK,
//    IRAM0_AHB_CLK,
//    IRAM1_AHB_CLK,
//    FLASH_AHB_CLK,
//    I2S_AHB_CLK,
//    PHY_AHB_CLK,
//    MAC_AHB_CLK,
//
////CLK_ENABLE_VTRL_REG 0x205010
//    BLE_AHB_HCLK,
//    ECC_AHB_CLK,
//    HASH_AHB_CLK,
//    AES_AHB_CLK,
//    DMA_AHB_CLK,
//    I2S_CLK,
//    RESERVED_RF_PLL_CLK,
//    RESERVED_DIG_PLL_CLK,
//    WLAN_CLK,
//    RX_CLK,
//    BLE_CLK,
//    SMIH_LP_CLK,
//    DOT_CLK,
//    DOT_RX_CLK,
//    RF_REG_40M_CLK,
//    AHB2_APB_CLK,
//    DFE_AHB_CLK,
//    ADC_AUX_40M_CLK,
//    TRNG_RBG_AHB_CLK,
//
//    MAX_CLK
//}T_SOC_CLK;


 void soc_update_clk_160m(void);

#endif

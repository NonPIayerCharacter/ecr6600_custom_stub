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


/*--------------------------------------------------------------------------
*												Include files
--------------------------------------------------------------------------*/
#include "common_def.h"
#include "uboot_init.h"
#include "pit.h"
#include "clk.h"
#include "uart.h"
#include "efuse.h"
#include "flash_encrypt.h"
#include "dbglog.h"

/*--------------------------------------------------------------------------
* 	                                           	Local Macros
--------------------------------------------------------------------------*/
/** Description of the macro */
#define READ_REG(reg)				(  *( (volatile unsigned int *) (reg) ) )
#define WRITE_REG(reg, data)		( (*( (volatile unsigned int *) (reg) ) ) = (unsigned int)(data) )

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
unsigned int g_efuse_ctrl = 0;

/*--------------------------------------------------------------------------
* 	                                          	Global Function Prototypes
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* 	                                          	Function Definitions
--------------------------------------------------------------------------*/

/**
 * @brief uboot阶段初始化c区域，复制data、初始化bss
 */
void boot_c_init(void)
{
	/* Use compiler builtin memcpy and memset */
	extern char __bss_start;
	extern char __bss_end;
	int size;

	/* If we load code to RAM we don't need to copy
	 * data section and lma will be equal to vma.
	 * */

	/* data section will be copied before we remap.
	 * We don't need to copy data section here. */
	extern char __data_lma;
	extern char __data_vma_start;
	extern char __data_vma_end;
	/* Copy data section to RAM */
	size = &__data_vma_end - &__data_vma_start;
	MEMCPY(&__data_vma_start, &__data_lma, size);

	/* Clear bss section */
	size = &__bss_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
}

/**
 * @brief boot_usdelay(unsigned us)
 */
void boot_usdelay(unsigned int us)
{
	unsigned long s = 0, to_now = 0, us_delay = 0;
	s = (unsigned long)pit_ch_count_get(SOC_PIT1_BASE, 3);
	to_now = 0;
	
	us_delay = (unsigned long)(us*PIT_DIVISION); // 160 OK
	
	while(to_now < us_delay)
	{
		to_now = s - (unsigned long)pit_ch_count_get(SOC_PIT1_BASE, 3);
	}
}


#if defined(CPU_CLK_SET_160M) || defined(CPU_CLK_SET_240M)
/**
 * @brief bbpll
 */
#define MEM_BASE_RFC_REG				0x00203000
#define CHIP_RF_DIG_PLL1				(MEM_BASE_RFC_REG + 0x84)
#define CHIP_RF_DIG_PLL2				(MEM_BASE_RFC_REG + 0x88)
#define CHIP_RF_DIG_PLL3				(MEM_BASE_RFC_REG + 0x8C)
#define CHIP_RF_PLL_REG12				(MEM_BASE_RFC_REG + 0x130)
#define CHIP_RF_PLL_REG13				(MEM_BASE_RFC_REG + 0x134)
#define CHIP_RF_PLL_REG14				(MEM_BASE_RFC_REG + 0x138)
#define CHIP_RF_PLL_REG18				(MEM_BASE_RFC_REG + 0x148)
#define CHIP_RF_PLL_REG20				(MEM_BASE_RFC_REG + 0x150)

//#define MEM_BASE_PCU					0x00201000
//#define RF_CTRL_DCXO_REG6				(MEM_BASE_PCU + 0x18)

//#define drv_pit_delay boot_usdelay
//void boot_chip_clk_bbpll_enable(void)
//{
//	volatile unsigned int value;
//	//WRITE_REG(RF_CTRL_DCXO_REG6,0x01f03201);	//note:eco3 isn't need 
//	while(1)
//	{
//		// 0x00203084	
//		WRITE_REG(CHIP_RF_DIG_PLL1, 0x1300);
//		// 0x00203130	
//		WRITE_REG(CHIP_RF_PLL_REG12, 0x6);
//		// 0x00203130	
//		WRITE_REG(CHIP_RF_PLL_REG12, 0x2);
//		// 0x0020308c
//		WRITE_REG(CHIP_RF_DIG_PLL3, 0x0e4345ac);
//		// 0x00203088
//		WRITE_REG(CHIP_RF_DIG_PLL2, 0x12697f74);
//		// 0x00203134
//		WRITE_REG(CHIP_RF_PLL_REG13, 0x49);
//		// 0x00203138
//		WRITE_REG(CHIP_RF_PLL_REG14, 0x006c4ec5);
//		// 0x00203150
//		WRITE_REG(CHIP_RF_PLL_REG20, 0x49e);
//		// 0x00203084
//		WRITE_REG(CHIP_RF_DIG_PLL1, 0x3300);
//		drv_pit_delay(5);
//		// 0x00203148
//		WRITE_REG(CHIP_RF_PLL_REG18, 0x1);
//		drv_pit_delay(20);
//		WRITE_REG(CHIP_RF_PLL_REG18, 0x0);
//		value = (READ_REG(0x203158) >> 16) & 0xffff;
//		if(value <= 0x4AE && value >= 0x48E)
//		{
//			break;
//		}
//	}
//	// 0x0020308c
//	WRITE_REG(CHIP_RF_DIG_PLL3, 0x2e4305a0);
//	// 0x00203088
//	WRITE_REG(CHIP_RF_DIG_PLL2, 0x1a697f74);
//	// 0x00203084	
//	WRITE_REG(CHIP_RF_DIG_PLL1, 0x2300);
//	// 0x00203130	
//	WRITE_REG(CHIP_RF_PLL_REG12, 0x3);
//	drv_pit_delay(10);
//	// 0x0020308c
//	WRITE_REG(CHIP_RF_DIG_PLL3, 0x2ec305a0);
//}

/**
 * @brief 提升cpu主频
 */
void boot_soc_update_cpu_clk(void)
{
#if 0
	OUT32(SOC_PD_CLK_EN_BASE0, IN32(SOC_PD_CLK_EN_BASE0) | 
		(FREF_CLK_EN_BIT) | 
		(RF_REGF_APB_CLK_BIT) | 
		(EFUSE_CLK_BIT) |
		(EFUSE_APB_CLK_BIT) |
		(IRAM0_AHB_CLK_BIT) |
		(IRAM1_AHB_CLK_BIT) );
	boot_chip_clk_bbpll_enable();
	efuse_cfg_160m();
	/* 1 必须在前 */
	OUT32(SOC_PD_CLK_DIV_EN, IN32(SOC_PD_CLK_DIV_EN) | CHIP_SMU_PD_CLK_DIV2_EN | CHIP_SMU_PD_CLK_DIV3_EN | CHIP_SMU_PD_CLK_UART_TRNG_80M_EN);
	/* 2 必须在1之后 */
#ifdef CPU_CLK_SET_160M
	OUT32(SOC_PD__CLK_MUX_BASE, (IN32(SOC_PD__CLK_MUX_BASE)&0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_160M);
#endif
#ifdef CPU_CLK_SET_240M
	OUT32(SOC_PD__CLK_MUX_BASE, (IN32(SOC_PD__CLK_MUX_BASE)&0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_240M);
#endif
	OUT32(SOC_PD_UART_CLK_SEL, UART_CLK_SEL_160M_DIV2);
	udelay(1);
	boot_uart_set_baudrate(SOC_UART0_BASE, UART_BAUD_115200, 0);
	
	g_efuse_ctrl = efuse_read(EFUSE_CTRL_OFFSET);
#else
	unsigned int val;
	g_efuse_ctrl = efuse_read(EFUSE_CTRL_OFFSET);
	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, RF_REGF_APB_CLK_BIT | FREF_CLK_EN_BIT );

	if((g_efuse_ctrl & EFUSE_CRYSTAL_OFFSET) == EFUSE_CRYSTAL_40M)
	{
		/**            40mhz             **/
		// cfg  dig_ari_wifi_bbpll_cp_ilk_sel, 0x0020308C:::bit7:4 = 6
		val = inw(SOC_RFC_DIG_PLL3);
		val = (val & 0xFFFFFF0F) | 0x60 ;
		outw(SOC_RFC_DIG_PLL3, val);
	}
	else
	{
		/**            26mhz             **/
		// cfg  dig_ari_wifi_bbpll_cp_ilk_sel, 0x0020308C:::bit7:4 = 6
		// cfg  dig_ari_wifi_bbpll_prse_div_shift, 0x0020308C:::bit15 = 0
		val = inw(SOC_RFC_DIG_PLL3);
		val = (val & 0xFFFF7F0F) | 0x60 ;
		outw(SOC_RFC_DIG_PLL3, val);
		
		// cfg  dig_ari_wifi_bbpll_lpf_rs_sel, 0x00203088:::bit20:14 = 0x25
		val = inw(SOC_RFC_DIG_PLL2);
		val = (val & (~(0x7F<<14))) | (0x25<<14);
		outw(SOC_RFC_DIG_PLL2, val);
		
		// cfg  rfc_reg_dpll_div_int, 0x00203134:::bit7:0 = 0x49
		outw(SOC_DIG_PLL_REG13, 0x49);
	
		// cfg  rfc_reg_dpll_div_frac, 0x00203138:::bit23:0 = 0x6C4EC5
		outw(SOC_DIG_PLL_REG14, 0x6C4EC5);

		// cfg  rfc_reg_dpll_freq_in, 0x00203150:::bit13:0 = 0x49E
		outw(SOC_DIG_PLL_REG20, 0x49E);	
	}

	//enable  dig_ari_wifi_bbpll_ldo_pu, 0x00203084:::bit13 = 1
	val = inw(SOC_RFC_DIG_PLL1);
	val |= 1<<13;
	outw(SOC_RFC_DIG_PLL1, val);

	// delay 10us for stability
	udelay(10);

	// enable rfc_reg_dpll_ fcal_en,  0x00203148:::bit0 = 1
	outw(SOC_DIG_PLL_REG18, 1);

	// delay 10us for stability
	udelay(10);

	//cfg  dig_ari_wifi_bbpll_prsc_pdiv2_pu  0x0020308C:::bit14 = 0
	//cfg  dig_ari_wifi_bbpll_vco_cal_pu  0x0020308C:::bit3 = 0
	//cfg  dig_ari_wifi_bbpll_vco_calrefl_en  0x0020308C:::bit2:0 = 0
	val = inw(SOC_RFC_DIG_PLL3);
	val &= ~((1<<14) |0xF);
	outw(SOC_RFC_DIG_PLL3, val);

	//cfg  rfc_reg_bbpll_fcal_en  0x00203148:::bit0 = 0
	outw(SOC_DIG_PLL_REG18, 0);

	// delay 5us for stability
	udelay(5);

	// cfg dig_ari_wifi_bbpll_pfd_pu  0x00203088:::bit27 = 1
	val = inw(SOC_RFC_DIG_PLL2);
	val |= 1<<27;
	outw(SOC_RFC_DIG_PLL2, val);

	// cfg rfc_reg_dpll_sdm_en  0x00203088:::bit0 = 1
	val = inw(SOC_DIG_PLL_REG12);
	val |= 1;
	outw(SOC_DIG_PLL_REG12, val);

	// delay 20us for stability
	udelay(20);

	// cfg dig_ari_wifi_bbpll_fout_postdiv_pu  0x0020308C:::bit23 = 1
	val = inw(SOC_RFC_DIG_PLL3);
	val |= 1<<23;
	outw(SOC_RFC_DIG_PLL3, val);

	/* 1 必须在前 */
	// 0x202074  bit1 = 1
	outw(0x00202074 , 0x6);

	udelay(1);
	
	efuse_cfg_160m();
	/* 1 必须在前 */
	//OUT32(SOC_PD_CLK_DIV_EN, IN32(SOC_PD_CLK_DIV_EN) | CHIP_SMU_PD_CLK_DIV2_EN | CHIP_SMU_PD_CLK_DIV3_EN | CHIP_SMU_PD_CLK_UART_TRNG_80M_EN);
	/* 2 必须在1之后 */
#ifdef CPU_CLK_SET_160M
	OUT32(SOC_PD__CLK_MUX_BASE, (IN32(SOC_PD__CLK_MUX_BASE)&0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_160M);
#endif
#ifdef CPU_CLK_SET_240M
	OUT32(SOC_PD__CLK_MUX_BASE, (IN32(SOC_PD__CLK_MUX_BASE)&0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_240M);
#endif
	OUT32(SOC_PD_UART_CLK_SEL, UART_CLK_SEL_160M_DIV2);
	udelay(1);
	boot_uart_set_baudrate(SOC_UART0_BASE, UART_BAUD_115200, 0);
#endif
}
#endif



/**
 * @brief 设置波特率
 */
void boot_uart_set_baudrate(unsigned int uart_base, unsigned int baud, unsigned int cond)
{
	unsigned int baudrate, uclk = 0;
	unsigned int osc = 8;
	unsigned int value = inw(UARTC_LCR_REG);


	uclk = inw(SOC_PD_UART_CLK_SEL) == 0x0 ? 40000000 : 80000000;
	if(uclk == 40000000)
	{
		int crystal = g_efuse_ctrl;//EFUSE_CRYSTAL_OFFSET
		if((crystal & EFUSE_CRYSTAL_OFFSET) == EFUSE_CRYSTAL_26M){
			uclk = 26000000;
		}
	}
	
	baudrate = uclk/osc/baud;
	outw(UARTC_OSCR_REG, osc);
	outw(UARTC_LCR_REG, 0x80|value);
	if(uclk==80000000)
	{
		if(baud==UART_BAUD_1500000)	//校准1500000
		{
			osc = 18;
			outw(UARTC_OSCR_REG, osc);
			baudrate = 3;
		}
		else if(baud==UART_BAUD_921600)	//校准921600
		{
			baudrate = 11;
		}
		else if(baud==UART_BAUD_806400)	//校准806400
		{
			osc = 10;
			outw(UARTC_OSCR_REG, osc);
			baudrate = 10;
		}
		else if(baud==UART_BAUD_691200)	//校准691200
		{
			baudrate = 15;
		}
		else if(baud==UART_BAUD_115200)	//校准115200，算出来应该是86.8，所以设置87
		{
			baudrate = 87;
		}
	}

	outw(UARTC_DLL_REG, (baudrate >> 0) & 0xff);
	outw(UARTC_DLM_REG, (baudrate >> 8) & 0xff);
	
	outw(UARTC_LCR_REG, value & 0xFFFFFF7F);
}



/**
 * @brief 计算32bit中1的个数（通用型计算函数）
 */
int count_one_bits(unsigned int n) // 计算二进制1的个数
{
	int count = 0;
	while (n)
	{
		n = n & (n - 1);
		count++;
	}
	return count;
}

/**
 * @brief flash写加密判断
 */
void flash_encrypt_write_check()
{
	int one_bits_num = count_one_bits((g_efuse_ctrl&EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK));
	if((g_efuse_ctrl&EFUSE_CTRL_FLASH_ENCRYPT_EN) && 
		((g_efuse_ctrl&EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK) != EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK) && 
		(one_bits_num%2))  // 1、使能了加密 2、计数标识不全为F 3、计数标识中1的个数为奇数
	{
		OUT32(SOC_PD_APB_ENCRYPT_EN, APB_WRIT_ENCRYPT);
	}
}

/**
 * @brief efsue防老化，agc memory数据更新标志发送
 * 使用0x201234寄存器的第28bit控制bootrom重新加载ecc秘钥
 * 使用0x201234寄存器的第27bit控制数据刷新
 */
void efuse_data_in_agc_memory_refresh(void)
{
	OUT32(SOC_AON_DUMMY_REG, IN32(SOC_AON_DUMMY_REG)&(~(1<<28)));
	OUT32(SOC_AON_DUMMY_REG, IN32(SOC_AON_DUMMY_REG)|(1<<27));
}




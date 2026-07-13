/**
 * @file uboot_cmd.c
 * @brief This is a brief description
 * @details This is the detail description
 * @author liuyong
 * @date 2021-1-20
 * @version V0.1
 * @par Copyright by http://eswin.com/.
 * @par History 1:
 *      Date:2021-9-11
 *      Version:V0.2
 *      Author:liuyong1@eswin.com
 *      Modification:for eco3

 * @par History 2:
 */


/*--------------------------------------------------------------------------
* 	                                           Include files
--------------------------------------------------------------------------*/
#include "common_def.h"
#include "uboot_init.h"
#include "uboot_cmd.h"
#include "uart.h"
#include "pit.h"
#include "trng.h"
#include "drv_spiflash.h"
#include "dbglog.h"
#include "env.h"
#include "../OpenBK7231T_App/libraries/miniz/miniz.h"

/*--------------------------------------------------------------------------
* 	                                           Local Macros
--------------------------------------------------------------------------*/
/** SPIFLASH_READY: spiflash ready value */
#define SPIFLASH_READY 				(1)
#define FLASH_SIZE_1_MB             (0x00100000)
#define FLASH_SIZE_2_MB             (0x00200000)
#define FLASH_SIZE_4_MB             (0x00400000)

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CRC_MODE 'C'
#define XMODEM_BLOCK_SIZE_1K 1024
#define XMODEM_BLOCK_SIZE_128 128

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
unsigned int g_flash_size = 0;
unsigned int all_nv_start_addr = 0, all_nv_end_addr = 0;
unsigned char partition_backup[0x1000] = {0}; // 用于flash加密时将明文分区表放到内存中


/*--------------------------------------------------------------------------
* 	                                          Global Variables
--------------------------------------------------------------------------*/
/**  message_ack_head: all ACK msg use this*/
struct message_ack_head ACK_msg = 
{
	.magic = ACK_MAGIC,
	.type = 0,
	.data_len = 0,
	.status = STATUS_SUCCESS,
	.CRC8 = 0
};

/**  cmd_data_buf: 64K size buffer*/
unsigned char cmd_data_buf[CMD_DATA_MAX_LEN] = {0};

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG	// 在log级别较高时打开
unsigned char stub_trace[32] = {0};
unsigned int stub_trace_index = 0;
#endif


/*--------------------------------------------------------------------------
* 	                                          Global Function Prototypes
--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
* 	                                          Function Definitions
--------------------------------------------------------------------------*/
	
/**
 * @brief 清空uart fifo
 */
void uart_fifo_reset(unsigned int uart_base)
{
	// 清空fifo
	outw(UARTC_FCR_REG, DRV_UART_FCR_FIFORST);
}

/**
 * @brief 用于消息完整性检测
 */
unsigned char uboot_mesage_check(unsigned char * buf, unsigned short length)
{
	unsigned int crc = 0;
	unsigned char ret = 0;
	unsigned int i = 0;

	for(i=0; i<length; i++)
	{
		crc += buf[i];
	}
	ret = crc%256;
	return ret;
}

/**
 * @brief flash初始化函数，并且获取flash大小并保存
 */
void sburner_flash_init(void)
{
	OUT32(SOC_PD_SPI_CONFIG, inw(SOC_PD_SPI_CONFIG) | 0x80);
#if defined(CPU_CLK_SET_240M)
	spiFlash_api_init(SPIFLASH_SCLK_DIV_2, SPIFLASH_READ_CMD_EB);  // CONFIG_STANDARD_SPI:0B  CONFIG_DUAL_SPI:BB  CONFIG_QUAD_SPI:EB
#else
	spiFlash_api_init(SPIFLASH_SCLK_DIV_0, SPIFLASH_READ_CMD_EB);  // CONFIG_STANDARD_SPI:0B  CONFIG_DUAL_SPI:BB  CONFIG_QUAD_SPI:EB
#endif
	// get flash id

	/* get flash size */
	/* 1M flash id: 0x14----  2M flash id: 0x15----  4M flash id: 0x16---- 
	 * no flash：0x00FFFFFF
	 */
	unsigned char id = (g_flash_id>>16)&0xff;  // Dynamically obtain flash size
	switch(id){
		case 0x14:
			g_flash_size = FLASH_SIZE_1_MB; break;
		case 0x15:
			g_flash_size = FLASH_SIZE_2_MB; break;
		case 0x16:
			g_flash_size = FLASH_SIZE_4_MB; break;
		default:
			uart_write((unsigned char * )"flash!", 6);
			g_flash_size = 0; break;
	}
}

static uint16_t crc16_ccitt(const uint8_t* data, uint16_t len)
{
	uint16_t crc = 0;
	while(len--)
	{
		crc ^= (uint16_t)(*data++) << 8;
		for(uint8_t i = 0; i < 8; i++)
			crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
	}
	return crc;
}

void uart_put_char(unsigned int uart_base, const char buf)
{
	outw(UARTC_THR_REG, buf);

	while(!(inw(UARTC_LSR_REG) & UARTC_LSR_TEMT));
}

/**
 * @brief ram download.
 * @details The detail function description.
 * @param[in] *buf: download data
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_ram_dl_handle(void * buf)
{
	unsigned int tempLen, i, offset = 0;
	unsigned char *addr;
	unsigned char rbuf[HEAD_SIZE] = {0};
	unsigned char CRC8 = 0;
	unsigned int last_frame_length = 0;
	
	struct message_rec_head rec_head;
	struct load_cfg_msg cfg_msg;
	struct message_rec_head * msg = (struct message_rec_head * )buf;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;
	
	MEMCPY(&cfg_msg,&(cmd_data_buf[HEAD_SIZE]),CFG_SIZE);

	/*[Begin addr_error] [liuyong-2021/4/13]*/
	if((cfg_msg.addr+cfg_msg.len) > IRAM1_BASE+IRAM1_SIZE)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		//todo :ERROR
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
		return;
	}
	/*[End addr_error] [liuyong-2021/4/13]*/

	//todo :ACK
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE);

	do
	{
		// init
		ACK_msg.status = STATUS_SUCCESS;
		//MEMSET(cmd_data_buf,0,CMD_DATA_MAX_LEN);
		
		// FRAME BEGAIN
		for(i=0;i<HEAD_SIZE;i++)
		{
			while (uart_data_tstc(SOC_UART0_BASE) == 0);
			uart_getc(SOC_UART0_BASE, (unsigned char *)&rbuf[i] ,UART_CMD_WAIT_TIMEOUT_MAX);
			if(rbuf[0] != MAGIC || (i>0 && rbuf[1] != STATE_RAM_DOWNLOAD))
			{
				i = 0xFFFFFFFF;
			}
		}
		MEMCPY(&cmd_data_buf,rbuf,HEAD_SIZE);
		MEMCPY(&rec_head,rbuf,HEAD_SIZE);

		// modified add + 1, for resend, 20210330
		tempLen = MIN(LOAD_MAX_SIZE_BIG+1, rec_head.data_len);  // get frame len, MAX is 1024
		addr = (unsigned char *)(cfg_msg.addr + offset);

		for (i=0; i<tempLen; i++)
		{
			while (uart_data_tstc(SOC_UART0_BASE) == 0);
			uart_getc(SOC_UART0_BASE, (unsigned char *)&cmd_data_buf[HEAD_SIZE+i] ,UART_CMD_WAIT_TIMEOUT_MAX);
		}

		//CRC	
		while (uart_data_tstc(SOC_UART0_BASE) == 0);
		uart_getc(SOC_UART0_BASE, (unsigned char *)&cmd_data_buf[HEAD_SIZE+tempLen] ,UART_CMD_WAIT_TIMEOUT_MAX);

		CRC8 = uboot_mesage_check((unsigned char * )cmd_data_buf, HEAD_SIZE+tempLen);
		if(cmd_data_buf[HEAD_SIZE+tempLen] != CRC8)
		{
			ACK_msg.status = STATUS_CRC_ERROR;
		}

		if(cmd_data_buf[HEAD_SIZE] == 0x1)
		{
			//重传帧
			addr -= last_frame_length;
		}
		last_frame_length = tempLen-1;

		// copy data to ram
		MEMCPY(addr, &(cmd_data_buf[HEAD_SIZE+1]), tempLen-1);

		//todo :ACK
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
		
		cfg_msg.len -= (tempLen-1);
		offset += (tempLen-1);
	}while(cfg_msg.len);
}

/**
 * @brief Flash Download.
 * @details The detail function description.
 * @param[in] *buf: download buffer
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_flash_dl_handle(void * buf)
{
	unsigned int tempLen = 0, offset = 0;
	unsigned int sector_erase_len = 0, erase_ret = 0, block_erase_cnt = 0;
	unsigned int i = 0;
	unsigned int ret = 0;
	unsigned int back_ret = IN32(SOC_PD_APB_ENCRYPT_EN);
	unsigned char CRC8 = 0;
	unsigned char flash_write_buffer[LOAD_MAX_SIZE_BIG];
	unsigned int last_frame_length = 0;
	
	struct message_rec_head rec_head;  // 4 bytes frame header in downloading
	struct load_cfg_msg cfg_msg;  // 8 bytes of addr and len
	struct message_rec_head * msg = (struct message_rec_head * )buf;  // get message
	
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;

	MEMCPY(&cfg_msg,&(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);  // cmd_data_buf contains 4 bytes of HEAD and 8 bits of data

	/*[Begin addr_error] [liuyong-2021/4/13]*/
	if((cfg_msg.addr+cfg_msg.len) > g_flash_size)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		//todo :ERROR
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg, ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
		return;
	}
	/*[End addr_error] [liuyong-2021/4/13]*/

	if (cfg_msg.len)
	{
		//是否需要用block erase
		if (cfg_msg.len < SPIFLASH_BLOCK_SIZE){
			spiFlash_api_erase(cfg_msg.addr, cfg_msg.len);
		}
		else
		{
			//地址不是block size对齐的前一部分区域的擦除
			if(cfg_msg.addr % SPIFLASH_BLOCK_SIZE)
			{
				sector_erase_len = SPIFLASH_BLOCK_SIZE - (cfg_msg.addr % SPIFLASH_BLOCK_SIZE);
				//判断前一部分不对齐时是否可以使用half block erase
				if(sector_erase_len < SPIFLASH_HALF_BLOCK_SIZE)
				{
					spiFlash_api_erase(cfg_msg.addr, sector_erase_len);
				}
				else
				{	//判断长度是否half block size对齐？ 将不对齐部分用sector erase擦除
					if(sector_erase_len - SPIFLASH_HALF_BLOCK_SIZE)
					{
						spiFlash_api_erase(cfg_msg.addr, sector_erase_len - SPIFLASH_HALF_BLOCK_SIZE);
						spiFlash_api_erase_half_block(cfg_msg.addr + (sector_erase_len - SPIFLASH_HALF_BLOCK_SIZE),SPIFLASH_HALF_BLOCK_SIZE );
					}
					else
					{
						spiFlash_api_erase_half_block(cfg_msg.addr, sector_erase_len);
					}
				}
			}

			//更新剩余擦除长度情况。
			erase_ret = cfg_msg.len - sector_erase_len;
			block_erase_cnt = (erase_ret / SPIFLASH_BLOCK_SIZE);
			
			//地址为block size对齐 并且 长度大于block size的部分使用block size擦除
			if((((cfg_msg.addr + sector_erase_len) % SPIFLASH_BLOCK_SIZE) == 0) && block_erase_cnt)
			{
				spiFlash_api_erase_block(cfg_msg.addr + sector_erase_len,block_erase_cnt * SPIFLASH_BLOCK_SIZE );
				erase_ret -= block_erase_cnt * SPIFLASH_BLOCK_SIZE ;
			}
			
			//剩余擦除长度不为0时的擦除
			if(erase_ret)
			{	//判断剩余部分的长度是否需要使用half block erase擦除先擦除一部分
				if(erase_ret < SPIFLASH_HALF_BLOCK_SIZE){
					spiFlash_api_erase(cfg_msg.addr + (cfg_msg.len - erase_ret), erase_ret);}
				else
				{
					spiFlash_api_erase_half_block(cfg_msg.addr + (cfg_msg.len - erase_ret), SPIFLASH_HALF_BLOCK_SIZE );
					erase_ret -= SPIFLASH_HALF_BLOCK_SIZE;
					if(erase_ret){
						spiFlash_api_erase(cfg_msg.addr + (cfg_msg.len - erase_ret), erase_ret);}
				}
			}
		}
	}

	ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg, ACK_SIZE-1);
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE);

	do
	{
		// init
		ACK_msg.status = STATUS_SUCCESS;
		//MEMSET(cmd_data_buf,0,CMD_DATA_MAX_LEN);

		// FRAME BEGAIN, 4 bytes HEAD
		for(i=0;i<HEAD_SIZE;i++)
		{
			while (uart_data_tstc(SOC_UART0_BASE) == 0);
			uart_getc(SOC_UART0_BASE, (unsigned char *)&cmd_data_buf[i] ,UART_CMD_WAIT_TIMEOUT_MAX);
			// Confirm magic word and frame type
			if(cmd_data_buf[0] != MAGIC || (i>0 && cmd_data_buf[1] != STATE_FLASH_DOWNLOAD))
			{
				i = 0xFFFFFFFF;
			}
		}
		MEMCPY(&rec_head,&cmd_data_buf,HEAD_SIZE);  // Copy the received 4 bytes data and analyze data_len

		// modified add + 1, for resend, 20210330
		tempLen = MIN(LOAD_MAX_SIZE_BIG+1, rec_head.data_len);  // get frame len, MAX is 1024
		
//		for (i=0; i<tempLen; i++)  // get tempLen data, and save after &cmd_data_buf[HEAD_SIZE]
//		{
//			while (uart_data_tstc(SOC_UART0_BASE) == 0);
//			uart_getc(SOC_UART0_BASE, (unsigned char *)&cmd_data_buf[HEAD_SIZE+i], UART_CMD_WAIT_TIMEOUT_MAX);
//		}
		uart_read((char *)&cmd_data_buf[HEAD_SIZE], tempLen);
		
		// CRC8
		while (uart_data_tstc(SOC_UART0_BASE) == 0);
		uart_getc(SOC_UART0_BASE, (unsigned char *)&cmd_data_buf[HEAD_SIZE+tempLen], UART_CMD_WAIT_TIMEOUT_MAX);
		
		CRC8 = uboot_mesage_check((unsigned char *)cmd_data_buf, HEAD_SIZE+tempLen);
		if(cmd_data_buf[HEAD_SIZE+tempLen] != CRC8)
		{
			ACK_msg.status = STATUS_CRC_ERROR;
		}
		
		if(cmd_data_buf[HEAD_SIZE] == 0x1)
		{
			//重传帧
			cfg_msg.len += last_frame_length;
			offset -= last_frame_length;
		}
		last_frame_length = tempLen-1;

		// write to flash
		MEMCPY(flash_write_buffer, (unsigned char *)(&cmd_data_buf[HEAD_SIZE+1]), tempLen-1);

		if(cfg_msg.addr == 0x0)
		{
#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG	// 在log级别较高时打开
			stub_trace[stub_trace_index++] = 'a';
#endif
			MEMCPY(&partition_backup[offset], flash_write_buffer, tempLen-1);
		}
		
		if(IN32(SOC_PD_APB_ENCRYPT_EN) && //使能了写加密
			0 != all_nv_start_addr && //初始化过nv区
			((cfg_msg.addr + offset) >= all_nv_start_addr) && 
			((cfg_msg.addr + offset) <= (all_nv_end_addr))) // 要烧录的地址范围在nv区
		{
			OUT32(SOC_PD_APB_ENCRYPT_EN, 0); //不加密
#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG	// 在log级别较高时打开
			stub_trace[stub_trace_index++] = 'b';
#endif
		}
		ret = spiFlash_api_write(cfg_msg.addr + offset, flash_write_buffer, tempLen-1);
		
		OUT32(SOC_PD_APB_ENCRYPT_EN, back_ret);

		if(ret != 0)
		{
			ACK_msg.status = STATUS_ERROR;
		}

		// 反馈
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg, ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg,  ACK_SIZE);

		// Change the flash write offset and current data length, ready to receive the remaining data
		cfg_msg.len -= (tempLen-1);
		offset += (tempLen-1);
		
		// 清空fifo
		uart_fifo_reset(SOC_UART0_BASE);
	}while(cfg_msg.len);
}

/**
 * @brief Flash export.
 * @details The detail function description.
 * @param[in] *buf: export buffer
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_flash_ul_handle(void * buf)
{
	unsigned int tempLen, offset = 0;
	struct load_cfg_msg cfg_msg;
	struct message_rec_head * msg = (struct message_rec_head * )buf;

	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.status = STATUS_SUCCESS;
	
	MEMCPY(&cfg_msg,&(cmd_data_buf[HEAD_SIZE]),CFG_SIZE);

	/*[Begin] [liuyong-2021/4/21]*/
	// addr and length judge
	if((cfg_msg.addr + cfg_msg.len) > g_flash_size)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		ACK_msg.data_len = 0;
		ACK_msg.CRC8 =  uboot_mesage_check((unsigned char * )&ACK_msg, ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
		return;
	}
	/*[End] [liuyong-2021/4/21]*/

	do
	{
		tempLen = MIN(LOAD_MAX_SIZE_BIG, cfg_msg.len);
		spiFlash_api_read(cfg_msg.addr + offset, (unsigned char *)(&cmd_data_buf[HEAD_SIZE]), tempLen);
		ACK_msg.data_len = tempLen;
		MEMCPY(cmd_data_buf,&ACK_msg,HEAD_SIZE);
		cmd_data_buf[HEAD_SIZE+tempLen] = STATUS_SUCCESS;
		cmd_data_buf[HEAD_SIZE+tempLen+1] = uboot_mesage_check((unsigned char * )cmd_data_buf,HEAD_SIZE+tempLen+1) ;
		uart_write((unsigned char * )cmd_data_buf, HEAD_SIZE+tempLen+2);
		cfg_msg.len -= tempLen;
		offset += tempLen;
	}while(cfg_msg.len);
}

/**
 * @brief Flash erase.
 * @details The detail function description.
 * @param[in] *buf: your data, import address
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_flash_erase_handle(void * buf)
{
	struct load_cfg_msg cfg_msg;
	struct message_rec_head * msg = (struct message_rec_head * )buf;
	
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;

	MEMCPY(&cfg_msg,&(cmd_data_buf[HEAD_SIZE]),CFG_SIZE);
	
	/*[Begin] [liuyong-2021/4/21]*/
	// addr and length judge
	if((cfg_msg.addr + cfg_msg.len) > g_flash_size)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg, ACK_SIZE-1);
		uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
		return;
	}
	/*[End] [liuyong-2021/4/21]*/
	
	spiFlash_api_erase(cfg_msg.addr, cfg_msg.len);
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,  ACK_SIZE-1);
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
}

/**
 * @brief Flash whole chip erase.
 * @details The detail function description.
 * @param[in] void:
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_flash_chiperase_handle(void * buf)
{
	struct message_rec_head * msg = (struct message_rec_head * )buf;
	
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;

	spiFlash_api_chiperase();
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,  ACK_SIZE-1);
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
}

/**
 * @brief Uboot handshake process.
 * @details The detail function description.
 * @param[in] void:
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
void uboot_sync(void)
{
	unsigned int i = 0;
	struct message_rec_head * msg = (struct message_rec_head * )cmd_data_buf;
	
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;
	
	//SYNC
	for (i=0; i<SYNC_REQUEST_SIZE; i++)
	{
		if (cmd_data_buf[HEAD_SIZE + i] != (char)((SYNC_REQUEST_VALUE>>(i*8)) & 0xff))
		{
			ACK_msg.status = STATUS_ERROR;
			ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
			uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
			return;	//eroor, again
		}
	}
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
}

/**
 * @brief Modify UART baud rate.
 * @details The detail function description.
 * @param[in] void:
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note use globle cmd_data_buf
 */
void uboot_buad(void)
{
	struct message_rec_head * msg = (struct message_rec_head * )cmd_data_buf;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
	boot_uart_set_baudrate(SOC_UART0_BASE, msg->run_addr, 0);  // 切换波特率
	boot_usdelay(50000); //可以设置4ms  ,目前50ms是兼容配置,适配较低速的电脑, ----工具侧延时3ms切换波特率
	uart_write((unsigned char * )&ACK_msg, ACK_SIZE); //输出反馈
}
void uboot_flashid(void)
{
	struct message_rec_head* msg = (struct message_rec_head*)cmd_data_buf;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);
	ACK_msg.data_len = 4;
	MEMCPY(cmd_data_buf, &ACK_msg, HEAD_SIZE);
	MEMCPY(&cmd_data_buf[HEAD_SIZE], &g_flash_id, 4);
	cmd_data_buf[HEAD_SIZE + 4] = STATUS_SUCCESS;
	cmd_data_buf[HEAD_SIZE + 4 + 1] = uboot_mesage_check((unsigned char*)cmd_data_buf, HEAD_SIZE + 4 + 1);
	uart_write((unsigned char*)cmd_data_buf, HEAD_SIZE + 4 + 2);
}

void uboot_flash_xmodem_dl(void* buf)
{
	uint8_t header[3] = { 0x00 };
	uint8_t data[XMODEM_BLOCK_SIZE_1K] = { 0xFF };
	uint8_t crc_bytes[2] = { 0x00 };
	uint16_t crc_calc, crc_recv;
	uint32_t offset = 0;
	struct load_cfg_msg cfg_msg;
	struct message_rec_head* msg = (struct message_rec_head*)buf;
	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);

	MEMCPY(&cfg_msg, &(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);

	if((cfg_msg.addr + cfg_msg.len) > g_flash_size)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);
		uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
		return;
	}

	uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
	boot_usdelay(100000);

	spiFlash_api_erase(cfg_msg.addr, cfg_msg.len);

	uart_put_char(SOC_UART0_BASE, CRC_MODE);

	for(;;)
	{
		// header
		if(uart_getc(SOC_UART0_BASE, &header[0], 3333) != 0)
		{
			uart_put_char(SOC_UART0_BASE, CRC_MODE);
			continue;
		}

		if(header[0] == EOT)
		{
			uart_put_char(SOC_UART0_BASE, ACK);
			break;
		}

		if(header[0] != STX)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		uart_getc(SOC_UART0_BASE, &header[1], 10000);
		uart_getc(SOC_UART0_BASE, &header[2], 10000);

		if((header[1] + header[2]) != 0xFF)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		// recv data + crc
		for(int i = 0; i < XMODEM_BLOCK_SIZE_1K; i++)
			uart_getc(SOC_UART0_BASE, &data[i], 20000);
		uart_getc(SOC_UART0_BASE, &crc_bytes[0], 10000);
		uart_getc(SOC_UART0_BASE, &crc_bytes[1], 10000);

		crc_recv = ((uint16_t)crc_bytes[0] << 8) | crc_bytes[1];
		crc_calc = crc16_ccitt(data, XMODEM_BLOCK_SIZE_1K);

		if(crc_recv != crc_calc)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		spiFlash_api_write(cfg_msg.addr + offset, data, XMODEM_BLOCK_SIZE_1K);
		offset += XMODEM_BLOCK_SIZE_1K;

		uart_put_char(SOC_UART0_BASE, ACK);
	}
}

void uboot_flash_xmodem_ul(bool isRaw, void* buf)
{
	uint8_t block_num = 1;
	uint8_t data[XMODEM_BLOCK_SIZE_1K];
	uint8_t resp;
	uint32_t offset = 0;
	int retry, ret;
	struct load_cfg_msg cfg_msg;
	struct message_rec_head* msg = (struct message_rec_head*)buf;
	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);

	MEMCPY(&cfg_msg, &(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);
	uint32_t data_len = cfg_msg.len;

	uart_write((unsigned char*)&ACK_msg, ACK_SIZE);

	// wait for 'C'
	do
	{
		if(uart_getc(SOC_UART0_BASE, &resp, 30000) != 0)
			continue;
	} while(resp != CRC_MODE);

	while(data_len > 0)
	{
		uint32_t chunk = (data_len > XMODEM_BLOCK_SIZE_1K) ? XMODEM_BLOCK_SIZE_1K : data_len;
		memset(data, 0xFF, XMODEM_BLOCK_SIZE_1K); // pad with 0xFF

		if(isRaw)
		{
			memcpy(data, (void*)cfg_msg.addr + offset, chunk);
		}
		else
		{
			spiFlash_api_read(cfg_msg.addr + offset, data, chunk);
		}

		uint16_t crc = crc16_ccitt(data, XMODEM_BLOCK_SIZE_1K);

		retry = 0;
		do
		{
			uart_put_char(SOC_UART0_BASE, STX);
			uart_put_char(SOC_UART0_BASE, block_num);
			uart_put_char(SOC_UART0_BASE, ~block_num);
			for(uint16_t i = 0; i < XMODEM_BLOCK_SIZE_1K; i++)
				uart_put_char(SOC_UART0_BASE, data[i]);
			uart_put_char(SOC_UART0_BASE, (crc >> 8) & 0xFF);
			uart_put_char(SOC_UART0_BASE, crc & 0xFF);

			// wait ack/nak
			ret = uart_getc(SOC_UART0_BASE, &resp, 3000);
			if(ret == 0 && resp == ACK)
				break;
			retry++;
		} while(retry < 10);

		if(retry >= 10)
		{
			uart_put_char(SOC_UART0_BASE, CAN);
			return;
		}

		offset += chunk;
		data_len -= chunk;
		block_num++;
	}

	// eot
	retry = 0;
	do
	{
		uart_put_char(SOC_UART0_BASE, EOT);
		ret = uart_getc(SOC_UART0_BASE, &resp, 3000);
		if(ret == 0 && resp == ACK)
			break;
		retry++;
	} while(retry < 10);

	return;
}

int mz_deflateInit3(mz_streamp pStream, int level, int method, int window_bits, int mem_level, int strategy)
{
	tdefl_compressor* pComp;
	mz_uint comp_flags = tdefl_create_comp_flags_from_zip_params(level, window_bits, strategy);

	if(!pStream)
		return MZ_STREAM_ERROR;
	if((method != MZ_DEFLATED) || ((mem_level < 1) || (mem_level > 9)) || ((window_bits != MZ_DEFAULT_WINDOW_BITS) && (-window_bits != MZ_DEFAULT_WINDOW_BITS)))
		return MZ_PARAM_ERROR;

	pStream->data_type = 0;
	pStream->adler = 0;
	pStream->msg = NULL;
	pStream->reserved = 0;
	pStream->total_in = 0;
	pStream->total_out = 0;
	if(!pStream->zalloc)
		pStream->zalloc = miniz_def_alloc_func;
	if(!pStream->zfree)
		pStream->zfree = miniz_def_free_func;

	pComp = (tdefl_compressor*)pStream->zalloc(pStream->opaque, 1, sizeof(tdefl_compressor));
	if(!pComp)
		return MZ_MEM_ERROR;

	pStream->state = (struct mz_internal_state*)pComp;

	if(tdefl_init(pComp, NULL, NULL, comp_flags) != TDEFL_STATUS_OKAY)
	{
		mz_deflateEnd(pStream);
		return MZ_PARAM_ERROR;
	}

	return MZ_OK;
}

void uboot_flash_xmodem_ul_z(void* buf)
{
	uint8_t block_num = 1;
	uint8_t resp = 0;
	int retry;
	int ret;
	bool use_1k = true;
	bool use_crc = true;

	uint8_t packet[3 + XMODEM_BLOCK_SIZE_1K + 2];

	struct load_cfg_msg cfg_msg;
	struct message_rec_head* msg = (struct message_rec_head*)buf;

	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);

	uint8_t comp_level = cmd_data_buf[HEAD_SIZE + CFG_SIZE];
	if(comp_level < 1 || comp_level > 10) comp_level = 5;

	memcpy(&cfg_msg, &(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);

	uart_write((unsigned char*)&ACK_msg, ACK_SIZE);

	int timeout = 10000;

	while(timeout > 0)
	{
		if(uart_getc(SOC_UART0_BASE, &resp, 1000) == 0)
		{
			if(resp == CRC_MODE)
			{
				use_crc = true;
				use_1k = true;
				break;
			}

			if(resp == NAK)
			{
				use_crc = false;
				use_1k = false;
				break;
			}

			if(resp == CAN) return;
		}

		timeout -= 1000;
	}

	if(timeout <= 0)
	{
		uart_put_char(SOC_UART0_BASE, CAN);
		uart_put_char(SOC_UART0_BASE, CAN);
		return;
	}

	z_stream stream;

	memset(&stream, 0, sizeof(stream));

	const uint8_t* src = (const uint8_t*)(SOC_XIP_BASE + cfg_msg.addr);

	uint32_t remaining = cfg_msg.len;

	if(mz_deflateInit3(&stream, comp_level, MZ_DEFLATED, -MZ_DEFAULT_WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY) != Z_OK)
	{
		return;
	}

	bool finished = false;

	while(!finished)
	{
		uint32_t block_size;
		uint8_t header;

		if(use_1k)
		{
			block_size = XMODEM_BLOCK_SIZE_1K;
			header = STX;
		}
		else
		{
			block_size = 128;
			header = SOH;
		}

		memset(packet, 0xFF, sizeof(packet));

		packet[0] = header;
		packet[1] = block_num;
		packet[2] = ~block_num;

		stream.next_out = &packet[3];
		stream.avail_out = block_size;

		while(stream.avail_out)
		{
			if(stream.avail_in == 0 && remaining)
			{
				uint32_t n = remaining;

				if(n > block_size) n = block_size;

				stream.next_in = (unsigned char*)(src + (cfg_msg.len - remaining));

				stream.avail_in = n;

				remaining -= n;
			}

			ret = deflate(&stream, remaining ? Z_NO_FLUSH : Z_FINISH);

			if(ret == Z_STREAM_END)
			{
				finished = true;
				break;
			}

			if(ret != Z_OK)
			{
				deflateEnd(&stream);
				return;
			}
		}

		uint32_t pkt_len = 3 + block_size;

		if(use_crc)
		{
			uint16_t crc = crc16_ccitt(&packet[3], block_size);

			packet[pkt_len++] = crc >> 8;
			packet[pkt_len++] = crc & 0xff;
		}
		else
		{
			uint8_t sum = 0;

			for(uint32_t i = 0; i < block_size; i++)
			{
				sum += packet[3 + i];
			}

			packet[pkt_len++] = sum;
		}

		retry = 0;

		while(retry < 10)
		{
			uart_write(packet, pkt_len);

			ret = uart_getc(SOC_UART0_BASE, &resp, 5000);

			if(ret == 0 && resp == ACK)
			{
				break;
			}

			retry++;
		}

		//if(use_1k && retry >= 7)
		//{
		//	use_1k = false;
		//}

		if(retry >= 10)
		{
			deflateEnd(&stream);

			uart_put_char(SOC_UART0_BASE, CAN);
			uart_put_char(SOC_UART0_BASE, CAN);

			return;
		}

		block_num++;
	}

	deflateEnd(&stream);

	retry = 0;

	while(retry < 10)
	{
		uart_put_char(SOC_UART0_BASE, EOT);

		ret = uart_getc(SOC_UART0_BASE, &resp, 5000);

		if(ret == 0 && resp == ACK)
		{
			return;
		}

		retry++;
	}

	uart_put_char(SOC_UART0_BASE, CAN);
	uart_put_char(SOC_UART0_BASE, CAN);
}

void uboot_flash_sha256(void* buf)
{
	struct load_cfg_msg cfg_msg;
	struct message_rec_head* msg = (struct message_rec_head*)buf;

	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 32;

	MEMCPY(&cfg_msg, &(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);

	if((cfg_msg.addr + cfg_msg.len) > g_flash_size)
	{
		ACK_msg.data_len = 0;
		ACK_msg.status = STATUS_ADDR_ERROR;
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);
		uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
		return;
	}
	unsigned char hash[32] = { 0 };
	extern void sha256(const unsigned char* message, unsigned int len, unsigned char* digest);
	sha256((void*)(SOC_XIP_BASE + cfg_msg.addr), cfg_msg.len, hash);
	MEMCPY(cmd_data_buf, &ACK_msg, HEAD_SIZE);
	MEMCPY(&cmd_data_buf[HEAD_SIZE], &hash, 32);
	cmd_data_buf[HEAD_SIZE + 32] = STATUS_SUCCESS;
	cmd_data_buf[HEAD_SIZE + 32 + 1] = uboot_mesage_check((unsigned char*)cmd_data_buf, HEAD_SIZE + 32 + 1);
	uart_write((unsigned char*)cmd_data_buf, HEAD_SIZE + 32 + 2);
}

static uint32_t flash_buf_used = 0;
static int flash_write_aligned(uint32_t* flash_offset,
	uint8_t* src,
	uint32_t len,
	uint32_t flash_end)
{
	uint8_t* flash_buf = cmd_data_buf + (32 * 1024);
	memcpy(flash_buf + flash_buf_used, src, len);
	flash_buf_used += len;

	uint32_t write_len = flash_buf_used & ~(3);

	if(write_len)
	{
		if((*flash_offset + write_len) > flash_end)
			return -1;

		spiFlash_api_write(*flash_offset, flash_buf, write_len);

		*flash_offset += write_len;

		flash_buf_used -= write_len;
		memmove(flash_buf, flash_buf + write_len, flash_buf_used);
	}

	return 0;
}

void uboot_flash_xmodem_dl_z(void* buf)
{
	uint8_t header[3] = { 0x00 };
	uint8_t data[XMODEM_BLOCK_SIZE_1K] = { 0xFF };
	uint8_t crc_bytes[2] = { 0x00 };
	uint16_t crc_calc, crc_recv;
	uint32_t flash_offset = 0;
	const uint32_t cmdbufsize = 32 * 1024;
	struct load_cfg_msg cfg_msg;
	struct message_rec_head* msg = (struct message_rec_head*)buf;

	ACK_msg.status = STATUS_SUCCESS;
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = msg->type;
	ACK_msg.data_len = 0x0000;
	ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);

	memcpy(&cfg_msg, &(cmd_data_buf[HEAD_SIZE]), CFG_SIZE);

	if((cfg_msg.addr + cfg_msg.len) > g_flash_size)
	{
		ACK_msg.status = STATUS_ADDR_ERROR;
		ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);
		uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
		return;
	}

	uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
	boot_usdelay(100000);
	spiFlash_api_erase(cfg_msg.addr, cfg_msg.len);

	mz_stream stream;
	memset(&stream, 0, sizeof(stream));
	if(mz_inflateInit2(&stream, -MZ_DEFAULT_WINDOW_BITS) != MZ_OK)
	{
		ACK_msg.status = STATUS_ERROR;
		uart_write((unsigned char*)&ACK_msg, ACK_SIZE);
		return;
	}

	uart_put_char(SOC_UART0_BASE, CRC_MODE);

	flash_offset = cfg_msg.addr;

	for(;;)
	{
		uint32_t data_size = 0;

		if(uart_getc(SOC_UART0_BASE, &header[0], 3333) != 0)
		{
			uart_put_char(SOC_UART0_BASE, CRC_MODE);
			continue;
		}

		if(header[0] == EOT)
		{
			stream.next_in = NULL;
			stream.avail_in = 0;

			while(1)
			{
				stream.next_out = cmd_data_buf;
				stream.avail_out = cmdbufsize;

				int status = mz_inflate(&stream, MZ_NO_FLUSH);
				uint32_t produced = cmdbufsize - stream.avail_out;

				if(produced > 0)
				{
					if(flash_write_aligned(&flash_offset,
						cmd_data_buf,
						produced,
						cfg_msg.addr + cfg_msg.len) != 0)
					{
						goto abort_decompression;
					}
				}

				if(status == MZ_STREAM_END) break;

				if(status != MZ_OK && status != MZ_BUF_ERROR)
					goto abort_decompression;
			}

			mz_inflateEnd(&stream);

			uart_put_char(SOC_UART0_BASE, ACK);
			return;
		}

		if(header[0] != STX && header[0] != SOH)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		data_size = (header[0] == STX) ? XMODEM_BLOCK_SIZE_1K : XMODEM_BLOCK_SIZE_128;

		uart_getc(SOC_UART0_BASE, &header[1], 10000);
		uart_getc(SOC_UART0_BASE, &header[2], 10000);

		if((header[1] + header[2]) != 0xFF)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		for(int i = 0; i < data_size; i++)
			uart_getc(SOC_UART0_BASE, &data[i], 20000);

		uart_getc(SOC_UART0_BASE, &crc_bytes[0], 10000);
		uart_getc(SOC_UART0_BASE, &crc_bytes[1], 10000);

		crc_recv = ((uint16_t)crc_bytes[0] << 8) | crc_bytes[1];
		crc_calc = crc16_ccitt(data, data_size);

		if(crc_recv != crc_calc)
		{
			uart_put_char(SOC_UART0_BASE, NAK);
			continue;
		}

		stream.next_in = data;
		stream.avail_in = data_size;

		while(stream.avail_in > 0)
		{
			stream.next_out = cmd_data_buf;
			stream.avail_out = cmdbufsize;

			int status = mz_inflate(&stream, MZ_NO_FLUSH);
			uint32_t produced = cmdbufsize - stream.avail_out;

			if(produced > 0)
			{
				if(flash_write_aligned(&flash_offset,
					cmd_data_buf,
					produced,
					cfg_msg.addr + cfg_msg.len) != 0)
				{
					goto abort_decompression;
				}
			}

			if(status == MZ_STREAM_END) break;

			if(status != MZ_OK && status != MZ_BUF_ERROR)
				goto abort_decompression;
		}

		uart_put_char(SOC_UART0_BASE, ACK);
	}

abort_decompression:
	mz_inflateEnd(&stream);

	uart_put_char(SOC_UART0_BASE, CAN);
	uart_put_char(SOC_UART0_BASE, CAN);
	uart_put_char(SOC_UART0_BASE, 'N');
}

#include "soc_top_reg.h"
#include "clk.h"
void uboot_read_efuse()
{
	ACK_msg.magic = ACK_MAGIC;
	ACK_msg.type = 0x99;
	ACK_msg.data_len = 0x80;
	CLK_ENABLE(SOC_PD_CLK_EN_BASE0, EFUSE_CLK_BIT | EFUSE_APB_CLK_BIT);
	memcpy(cmd_data_buf, &ACK_msg, HEAD_SIZE);
	efuse_read_series(0, cmd_data_buf + HEAD_SIZE, 0x80);
	//CLK_DISABLE(SOC_PD_CLK_EN_BASE0, EFUSE_CLK_BIT | EFUSE_APB_CLK_BIT);

	cmd_data_buf[HEAD_SIZE + ACK_msg.data_len] = STATUS_SUCCESS;
	cmd_data_buf[HEAD_SIZE + ACK_msg.data_len + 1] = uboot_mesage_check((unsigned char*)cmd_data_buf, HEAD_SIZE + ACK_msg.data_len + 1);
	uart_write((unsigned char*)cmd_data_buf, HEAD_SIZE + ACK_msg.data_len + 2);
}

/* ********************************************************************************************************* */
/**
 * @brief Analysis of UART command.
 * @details The detail function description.
 * @param[in] void:
 * @return void:
 * @par Demo:
 * @code
 *		Description of function demo
 * @endcode
 * @note Description of function note
 */
int uart_cmd_parser(void)
{
	unsigned int i = 0;
	unsigned char resp = 0;
	unsigned char CRC8 = 0;
	unsigned char buf[HEAD_SIZE] = {0};		// not parser
	struct message_rec_head rec_head;		// parser buf 4 bytes data, can get data_len.
	
	unsigned char cfgbuf[CFG_SIZE] = {0};	// 8 bytes data buf

	// 清空fifo
	uart_fifo_reset(SOC_UART0_BASE);

	// flash 初始化
	sburner_flash_init();
	
#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG	// 在log级别较高时打开
	stub_trace[stub_trace_index++] = '0';	//用于追踪的轨迹log
#endif

	do{
		MEMSET(buf,0,HEAD_SIZE);  // 4 bytes HEAD buf
		for(i=0;i<HEAD_SIZE;++i)
		{
			while (uart_data_tstc(SOC_UART0_BASE) == 0);  // waite uart data
			resp = uart_getc(SOC_UART0_BASE, (unsigned char *)&buf[i],	UART_CMD_WAIT_TIMEOUT_MAX);
			if(resp < 0) {  // timeout: UART_CMD_WAIT_TIMEOUT_MAX * 100 us
				resp = MSG_ERR;
				return resp;
			}
			if((buf[0]) != MAGIC)  // receive first data not MAGIC, wait other data
			{
				i = 0xFFFFFFFF;
			}
		}
		
        //MEMSET(cmd_data_buf,0,CMD_DATA_MAX_LEN);
    	MEMCPY(cmd_data_buf,buf,HEAD_SIZE);  // last buf, save 4 bytes head data
		MEMCPY(&rec_head,buf,HEAD_SIZE);  // data head, save 4 bytes to rec_head, and parser

		// check HEAD STATE
		if(buf[1] <= 0x9F)
		{
			for(i=0;i<rec_head.data_len;i++)  // parser buf 4 bytes data, can get data_len.
			{
				while (uart_data_tstc(SOC_UART0_BASE) == 0);  // waite uart data
				// receive data_len data to cfgbuf
				resp = uart_getc(SOC_UART0_BASE, (unsigned char *)&cfgbuf[i],  UART_CMD_WAIT_TIMEOUT_MAX);
		        if(resp < 0) {
		            resp = MSG_ERR;
		            return resp;
       	 		}
			}
    		MEMCPY(&(cmd_data_buf[HEAD_SIZE]),cfgbuf,rec_head.data_len);  // last buf, save 8 bytes data
			
			// CRC8
			while (uart_data_tstc(SOC_UART0_BASE) == 0);  // waite 1bytes crc8 data
			resp = uart_getc(SOC_UART0_BASE,(unsigned char *)&cmd_data_buf[HEAD_SIZE+rec_head.data_len],UART_CMD_WAIT_TIMEOUT_MAX);
			if(resp < 0) {
				resp = MSG_ERR;
				return resp;
			}
			CRC8 = uboot_mesage_check((unsigned char * )cmd_data_buf, HEAD_SIZE+rec_head.data_len);
			if(cmd_data_buf[HEAD_SIZE+rec_head.data_len] != CRC8)
			{
				rec_head.type = STATE_ERR;  // crc8 error
			}
		}
		else
		{
			rec_head.type = STATE_ERR;
		}


		// 根据命令类型解析
		switch (rec_head.type)
		{
			case STATE_SYN:
				uboot_sync();
				break;

			case STATE_RAM_DOWNLOAD:  // 0x01
				uboot_ram_dl_handle(&cmd_data_buf);
				break;

			case STATE_FLASH_DOWNLOAD:  // 0x02
				uboot_flash_dl_handle(&cmd_data_buf);

				//如果使能了加密，且未初始化nv地址，需要考虑三个nv区不加密
				if(IN32(SOC_PD_APB_ENCRYPT_EN) && 0==all_nv_start_addr) // 未初始化nv地址
				{
					unsigned int customer_addr, customer_len, customer_end;
					unsigned int develop_addr, develop_len, develop_end;
					unsigned int amt_addr, amt_len, amt_end;
					
					// 不同于bootrom，适配两段式新分区表的初始化函数
					boot_partion_init();
					
					boot_partion_info_get(PARTITION_NAME_NV_CUSTOMER, &customer_addr, &customer_len);
					boot_partion_info_get(PARTITION_NAME_NV_DEVELOP, &develop_addr, &develop_len);
					boot_partion_info_get(PARTITION_NAME_NV_AMT, &amt_addr, &amt_len);
					customer_end = customer_addr + customer_len;
					develop_end = develop_addr + develop_len;
					amt_end = amt_addr + amt_len;
					all_nv_start_addr = MIN(MIN(customer_addr, develop_addr), amt_addr);
					all_nv_end_addr = MAX(MAX(customer_end, develop_end), amt_end);
					
#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG	// 在log级别较高时打开
					stub_trace[stub_trace_index++] = 'b';
#endif
				}
				break;

			case STATE_FLASH_UPLOAD:  // 0x03
				uboot_flash_ul_handle(&cmd_data_buf);
				break;

			case STATE_FLASH_ERASE:  // 0x04
				uboot_flash_erase_handle(&cmd_data_buf);
				break;

			case STATE_FLASH_CHIPERASE:  // 0x05
				uboot_flash_chiperase_handle(&cmd_data_buf);
				break;
#if 1
			case STATE_RUN:  // 0x06
				ACK_msg.type = STATE_RUN;
				ACK_msg.data_len = 0x0000;
				ACK_msg.status = STATUS_SUCCESS;
				ACK_msg.CRC8 = uboot_mesage_check((unsigned char*)&ACK_msg, ACK_SIZE - 1);
				uart_write((unsigned char*)&ACK_msg, ACK_SIZE);  // Send ACK to download tool
				boot_usdelay(10000);  // Reserve 1ms for downloading tools
				boot_uart_set_baudrate(SOC_UART0_BASE, 115200, 0);  // 切换波特率
				efuse_data_in_agc_memory_refresh();

#if DBGLOG_LEVEL >= DBGLOG_LEVEL_CRITICAL	// 在log级别较高时延迟8秒启动，方便看到全部log
				boot_usdelay(100000);  // Reserve 100ms for baudrate

				for(int k = 4; k > 0; k--)
				{
					DBGLOG_CRITICAL("\nStart uboot after %d seconds", k);
					boot_usdelay(1000000);
				}
				DBGLOG_CRITICAL("\n");
				int one_bits_num = count_one_bits((g_efuse_ctrl & EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK));
				if((g_efuse_ctrl & EFUSE_CTRL_FLASH_ENCRYPT_EN) &&
					(g_efuse_ctrl & EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK) != EFUSE_CTRL_FLASH_ENCRYPT_CNT_MASK &&
					one_bits_num % 2)  // 1、使能了加密 2、计数标识不全为F 3、计数标识中1的个数为奇数
				{
					DBGLOG_CRITICAL("encrypt yes\n");
				}
				else
				{
					DBGLOG_CRITICAL("encrypt no\n");
				}
#if 0
#if DBGLOG_LEVEL >= DBGLOG_LEVEL_DEBUG
				stub_trace[stub_trace_index++] = 'z';
				DBGLOG_DEBUG("stub_trace:");
				for(int i = 0; i < stub_trace_index; i++)
				{
					DBGLOG_DEBUG("%c ", stub_trace[i]);
				}
				DBGLOG_DEBUG("\n");
				DBGLOG_DEBUG("all_nv_start_addr=0x%X\n", all_nv_start_addr);
				DBGLOG_DEBUG("all_nv_end_addr=0x%X\n", all_nv_end_addr);

				unsigned int customer_addr, customer_len;
				unsigned int develop_addr, develop_len;
				unsigned int amt_addr, amt_len;
				boot_partion_info_get(PARTITION_NAME_NV_CUSTOMER, &customer_addr, &customer_len);
				boot_partion_info_get(PARTITION_NAME_NV_DEVELOP, &develop_addr, &develop_len);
				boot_partion_info_get(PARTITION_NAME_NV_AMT, &amt_addr, &amt_len);

				DBGLOG_DEBUG("customer_addr=0x%X\n", customer_addr);
				DBGLOG_DEBUG("customer_len=0x%X\n", customer_len);
				DBGLOG_DEBUG("develop_addr=0x%X\n", develop_addr);
				DBGLOG_DEBUG("develop_len=0x%X\n", develop_len);
				DBGLOG_DEBUG("amt_addr=0x%X\n", amt_addr);
				DBGLOG_DEBUG("amt_len=0x%X\n", amt_len);

				DBGLOG_DEBUG("partition_backup:");
				for(int i = 0; i < 0x800; i++)
				{
					DBGLOG_DEBUG("%c", partition_backup[i]);
				}
				DBGLOG_DEBUG("\n");
#endif
#endif
#endif
//#include "bootrom.h"
#define CPU_CLK_SEC_40M_26M						(0x0 << 0)
#define CPU_CLK_FREQ_DIV0						(0x1 << 2)
#define CHIP_SMU_PD_CLK_UART_TRNG_80M_EN		(1<<2)
#define CHIP_SMU_PD_CLK_DIV3_EN					(1<<1)
#define CHIP_SMU_PD_CLK_DIV2_EN					(1<<0)
#define FLASHBOOT_CHECK_UARTBOOT				1
#define FLASHBOOT_NOT_CHECK_UARTBOOT			0
				extern int flash_boot(int uart_boot_check);
				OUT32(SOC_PD_CLK_DIV_EN, (IN32(SOC_PD_CLK_DIV_EN) | CHIP_SMU_PD_CLK_DIV2_EN | CHIP_SMU_PD_CLK_DIV3_EN) & ~CHIP_SMU_PD_CLK_UART_TRNG_80M_EN);
				OUT32(SOC_PD__CLK_MUX_BASE, (IN32(SOC_PD__CLK_MUX_BASE) & 0xFFFFFFF0) | CPU_CLK_FREQ_DIV0 | CPU_CLK_SEC_40M_26M);
				flash_boot(FLASHBOOT_NOT_CHECK_UARTBOOT);  // bootrom function

				/****************************************************************
				 *    Running to this point, it means uboot failed to start     *
				 ****************************************************************/
				while(1)
				{
					boot_usdelay(2000000);
					uart_write((unsigned char*)"stub err!\n", 18);
				}
#endif

			case STATE_BOUND:  // 0x07
				uboot_buad();
				break;
			case 0x09: // check if implementation is correct with off tools
				uboot_flash_sha256(&cmd_data_buf);
				break;
			case 0x90:
				uboot_flashid();
				break;
			case 0x91:
				uboot_flash_xmodem_dl(&cmd_data_buf);
				break;
			case 0x92:
				uboot_flash_xmodem_ul(false, &cmd_data_buf);
				break;
			case 0x96:
				uboot_flash_xmodem_ul_z(&cmd_data_buf);
				break;
			case 0x97:
				uboot_flash_xmodem_dl_z(&cmd_data_buf);
				break;
			case 0x98:
				uboot_flash_xmodem_ul(true, &cmd_data_buf);
				break;
			case 0x99:
				uboot_read_efuse();
				break;
			
			default:
				ACK_msg.type = STATE_ERR;
				ACK_msg.status = STATUS_TYPE_ERROR;
				ACK_msg.CRC8 = uboot_mesage_check((unsigned char * )&ACK_msg,ACK_SIZE-1);
				uart_write((unsigned char * )&ACK_msg, ACK_SIZE);
				break;
		}
	}while(1);
}

__attribute__((__used__)) void* malloc(size_t size)
{
	// since it's the only allocation, just return raw address
	return (void*)(0x00080000);
}
void* realloc(void* ptr, size_t size)
{
	return NULL;
}
void free(void* ptr)
{

}

void __assert_func(const char* file, int line, const char* func, const char* failedexpr)
{
	while(1);
}
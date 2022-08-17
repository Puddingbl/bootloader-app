
/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __IAP_CONFIG_H
#define __IAP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define  IAP_RUN_IN     1   /* APP在哪运行: 0:ROM; 1:RAM */

#if      IAP_RUN_IN == 1
	#define  IAP_RAM  1
#elif    IAP_RUN_IN == 0
	#define  IAP_ROM  1
#endif

/* includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"

/* user include---------------------------------------------------------------*/
#include "spi_norflash.h"
#include "qspi_norflash.h"
/* define---------------------------------------------------------------------*/
#define  DATA_BUFF_SIZE        147452
#define  IAP_BUFF_ADDR				 0x24040000       //把data_buff定义到哪个地址

/* 装载程序相关flash */
#define  PAGE_SIZE   					 4096						  //装载程序的flash页大小
#define  FLASH_LOAD_ADDR       0x00000000       //想要写入到flash的地址

#define  iap_flash_eraseSector(addr)      						BSP_QSPI_Erase_Block(addr)
#define  iap_flash_write(data, addr, size)						BSP_QSPI_WriteUnfixed(data, addr, size)
#define  iap_flash_read(data, addr, size)							BSP_QSPI_FastRead(data, addr, size)

/* 记录数据相关flash */

/* extern variables-----------------------------------------------------------*/


/* exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif


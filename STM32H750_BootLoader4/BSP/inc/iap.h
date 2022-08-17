/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __IAP_H
#define __IAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "iap_config.h"

/* define---------------------------------------------------------------------*/

#define IAP_HEADER_SIZE		 64


typedef enum{
	APP_NULL = 0,
	APP1,
	APP2,
	APP_CNT
} app_e;


typedef unsigned int 		__be32;
typedef	unsigned char		uint8_t;

#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN	32	/* Image Name Length		*/

typedef struct image_header {
	__be32		ih_magic;	/* Image Header Magic Number	*/
	__be32		ih_hcrc;	/* Image Header CRC Checksum	*/
	__be32		ih_time;	/* Image Creation Timestamp	*/
	__be32		ih_size;	/* Image Data Size		*/
	__be32		ih_load;	/* Data	 Load  Address		*/
	__be32		ih_ep;		/* Entry Point Address		*/
	__be32		ih_dcrc;	/* Image Data CRC Checksum	*/
	uint8_t		ih_os;		/* Operating System		*/
	uint8_t		ih_arch;	/* CPU architecture		*/
	uint8_t		ih_type;	/* Image Type			*/
	uint8_t		ih_comp;	/* Compression Type		*/
	uint8_t		ih_name[IH_NMLEN];	/* Image Name		*/
} image_header_t;

typedef struct bootloader {
	image_header_t  header;
	uint32_t        size;				 //程序大小
	uint32_t        load;        //程序下载地址
	uint32_t        ep;          //程序跳转地址
	app_e           updata_flag; //跳转标志
} bootloader_t;

/* extern variables-----------------------------------------------------------*/
extern uint32_t flash_addr;

/* exported functions ------------------------------------------------------- */
extern void start_app(uint32_t new_vector);
void iap_init(void);

uint32_t iap_get_data_size(void);
uint32_t iap_gei_file_size(void);
uint32_t iap_get_load(void);
app_e iap_get_updata_flag(void);
uint8_t *iap_get_data_buff(void);

void iap_set_header(image_header_t header);
void iap_set_size(uint32_t size);
void iap_set_load(uint32_t load);
void iap_set_updata_flag(app_e flag);
void iap_set_data_buff(uint8_t data, uint32_t pos);
uint8_t iap_revice(uint8_t data);
void iap_jump(app_e app);
#ifdef __cplusplus
}
#endif

#endif

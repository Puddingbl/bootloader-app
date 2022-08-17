/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __PUBLIC_H
#define __PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stdio.h"

/* define---------------------------------------------------------------------*/

/* DEBUG专用------------------------------------------------------*/
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_TRACE(format, ...) printf("Info:[%s:%s(%d)]:" format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)  //同时打印文件名、函数名、行
#define DEBUG_LOG(format, ...) printf(format, ##__VA_ARGS__)																															 //普通printf
#else
#define DEBUG_TRACE(format, ...)
#define DEBUG_LOG(format, arg...)
#endif

//定义枚举类型 -> TRUE/FALSE位
typedef enum 
{
  FALSE = 0U, 
  TRUE = !FALSE
} FlagStatus_t;


//定义枚举类型 -> BIT位
typedef enum
{
	BIT0 = (uint8_t)(0x01 << 0),  
	BIT1 = (uint8_t)(0x01 << 1),  
	BIT2 = (uint8_t)(0x01 << 2),  
	BIT3 = (uint8_t)(0x01 << 3),  
	BIT4 = (uint8_t)(0x01 << 4),
	BIT5 = (uint8_t)(0x01 << 5),
	BIT6 = (uint8_t)(0x01 << 6),
	BIT7 = (uint8_t)(0x01 << 7),
}BIT_t;

typedef enum
{
	STATE_0 = 0,
	STATE_1,
	STATE_2,
	STATE_3,
	STATE_4,
	STATE_5,
	STATE_6,
	STATE_7,
	STATE_8,
	STATE_9,
	STATE_10,
	STATE_11,
	STATE_12	
} parseState_e;

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

/* extern variables-----------------------------------------------------------*/

/* exported functions ------------------------------------------------------- */
void run(void);
unsigned int be32_to_cpu(unsigned int x);
void copy_app(int *from, int *to, int len);
void relocate_and_start_app(unsigned int pos);
extern void start_app(uint32_t new_vector);
#ifdef __cplusplus
}
#endif

#endif

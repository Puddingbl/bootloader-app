/* define to prevent recursive inclusion -------------------------------------*/
#ifndef __PUBLIC_H
#define __PUBLIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include "usart.h"
#include "stdio.h"
#include "iap.h"

/* define---------------------------------------------------------------------*/

/* DEBUGר��------------------------------------------------------*/
#define DEBUG 1
#ifdef DEBUG
#define DEBUG_TRACE(format, ...) printf("Info:[%s:%s(%d)]:" format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)  //ͬʱ��ӡ�ļ���������������
#define DEBUG_LOG(format, ...) printf(format, ##__VA_ARGS__)																															 //��ͨprintf
#else
#define DEBUG_TRACE(format, ...)
#define DEBUG_LOG(format, arg...)
#endif

//����ö������ -> TRUE/FALSEλ
typedef enum 
{
  FALSE = 0U, 
  TRUE = !FALSE
} FlagStatus_t;


//����ö������ -> BITλ
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

/* extern variables-----------------------------------------------------------*/

/* exported functions ------------------------------------------------------- */
void run(void);

void copy_app(int *from, int *to, int len);
void relocate_and_start_app(unsigned int pos);

#ifdef __cplusplus
}
#endif

#endif

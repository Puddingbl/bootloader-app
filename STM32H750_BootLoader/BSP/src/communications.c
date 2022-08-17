#include "communications.h"
#include "main.h"
#include "spi_norflash.h"
#include "stdlib.h"
#include "stdio.h"
/*全局变量声明*/
float id_vaul = 0;
Com_General_Queue_t com_usart1;

/*私有声明*/
static uint16_t usart1_tx_buff[30] = {0};
static uint8_t usart1_rx_buff[256] = {0};
static uint8_t usart1_info[30] = {0};
#define page_size  256
static void usart1_protocol_parse(Queue_t *q, uint8_t buff);

void com_module_init(void)
{
	//串口1模块初始化
	com_usart1.protocol_parse = usart1_protocol_parse;
	queue_init(&com_usart1.rx_buff, usart1_rx_buff);      //使用DMA时是usart1_rxbuff，普通方式时是usart1_rx_buff
	queue_init(&com_usart1.info, usart1_info);
}


/**
	* 通用通信处理
**/
void com_general_handle(Com_General_Queue_t *q)
{
	uint8_t receivelenth = 0;
	QElemType buffer[1];
	receivelenth = queue_length(q->rx_buff);
	if(receivelenth == 0)
		return;
	
	for(; receivelenth>0; receivelenth--)
	{
		queue_extract(&q->rx_buff, buffer);
		q->protocol_parse(&q->info, (uint8_t)buffer[0]);
	}
}


void copy_data(uint8_t *from, uint8_t *to, uint32_t len)
{
	// 从哪里到哪里, 多长 ?
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		to[i] = from[i];
	}
}


unsigned int load;
unsigned int size;
static uint8_t updata_flag = 0;
static uint8_t updata_headbuff[64] = {0};
static uint8_t updata_databuff[256] = {0};
static uint8_t updata_databuff1[300] = {0};
static uint32_t updata_cnt = 0;
static uint32_t updata_cnt1 = 0;
static uint32_t updata_cnt2 = 0;
static uint32_t updata_cnt3 = 0;

//串口1协议解析
static void usart1_protocol_parse(Queue_t *q, uint8_t buff)
{
	static parseState_e uartcmdstate = STATE_0;
	static uint8_t duty;	
	
	
	/* 解析接收的updata文件头部信息，然后跳转到2、3执行 */
	if(updata_flag == 1)							
	{
		updata_headbuff[updata_cnt++] = buff;
		if(updata_cnt == 64)
		{
//			uint32_t *updata_tast = (uint32_t*)0x24000000;
			SPI_Flash_WritePage(updata_headbuff, 0x00000000, 64);
//			SPI_Flash_ReadUnfixed((uint8_t*)updata_tast, 0x00000000, 64);

//			image_header_t *head = (image_header_t*)updata_tast;
			image_header_t *head = (image_header_t*)updata_headbuff;			
			/* 解析头部 */
			load = be32_to_cpu(head->ih_load);
			size = be32_to_cpu(head->ih_size);			
			printf("load = %#x\n", load);
			printf("size = %#x\n", size);
			printf("......\n");
			
			updata_cnt1 = size/256;
			updata_cnt2 = size%256;
			printf("updata_cnt1 = %#x\n", updata_cnt1);
			printf("updata_cnt2 = %#x\n", updata_cnt2);
		
			updata_cnt = 0;
			updata_flag = 2;
		}
	}
	/* 每次缓存256字节，然后写入，写入updata_cnt1次 */
	else if(updata_flag == 2)
	{	
		updata_databuff1[updata_cnt++] = buff;
		if(updata_cnt == 256)
		{
			SPI_Flash_WritePage(updata_databuff, 0x00000000 + page_size*updata_cnt3 + 64, 256);
			updata_cnt3++;
			if(updata_cnt3 == updata_cnt1)
			{				
				updata_flag = 3;
			}
			updata_cnt = 0;
		}
	}
	/* 写入剩余的不够256字节的数据，然后跳回正常串口接收状态 */
	else if(updata_flag == 3)
	{
		updata_databuff[updata_cnt++] = buff;
		if(updata_cnt == updata_cnt2)
		{
			SPI_Flash_WritePage(updata_databuff, 0x00000000 + page_size*updata_cnt3 + 64, updata_cnt2);
			printf("接收完成\n");
			updata_flag = 0;
			updata_cnt3 = 0;
			updata_cnt = 0;
		}
	}	
	else
	{
		switch(uartcmdstate)
		{
			case STATE_0:
			{
				if(buff == 'a')
				{
					queue_clear(q);
					
					queue_insert(q, buff);
					uartcmdstate = STATE_1;
					printf("第一步\r\n");
				}
				else
				{
					uartcmdstate = STATE_0;
					printf("error\r\n");				
				}
			}
			break;
			
			case STATE_1:
			{
				if(buff == 'b')
				{
					queue_insert(q, buff);
					
					uartcmdstate = STATE_2;
					printf("第二步\r\n");
				}
				else
					uartcmdstate = STATE_0;			
			}
			break;

			case STATE_2:
			{
				if(buff == 'c')
				{
					queue_insert(q, buff);	
					printf("第三步\r\n");
					printf("等待擦除\n");
					updata_flag = 1;
					for(uint16_t i=0; i<256; i++)
				  {
						SPI_Flash_EraseSector(0x00000000 + page_size*i);
				  }
					printf("擦除完成\n");
					uartcmdstate = STATE_0;
				}
				else if(buff == 'd')
				{	
//					for(uint16_t j=0; j<updata_cnt1; j++)
//					{
//						for(uint16_t i=0; i<256; i++)
//						{
//							SPI_Flash_ReadUnfixed((uint8_t*)((0x24000000+j*256)+i), (0x00000000+j*256)+i, 1);
//						}						
//					}
//					for(uint16_t k=0; k<updata_cnt2; k++)
//					{
//						SPI_Flash_ReadUnfixed((uint8_t*)((0x24000000+updata_cnt1*256)+k), (0x00000000+updata_cnt1*256)+k, 1);
//					}	
					uint32_t *data = (uint32_t*)0x24000000;
					SPI_Flash_ReadUnfixed((uint8_t*)data, 0x00000000, 64);
					image_header_t *head = (image_header_t*)data;

					load = be32_to_cpu(head->ih_load);
					size = be32_to_cpu(head->ih_size);			
					printf("load = %#x\n", load);
					printf("size = %#x\n", size);
					printf("......\n");
					
					updata_cnt1 = size/256;
					updata_cnt2 = size%256;
					printf("updata_cnt1 = %#x\n", updata_cnt1);
					printf("updata_cnt2 = %#x\n", updata_cnt2);
					
					printf("*data=%#x\n", *(uint32_t*)data);
					printf("复制完成");
//					start_app(0x24000000);
					updata_flag = 0;
					uartcmdstate = STATE_0;
				}
				else if(buff == 'e')
				{
					uartcmdstate = STATE_0;
				}
				else if(buff == 'f')
				{
					uartcmdstate = STATE_0;
				}
				else if(buff == 'g')
				{
					uartcmdstate = STATE_0;
				}
				else if(buff == 'h')
				{
					uartcmdstate = STATE_0;				
				}
				else if(buff == 'i')
				{			
					uartcmdstate = STATE_0;	
				}
				else if(buff == 'j')
				{			
					uartcmdstate = STATE_0;	
				}			
				else
					uartcmdstate = STATE_0;						
			}
			break;

			case STATE_3:
			{
				printf("我\r\n");
				uartcmdstate = STATE_0;
			}
			break;

			default:uartcmdstate = STATE_0;break;
		}		
	}

}

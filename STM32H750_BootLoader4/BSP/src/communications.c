#include "communications.h"
#include "main.h"
#include "spi_norflash.h"
#include "iap.h"

/*ȫ�ֱ�������*/
float id_vaul = 0;
Com_General_Queue_t com_usart1;

/*˽������*/
//static uint16_t usart1_tx_buff[30] = {0};
static uint8_t usart1_rx_buff[256] = {0};
static uint8_t usart1_info[30] = {0};
#define page_size  256
static void usart1_protocol_parse(Queue_t *q, uint8_t buff);

void com_module_init(void)
{
	//����1ģ���ʼ��
	com_usart1.protocol_parse = usart1_protocol_parse;
	queue_init(&com_usart1.rx_buff, usart1_rx_buff);      //ʹ��DMAʱ��usart1_rxbuff����ͨ��ʽʱ��usart1_rx_buff
	queue_init(&com_usart1.info, usart1_info);
}


/**
	* ͨ��ͨ�Ŵ���
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
	// �����ﵽ����, �೤ ?
	uint32_t i;
	for (i = 0; i < len; i++)
	{
		to[i] = from[i];
	}
}

//����1Э�����
static void usart1_protocol_parse(Queue_t *q, uint8_t buff)
{
	static parseState_e uartcmdstate = STATE_0;

	switch(uartcmdstate)
	{
		case STATE_0:
		{
			if(buff == 'a')
			{
				queue_clear(q);
				
				queue_insert(q, buff);
				uartcmdstate = STATE_1;
				printf("��һ��\r\n");
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
				printf("�ڶ���\r\n");
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
				printf("������\r\n");
				SPI_Flash_EraseSector(0);
				flash_addr = 0;
				printf("�ȴ�����\n");
				uartcmdstate = STATE_3;
			}
			else if(buff == 'd')
			{	
				printf("������\r\n");
				SPI_Flash_EraseSector(4096);
				flash_addr = 4096;
				printf("�ȴ�����\n");
				uartcmdstate = STATE_3;
			}
			else if(buff == 'e')
			{
				iap_set_updata_flag(APP1);
				uartcmdstate = STATE_0;
			}		
			else if(buff == 'f')
			{
				iap_set_updata_flag(APP2);
				uartcmdstate = STATE_0;
			}				
		}
		break;

		case STATE_3:
		{
			if(iap_revice(buff))
			{
				uartcmdstate = STATE_0;
			}	
		}
		break;

		default:uartcmdstate = STATE_0;break;
	}		
}

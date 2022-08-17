
#include "public.h"
#include "spi_norflash.h"
#include "communications.h"
#include "string.h"


void spiFlashTest(void);
void qspiFlashTest(void);

/**
	* @name   fputc
	* @brief  fputcӳ��������
	* @param  ch -> �������ַ�
	* @retval ch -> �ѷ����ַ�      
*/

int fputc(int ch,FILE *p) 
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);
  return ch;
}


/*************************************���Ƿָ���*********************************************/

uint8_t updata_buff = 0;

void run()
{
	__enable_irq();

//	qspiFlashTest();
//	spiFlashTest();		
	iap_init();
	com_module_init();
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //�򿪴����жϣ����ѽ��յ���һ�����ݷŵ�updata_buff
	printf(".........................\n");
	
	while(1)
	{
		com_general_handle(&com_usart1);
		
		iap_jump(iap_get_updata_flag());
	}
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	printf("updata_buff=%d\n",updata_buff);
	queue_insert(&com_usart1.rx_buff,(uint8_t)updata_buff);
	//�������ڽ���(�ж�ģʽ��1���ֽ�)
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //���һ���жϽ��պ���Զ��ر��жϣ�������Ҫ�ٵ��ô˺������ж�
}


void spiFlashTest()
{
	uint8_t i;
	uint8_t CMP_Flag = TRUE;
	//�����д����
	uint8_t Tx_Buffer[] = "Ӳ����԰ - ��Ƭ����Ʒʵս��Ŀ��ѧ,΢�Ź��ںţ�yjjy168168168";
	const uint8_t BufferSize  = sizeof(Tx_Buffer)/sizeof(Tx_Buffer[0]);
	uint8_t Rx_Buffer[BufferSize];
	
	SPI_Flash.ReadJedecID();  //��ȡFlashоƬ���豸ID	
	/********FlashоƬ��д����********/
	//��������
	SPI_Flash.EraseSector(0x00000000);
	//д�벻����������
	SPI_Flash.WriteUnfixed(Tx_Buffer,0x00000088,BufferSize);
	printf("д�������Ϊ��%s\r\n", Tx_Buffer);
	//��������������
	SPI_Flash.ReadUnfixed(Rx_Buffer,0x000000088,BufferSize);
	printf("����������Ϊ��%s\r\n", Rx_Buffer);
	//�Ƚϻ�������
	for(i=0;i<BufferSize;i++)
	{
		if(Tx_Buffer[i] != Rx_Buffer[i])
		{
			CMP_Flag = FALSE;
			break;
		}
	}
	//��ӡ�ȽϽ��
	if(CMP_Flag == TRUE)
		printf("��ϲ��FalshоƬ��д���Գɹ���\r\n\r\n\r\n");
	else
		printf("What��FalshоƬ��д����ʧ�ܣ�\r\n\r\n\r\n");		
	
	HAL_Delay(1000);
}

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

// ����ԭ������
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

void qspiFlashTest()
{
	__IO TestStatus TransferStatus1 = FAILED;	
  __IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);	
	
  uint32_t addr = 0x00000000 ;
	
	uint8_t state = QSPI_ERROR;
	uint8_t Tx_Buffer[] = "cxnvVNŷ��Ů���ǲ��ܱ���\n";
	const uint16_t BufferSize  = sizeof(Tx_Buffer)/sizeof(Tx_Buffer[0]);
	uint8_t Rx_Buffer[BufferSize];		
	
//	/* ʹ��ָ��� */
//	SCB_EnableICache();
//	/* ʹ�����ݻ��� */
//	SCB_EnableDCache();
  
	/* 32M����flash W25Q256��ʼ�� */
	BSP_QSPI_Init();
	
	
	state = BSP_QSPI_Erase_Block(addr);
	
	if(state == QSPI_OK)
		printf("\r\n�����ɹ�!\r\n");
	else
	{
		printf("\r\n����ʧ��!\r\n");
		while(1);     
	}

	BSP_QSPI_WriteUnfixed(Tx_Buffer, addr, BufferSize);	
	printf("д�������Ϊ��%s\r\n", Tx_Buffer);
	
	/* ���ո�д������ݶ������ŵ����ջ������� */
	BSP_QSPI_FastRead(Rx_Buffer, addr, BufferSize);
	printf("����������Ϊ��%s\r\n", Rx_Buffer);  
	
	/* ���д�������������������Ƿ���� */
	TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
	
	if( PASSED == TransferStatus1 )
	{    
		printf("\r\n��д(memcpy)���Գɹ�!\n\r");
	}
	else
	{        
		printf("\r\n��д(memcpy)����ʧ��!\n");
	}

	if (QSPI_ResetMemory() != QSPI_OK)
	{
		
	} 
	if( QSPI_EnableMemoryMappedMode() != QSPI_OK )
	{
		printf("�����ڴ�ģʽʧ��\n");
	}	
	
	memset(Rx_Buffer,0,BufferSize);
	
	for(int i = 0; i < BufferSize; i++)
	{ 
		Rx_Buffer[i] = *qspi_addr;
		qspi_addr++;
	}	

	printf("����������Ϊ��%s\r\n", Rx_Buffer);  
	/* ���д�������������������Ƿ���� */
	TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);	

	if( PASSED == TransferStatus1 )
	{    
		printf("\r\n��д(ָ�����)���Գɹ�!\n\r");
	}
	else
	{        
		printf("\r\n��д(ָ�����)����ʧ��!\n\r");
	}  	
}

/*
 * ��������Buffercmp
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-PASSED pBuffer1 ����   pBuffer2
 *         -FAILED pBuffer1 ��ͬ�� pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}


/**
*/

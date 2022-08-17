
#include "public.h"
#include "spi_norflash.h"
#include "communications.h"
#include "string.h"


void spiFlashTest(void);
void qspiFlashTest(void);

/**
	* @name   fputc
	* @brief  fputc映射物理串口
	* @param  ch -> 待发送字符
	* @retval ch -> 已发送字符      
*/

int fputc(int ch,FILE *p) 
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);
  return ch;
}


/*************************************我是分割线*********************************************/

uint8_t updata_buff = 0;

void run()
{
	__enable_irq();

//	qspiFlashTest();
//	spiFlashTest();		
	iap_init();
	com_module_init();
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //打开串口中断，并把接收到的一个数据放到updata_buff
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
	//继续串口接收(中断模式，1个字节)
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //完成一次中断接收后悔自动关闭中断，所以需要再调用此函数打开中断
}


void spiFlashTest()
{
	uint8_t i;
	uint8_t CMP_Flag = TRUE;
	//定义读写缓存
	uint8_t Tx_Buffer[] = "硬件家园 - 单片机产品实战项目教学,微信公众号：yjjy168168168";
	const uint8_t BufferSize  = sizeof(Tx_Buffer)/sizeof(Tx_Buffer[0]);
	uint8_t Rx_Buffer[BufferSize];
	
	SPI_Flash.ReadJedecID();  //读取Flash芯片的设备ID	
	/********Flash芯片读写测试********/
	//扇区擦除
	SPI_Flash.EraseSector(0x00000000);
	//写入不定长度数据
	SPI_Flash.WriteUnfixed(Tx_Buffer,0x00000088,BufferSize);
	printf("写入的数据为：%s\r\n", Tx_Buffer);
	//读出不定长数据
	SPI_Flash.ReadUnfixed(Rx_Buffer,0x000000088,BufferSize);
	printf("读出的数据为：%s\r\n", Rx_Buffer);
	//比较缓存数据
	for(i=0;i<BufferSize;i++)
	{
		if(Tx_Buffer[i] != Rx_Buffer[i])
		{
			CMP_Flag = FALSE;
			break;
		}
	}
	//打印比较结果
	if(CMP_Flag == TRUE)
		printf("恭喜，Falsh芯片读写测试成功！\r\n\r\n\r\n");
	else
		printf("What？Falsh芯片读写测试失败！\r\n\r\n\r\n");		
	
	HAL_Delay(1000);
}

typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

// 函数原型声明
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);

void qspiFlashTest()
{
	__IO TestStatus TransferStatus1 = FAILED;	
  __IO uint8_t* qspi_addr = (__IO uint8_t*)(0x90000000);	
	
  uint32_t addr = 0x00000000 ;
	
	uint8_t state = QSPI_ERROR;
	uint8_t Tx_Buffer[] = "cxnvVN欧文女明星才能比我\n";
	const uint16_t BufferSize  = sizeof(Tx_Buffer)/sizeof(Tx_Buffer[0]);
	uint8_t Rx_Buffer[BufferSize];		
	
//	/* 使能指令缓存 */
//	SCB_EnableICache();
//	/* 使能数据缓存 */
//	SCB_EnableDCache();
  
	/* 32M串行flash W25Q256初始化 */
	BSP_QSPI_Init();
	
	
	state = BSP_QSPI_Erase_Block(addr);
	
	if(state == QSPI_OK)
		printf("\r\n擦除成功!\r\n");
	else
	{
		printf("\r\n擦除失败!\r\n");
		while(1);     
	}

	BSP_QSPI_WriteUnfixed(Tx_Buffer, addr, BufferSize);	
	printf("写入的数据为：%s\r\n", Tx_Buffer);
	
	/* 将刚刚写入的数据读出来放到接收缓冲区中 */
	BSP_QSPI_FastRead(Rx_Buffer, addr, BufferSize);
	printf("读出的数据为：%s\r\n", Rx_Buffer);  
	
	/* 检查写入的数据与读出的数据是否相等 */
	TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
	
	if( PASSED == TransferStatus1 )
	{    
		printf("\r\n读写(memcpy)测试成功!\n\r");
	}
	else
	{        
		printf("\r\n读写(memcpy)测试失败!\n");
	}

	if (QSPI_ResetMemory() != QSPI_OK)
	{
		
	} 
	if( QSPI_EnableMemoryMappedMode() != QSPI_OK )
	{
		printf("进入内存模式失败\n");
	}	
	
	memset(Rx_Buffer,0,BufferSize);
	
	for(int i = 0; i < BufferSize; i++)
	{ 
		Rx_Buffer[i] = *qspi_addr;
		qspi_addr++;
	}	

	printf("读出的数据为：%s\r\n", Rx_Buffer);  
	/* 检查写入的数据与读出的数据是否相等 */
	TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);	

	if( PASSED == TransferStatus1 )
	{    
		printf("\r\n读写(指针操作)测试成功!\n\r");
	}
	else
	{        
		printf("\r\n读写(指针操作)测试失败!\n\r");
	}  	
}

/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
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

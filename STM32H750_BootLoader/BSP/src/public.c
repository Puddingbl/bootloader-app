
#include "public.h"
#include "spi_norflash.h"
#include "communications.h"

void spiFlashTest();

/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
 #endif
#endif

#if defined (__GNUC__) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

	
/**
	* @name   fputc
	* @brief  fputc映射物理串口
	* @param  ch -> 待发送字符
	* @retval ch -> 已发送字符      
*/
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);
  return ch;
}


/*************************************我是分割线*********************************************/

#define select_way  3

unsigned int be32_to_cpu(unsigned int x)
{
	unsigned char *p = (unsigned char *)&x;
	unsigned int le;
	le = (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + (p[3]);
	return le;
}

void copy_app(int *from, int *to, int len)
{
	// 从哪里到哪里, 多长 ?
	int i;
	for (i = 0; i < len/4+1; i++)
	{
		to[i] = from[i];
	}
}

void relocate_and_start_app(unsigned int pos)
{
	image_header_t *head;
	unsigned int load;
	unsigned int size;
	unsigned int new_pos = pos+sizeof(image_header_t);
	
//	/* 读出头部 */
//	head = (image_header_t *)pos;
//	
//	/* 解析头部 */
//	load = be32_to_cpu(head->ih_load);
//	size = be32_to_cpu(head->ih_size);
//	
//	printf("load = %#x\n", load);
//	printf("size = %#x\n", size);
//	printf("......\n");
//	/* 把程序复制到RAM */
//	copy_app((int *)new_pos, (int *)load, size);
	
	/* 跳转执行 */
	start_app(pos);
}

void MSR_MSP(uint32_t addr)
{
__ASM volatile("MSR MSP, r0"); 
__ASM volatile("BX r14");
}

uint8_t updata_buff = 0;

void run()
{
#if  select_way == 1
	uint32_t *p = (uint32_t *)0x90000004;    //取08040004处的地址
	uint32_t val = *p;											 //取08040004处的值，该处存放了resethandle函数地址

	void (*app)(void);

	printf("bootloder\n");
	printf("val=%d\n",val);

	app = (void (*)(void))val;							//把resethandle函数地址传给app
	MSR_MSP(val);
	app();
#elif select_way == 2
	uint32_t new_vector = 0x90000000;
	
	printf("bootloder\n");
	
	start_app(new_vector);
#elif	select_way == 3		
	com_module_init();
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //打开串口中断，并把接收到的一个数据放到updata_buff
	
	spiFlashTest();
	
//		uint32_t *data = (uint32_t*)0x90000000;
//		printf("*data=%#x\n", *data);
//		printf("复制完成");	
	
	while(1)
	{
		com_general_handle(&com_usart1);
	}
		
//	unsigned int app_pos = 0x90000000;	
	
	
//	relocate_and_start_app(app_pos);
	
#endif
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

/**
*/
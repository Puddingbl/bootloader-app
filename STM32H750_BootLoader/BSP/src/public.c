
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
	* @brief  fputcӳ��������
	* @param  ch -> �������ַ�
	* @retval ch -> �ѷ����ַ�      
*/
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0x000A);
  return ch;
}


/*************************************���Ƿָ���*********************************************/

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
	// �����ﵽ����, �೤ ?
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
	
//	/* ����ͷ�� */
//	head = (image_header_t *)pos;
//	
//	/* ����ͷ�� */
//	load = be32_to_cpu(head->ih_load);
//	size = be32_to_cpu(head->ih_size);
//	
//	printf("load = %#x\n", load);
//	printf("size = %#x\n", size);
//	printf("......\n");
//	/* �ѳ����Ƶ�RAM */
//	copy_app((int *)new_pos, (int *)load, size);
	
	/* ��תִ�� */
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
	uint32_t *p = (uint32_t *)0x90000004;    //ȡ08040004���ĵ�ַ
	uint32_t val = *p;											 //ȡ08040004����ֵ���ô������resethandle������ַ

	void (*app)(void);

	printf("bootloder\n");
	printf("val=%d\n",val);

	app = (void (*)(void))val;							//��resethandle������ַ����app
	MSR_MSP(val);
	app();
#elif select_way == 2
	uint32_t new_vector = 0x90000000;
	
	printf("bootloder\n");
	
	start_app(new_vector);
#elif	select_way == 3		
	com_module_init();
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&updata_buff,1);  //�򿪴����жϣ����ѽ��յ���һ�����ݷŵ�updata_buff
	
	spiFlashTest();
	
//		uint32_t *data = (uint32_t*)0x90000000;
//		printf("*data=%#x\n", *data);
//		printf("�������");	
	
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

/**
*/
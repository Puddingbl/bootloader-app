
#include "public.h"
#include "tim.h"

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


void run()
{
	HAL_TIM_Base_Start_IT(&htim6); //������ʱ��6
	while(1)
	{
		printf("APP\n");
		HAL_Delay(500);		
	}

}

uint16_t cont1 = 0;
/*
	* @name   HAL_TIM_PeriodElapsedCallback
	* @brief  ��ʱ���жϻص�����
	* @param  *htim -> ����ʱ���Ľṹ��ָ��
	* @retval None    
	* @ ElapsedCallback�Ǵ����ص��������
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim6.Instance)
	{		
		cont1++;
		if(cont1 == 500)
		{
			printf("test\n");
			cont1 = 0;
		}
	}
}

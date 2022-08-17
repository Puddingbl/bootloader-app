#ifndef __KEY_CONFIG_H__
#define __KEY_CONFIG_H__

#include "main.h"

#define KEY_DEBUG 1 


#define key_port    													 GPIO_TypeDef                       //�ڼ�������
#define key_pin 															 uint16_t														//�ڼ�������
#define key_readInput(GPIOx ,GPIO_Pin)  			 HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)  //������״̬


/* ����ִ��key_tick_inc��ʱ�� */
#define KEY_TIME_BASE     1   //ms  

/* ��������  */
#define KEY_NUMBER    	  2   

#endif

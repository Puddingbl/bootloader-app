#ifndef __KEY_CONFIG_H__
#define __KEY_CONFIG_H__

#include "main.h"

#define KEY_DEBUG 1 


#define key_port    													 GPIO_TypeDef                       //第几组引脚
#define key_pin 															 uint16_t														//第几个引脚
#define key_readInput(GPIOx ,GPIO_Pin)  			 HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)  //读引脚状态


/* 定期执行key_tick_inc的时间 */
#define KEY_TIME_BASE     1   //ms  

/* 按键数量  */
#define KEY_NUMBER    	  2   

#endif

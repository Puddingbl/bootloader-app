#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"
#include "public.h"
#include "key_config.h"
#include "staticLinkList.h"


#ifdef KEY_DEBUG
	#define KEY_TRACE(format, ...)  DEBUG_TRACE(format, __VA_ARGS__)  //同时打印文件名、函数名、行
	#define KEY_LOG(format, ...)    DEBUG_LOG(format, ##__VA_ARGS__)		//普通printf
	#else
	#define KEY_TRACE(format, ...)
	#define KEY_LOG(format, arg...)
#endif



// 按键事件
typedef enum
{
	STATELESS = 0,
	PRESS,
	BOUNCE,
	CLICK,
	LONG_PRESS,
	DOUBLE_CLICK,
} key_event;

// 按键事件
typedef enum
{
	KEY_LOW = 0,
	KEY_HIGH,
} key_eleclevel;

typedef struct  key_t
{	
	uint8_t key_id;                 //用于调试
	
	uint8_t key_buffer;             //用于消抖
	uint8_t debounce_time;
	uint8_t key_eleclevel;					//按键按下为高电平还是低电平
	uint8_t key_temp;    				 		//用于消抖
	
	uint32_t double_dlick_time; 		//判断双击开始计时的时间
	uint32_t double_dlick_timer;  	//双击计数器
	uint16_t double_dlick_target;		//双击计数目标值，需根据定时器自己定初值	
	
	uint32_t long_press_time;  			//判断长按开始计时的时间
	uint32_t long_press_timer;      //2S长按计数器
	uint16_t long_press_target;     //2S计数目标值，需根据定时器自己定初值
	
  uint8_t click_flag;          //用于判断按下弹起
  uint8_t bounce_falg;				 //判断弹起标志位
	
	uint8_t  press;              //按下
	uint8_t  bounce;						 //弹起
	uint8_t  click       ;			 //单击、按下弹起
	uint8_t  long_press;         //长按
	uint8_t  double_click;       //双击
	
	key_event  event;						 //按键状态，供应用层调用
	
	uint16_t gpio_pin;					 //GPIO端口		
	uint16_t *gpiox;              //GPIO组
}key_t;

void link_init(void); 
void key_init(key_t *key_num, key_port *gpiox, key_pin gpio_pin, uint8_t id);  //增加新按键调用此函数即可
void key_debounce_scan(void);
void key_detect_scan(void);
void key_tick_inc(uint32_t tick_period);

void key_set_eleclevel(key_t *key_num, key_eleclevel level);
void key_set_ddt(key_t *key_num, uint16_t t);
void key_set_lpt(key_t *key_num, uint16_t t);

key_event key_get_event(key_t *key_num);

/*自行创建按键*/
extern key_t   keya;
extern key_t   keyb;


#endif

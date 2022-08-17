#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"
#include "public.h"
#include "key_config.h"
#include "staticLinkList.h"


#ifdef KEY_DEBUG
	#define KEY_TRACE(format, ...)  DEBUG_TRACE(format, __VA_ARGS__)  //ͬʱ��ӡ�ļ���������������
	#define KEY_LOG(format, ...)    DEBUG_LOG(format, ##__VA_ARGS__)		//��ͨprintf
	#else
	#define KEY_TRACE(format, ...)
	#define KEY_LOG(format, arg...)
#endif



// �����¼�
typedef enum
{
	STATELESS = 0,
	PRESS,
	BOUNCE,
	CLICK,
	LONG_PRESS,
	DOUBLE_CLICK,
} key_event;

// �����¼�
typedef enum
{
	KEY_LOW = 0,
	KEY_HIGH,
} key_eleclevel;

typedef struct  key_t
{	
	uint8_t key_id;                 //���ڵ���
	
	uint8_t key_buffer;             //��������
	uint8_t debounce_time;
	uint8_t key_eleclevel;					//��������Ϊ�ߵ�ƽ���ǵ͵�ƽ
	uint8_t key_temp;    				 		//��������
	
	uint32_t double_dlick_time; 		//�ж�˫����ʼ��ʱ��ʱ��
	uint32_t double_dlick_timer;  	//˫��������
	uint16_t double_dlick_target;		//˫������Ŀ��ֵ������ݶ�ʱ���Լ�����ֵ	
	
	uint32_t long_press_time;  			//�жϳ�����ʼ��ʱ��ʱ��
	uint32_t long_press_timer;      //2S����������
	uint16_t long_press_target;     //2S����Ŀ��ֵ������ݶ�ʱ���Լ�����ֵ
	
  uint8_t click_flag;          //�����жϰ��µ���
  uint8_t bounce_falg;				 //�жϵ����־λ
	
	uint8_t  press;              //����
	uint8_t  bounce;						 //����
	uint8_t  click       ;			 //���������µ���
	uint8_t  long_press;         //����
	uint8_t  double_click;       //˫��
	
	key_event  event;						 //����״̬����Ӧ�ò����
	
	uint16_t gpio_pin;					 //GPIO�˿�		
	uint16_t *gpiox;              //GPIO��
}key_t;

void link_init(void); 
void key_init(key_t *key_num, key_port *gpiox, key_pin gpio_pin, uint8_t id);  //�����°������ô˺�������
void key_debounce_scan(void);
void key_detect_scan(void);
void key_tick_inc(uint32_t tick_period);

void key_set_eleclevel(key_t *key_num, key_eleclevel level);
void key_set_ddt(key_t *key_num, uint16_t t);
void key_set_lpt(key_t *key_num, uint16_t t);

key_event key_get_event(key_t *key_num);

/*���д�������*/
extern key_t   keya;
extern key_t   keyb;


#endif

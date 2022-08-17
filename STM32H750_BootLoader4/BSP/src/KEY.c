/**
	* �ļ�����key.c
	* ��������������
	* 
	* ʱ�䣺2022-8-8
**/

#include "key.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


/* Private functions----------------------------------------------------------*/
static uint32_t key_tick_get(void);
static uint32_t key_tick_elaps(uint32_t prev_tick);
static void key_detect(key_t *key_num);
static void key_debounce(key_t *key_num);

/* Private variables----------------------------------------------------------*/
static uint32_t key_sys_time = 0;
static volatile uint8_t key_tick_irq_flag;

static sLink_t 		  key_sll[KEY_NUMBER+2];
static staticll_t   key_link;

/* Public variables-----------------------------------------------------------*/
key_t   keya; 
key_t   keyb;


/**
	* @name   link_init
	* @brief  �����ʼ������
	* @param  NULL
	* @retval None      
*/
void link_init()
{
	slink_init(&key_link, key_sll, KEY_NUMBER+2);
}

/**
	* @name   Key_Init
	* @brief  ������ʼ����������������
	* @param  key_num����Ҫ�����İ����Ľṹ��
						gpiox/gpio_pin��������Ӧ��GPIO��
            ID:�����ã�ע�͵��ṹ���Ա����ú����ǿ������
	* @retval None      
*/
void key_init(key_t *key_num, key_port *gpiox, key_pin gpio_pin, uint8_t id)
{
	key_num->gpiox = (uint16_t*)gpiox;
	key_num->gpio_pin = gpio_pin;
	
	key_num->key_id = id;
	key_num->key_buffer = 0;	
	key_num->key_eleclevel = 0x00;
	key_num->key_temp = 0;	
	key_num->double_dlick_timer = 0;      //
	key_num->double_dlick_target = 500/KEY_TIME_BASE;    //
	key_num->long_press_timer = 0;
	key_num->long_press_target = 1000/KEY_TIME_BASE;       //��ʱ��1ms
	key_num->click_flag = 0;
	key_num->bounce_falg = FALSE;
	key_num->press = 0;
	key_num->bounce = 0;
	key_num->click = 0;
	key_num->long_press = 0;
	key_num->double_click = 0;
	key_num->event = STATELESS;
	
	slink_insert(&key_link, id, (uint16_t*)key_num);
}

/**
	*���ð�����Ч��ƽ
	*���� level:HIGH/LOW
*/
void key_set_eleclevel(key_t *key_num, key_eleclevel level)
{
	key_num->key_eleclevel = level;
}

/**
	*����˫�����ʱ��
*/
void key_set_ddt(key_t *key_num, uint16_t t)
{
	key_num->double_dlick_target = t;
}

/**
	*���ó���ʱ��
*/
void key_set_lpt(key_t *key_num, uint16_t t)
{
	key_num->long_press_target = t;
}

/*
	* @name   Key_De_shake_scan
	* @brief  �������������Żص���������ʱ���жϣ���
	* @param  NULL
	* @retval None      
*/
void key_debounce_scan()
{
	uint16_t i, length;

	length = slink_length(&key_link);
	
	for(i=0; i<length; i++)
	{	
		key_debounce((key_t*)slink_getData(&key_link, i+1));	
	}
}

/*
	* @name   Key_Detect_scan
	* @brief  �����¼����ɨ�躯��
	* @param  NULL
	* @retval None      
*/
void key_detect_scan() 
{
	uint16_t i, length;

	length = slink_length(&key_link);
	
	for(i=0; i<length; i++)
	{	
		key_detect((key_t*)slink_getData(&key_link, i+1));	
	}	
}

/**
	* @name   key_debounce
	* @brief  ��������ʵ�ֺ���
	* @param  key_num�������ṹ��
	* @retval None      
*/
static void key_debounce(key_t *key_num)
{	

	key_num->key_buffer = (key_num->key_buffer<<1) | key_readInput((key_port *)key_num->gpiox, key_num->gpio_pin); //����ֵ

	if(key_num->key_eleclevel == KEY_HIGH)
	{
		if((key_num->key_buffer & 0xFF) == 0xFF) //��8�ξ�Ϊ����
		{
			key_num->key_temp = 1;
		}
		else if((key_num->key_buffer & 0xFF) == 0x00) //��8�ξ�Ϊ����
		{
			key_num->key_temp = 0;
		}	
	}
	else if(key_num->key_eleclevel == KEY_LOW)
	{
		if((key_num->key_buffer & 0xFF) == 0x00) //��8�ξ�Ϊ����
		{
			key_num->key_temp = 1;
		}
		else if((key_num->key_buffer & 0xFF) == 0xFF) //��8�ξ�Ϊ����
		{
			key_num->key_temp = 0;
		}		
	}	
}



/**
	* @name   Key_Detect
	* @brief  �����¼����ʵ�ֺ���
	* @param  key_num�������ṹ��
	* @retval None      
*/
static void key_detect(key_t *key_num)    
{	
	if(key_num == NULL)
	{
		KEY_LOG("����!\n");
//		System.Err_Handle();
	}
	key_num->press = (key_num->key_temp & (key_num->key_temp ^ key_num->long_press));
	key_num->bounce = key_num->key_temp;
	key_num->long_press = key_num->key_temp;
	
	/* �жϰ��� */
	if(key_num->press == TRUE)
	{
		
		key_num->click+=1;         //�����жϰ��µ���
		key_num->click_flag = 1;
		
		key_num->bounce_falg = TRUE;  //�����жϵ���
		
		key_num->event = PRESS;  //����
		KEY_LOG("key%d����\r\n",key_num->key_id);

		key_num->long_press_time = key_tick_get();
	}	
	
	/* �жϵ��� */
	else if(key_num->bounce == FALSE)
	{	
		if(key_num->bounce_falg == TRUE)
		{
			key_num->bounce_falg = FALSE;
			key_num->event = BOUNCE;  //����
			KEY_LOG("key%d����\r\n",key_num->key_id);
		}
		else
		{
			key_num->event = STATELESS;
		}
		
		if(key_num->click_flag == 1)  //�����жϰ��µ���
		{
			key_num->click_flag = 0;
			key_num->click+=1;
			
		}
	}
	
	/* �жϰ��µ��� */
	if(key_num->click == 2)      
	{
		key_num->click = 0;
		key_num->event = CLICK;	
		KEY_LOG("key%d���µ���\r\n",key_num->key_id);		
		
		key_num->double_click+=1;  //�����ж�˫��
		if(key_num->double_click == 1)
		{
			key_num->double_dlick_time = key_tick_get();		
		}
	}
	
	/* �ж�˫�� */
	if(key_num->double_click == 2)     
	{			
		key_num->double_dlick_timer = key_tick_elaps(key_num->double_dlick_time);
		
		if(key_num->double_dlick_timer <= key_num->double_dlick_target)
		{
			KEY_LOG("˫��Time=%d\r\n",key_num->double_dlick_time);
			KEY_LOG("˫��Timer=%d\r\n",key_num->double_dlick_timer);
			key_num->double_click = 0;
		  key_num->double_dlick_timer = 0;
			
			key_num->event = DOUBLE_CLICK;
			KEY_LOG("key%d˫��\r\n",key_num->key_id);
		}
		else
		{
			key_num->double_click = 0;
			key_num->double_dlick_timer = 0;			
		}
	}
  
	/* �жϳ��� */
	if(key_num->long_press == TRUE)           
	{		
		if(key_num->long_press_timer >= key_num->long_press_target)
		{
			KEY_LOG("����Timer=%d\r\n",key_num->long_press_timer);
			key_num->long_press_timer = 0;
			key_num->long_press_time = key_tick_get();
			key_num->event = LONG_PRESS;
			KEY_LOG("key%d����\r\n",key_num->key_id);
			
			key_num->click = 0;       //������µ�����Ϣ
			key_num->click_flag = 0;
			
			key_num->bounce_falg = TRUE;  //�����жϵ���
		}
		else
		{
			key_num->long_press_timer = key_tick_elaps(key_num->long_press_time);
		}
	}	
}

/**
	* @name   Key_get_event
	* @brief  ���ذ����¼�
	* @param  key_num�������ṹ��
	* @return �����¼�      
*/
key_event key_get_event(key_t *key_num)
{
	return key_num->event;
}

/**
 * You have to call this function periodically
 * @param tick_period the call period of this function in milliseconds
 */
void key_tick_inc(uint32_t tick_period)
{
    key_tick_irq_flag = 0;
    key_sys_time += tick_period;
}

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t key_tick_get(void)
{
    /*If `lv_tick_inc` is called from an interrupt while `sys_time` is read
     *the result might be corrupted.
     *This loop detects if `lv_tick_inc` was called while reading `sys_time`.
     *If `tick_irq_flag` was cleared in `lv_tick_inc` try to read again
     *until `tick_irq_flag` remains `1`.*/
    uint32_t result;
    do {
        key_tick_irq_flag = 1;
        result        = key_sys_time;
    } while(!key_tick_irq_flag); /*Continue until see a non interrupted cycle*/

    return result;
}

/**
 * Get the elapsed milliseconds since a previous time stamp
 * @param prev_tick a previous time stamp (return value of lv_tick_get() )
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t key_tick_elaps(uint32_t prev_tick)
{
    uint32_t act_time = key_tick_get();

    /*If there is no overflow in sys_time simple subtract*/
    if(act_time >= prev_tick) {
        prev_tick = act_time - prev_tick;
    }
    else {
        prev_tick = UINT32_MAX - prev_tick + 1;
        prev_tick += act_time;
    }

    return prev_tick;
}

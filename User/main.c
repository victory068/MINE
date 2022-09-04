#include "stm32f10x.h"                  // Device header
#include "stm32f10x_it.h"                   
#include "delay-1.h"
#include "usart-2.h"
#include "bsp_i2c.h"
#include "stdio.h"
#include "string.h"
#include "tcp.h"
#include "esp8266.h"
#include "timer.h"
#include "OLED.h"

void uart3_Init(u32 bound);

extern uint32_t  H1;  //Humility
extern uint32_t  T1;  //Temperature

int main(void)
{
	delay_init();     //延时函数初始化  
	uart3_Init(115200);//串口调试
	ESP8266_Init(115200);//串口2初始化
	IIC_Init();
	OLED_Init();
	
	
	while (1)
	{
		
		Tencent_AT();//对接腾讯云AT固件测试demo
		read_AHT20_once();
		delay_ms(1500);
		//ESP8266_Subscribe_To_MQTT_Topic_Information();
		
		OLED_ShowString(1, 1, "temp:");
		OLED_ShowNum(1, 6, T1/10, 2);
		OLED_ShowString(2, 1, "humidity:");
		OLED_ShowNum(2, 10, H1/10, 2);
	    
		//OLED_Clear();
		
		ESP8266_Post_MQTT_Topic_Information();
		delay_ms(1500);
		//ESP8266_Disconnect_MQTT();
		
	}
}


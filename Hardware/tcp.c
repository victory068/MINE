#include "tcp.h"
#include "usart-2.h"
#include "esp8266.h"
#include "delay-1.h"
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"


volatile u8 TcpClosedFlag = 0;
void Tencent_AT(void)
{
	ESP8266_Get_LinkStatus();
	ESP8266_Get_MqttStatus();

	ESP8266_restore();
    ESP8266_Net_Mode_Choose(STA);

	ESP8266_JoinAP(Direct_Connection);//����ʹ����ֱ���ķ�ʽ�����������ԣ�����һ����������AP���������и��Ĳ���ֵ
	//��������
	
	delay_ms(10);
	ESP8266_Set_Device_Information();
	ESP8266_Configure_MQTT_Connection_Parameters();
	

}



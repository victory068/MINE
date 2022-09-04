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

	ESP8266_JoinAP(Direct_Connection);//这里使用了直连的方式，方便做调试，如需一键配网或者AP配网请自行更改参数值
	//配网步骤
	
	delay_ms(10);
	ESP8266_Set_Device_Information();
	ESP8266_Configure_MQTT_Connection_Parameters();
	

}



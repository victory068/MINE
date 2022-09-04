#include "esp8266.h"
#include "usart-2.h"
#include "delay-1.h"
#include "timer.h"
#include "tcp.h"
#include <stdarg.h>

struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct = { 0 };  //������һ������֡�ṹ��
void ESP8266_Init(u32 bound)
{
    uart2_Init(bound); 
}

//��ESP8266ģ�鷢�����ָ��
// cmd �����͵�ָ��
bool ESP8266_Send_AT_Cmd(char *cmd,u32 time)
{
    ESP8266_Fram_Record_Struct .InfBit .FramLength = 0; //���½����µ����ݰ�
    ESP8266_USART("%s\r\n", cmd);
	  
    delay_ms(time);   //��ʱ
    ESP8266_Fram_Record_Struct.Data_RX_BUF[ESP8266_Fram_Record_Struct.InfBit.FramLength ] = '\0';
		return true;			
}


void ESP8266_restore(void)//���ͻָ�����Ĭ��ָ���ʼ��ģ�飬���WIFIģ���������Ϣ
{ 
   ESP8266_Send_AT_Cmd("AT+RESTORE",1000);
}

void ESP8266_Disconnect_Ap(void)//�Ͽ���·����������
{
	 ESP8266_Send_AT_Cmd("AT+CWQAP",1000);
}

void ESP8266_Disconnect_MQTT(void)//�Ͽ���Ѷ������
{
	 ESP8266_Send_AT_Cmd("AT+TCMQTTDISCONN",1000);
}

void ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)//����WIFIģʽ
{
	 switch ( enumMode )
    {
        case STA:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", 2500 );
						 break;
        case AP:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", 2500 );
						 break;
        case STA_AP:
             ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", 2500 );
						 break;
        default:
						 break;
    }       
}

void ESP8266_JoinAP(Distribution_method method)//����WIFI������ʽ
{
	   char cCmd[50];
     switch ( method )
      {
        case Direct_Connection://ֱ��·��
					  sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", STA_ssid, STA_passwd );
            ESP8266_Send_AT_Cmd ( cCmd, 3000 ); 
            break; 
        case SmartConfig://һ������
            ESP8266_Send_AT_Cmd ( "AT+TCSTARTSMART", 1000 ); 
						break;
        case SoftAP://AP����
					  sprintf ( cCmd, "AT+TCSAP=\"%s\",\"%s\"", AP_ssid, AP_passwd );
            ESP8266_Send_AT_Cmd ( cCmd, 1000 );
						break;
        default:
					  break;
      }
}

uint8_t ESP8266_Get_LinkStatus(void)//��ȡ����״̬
{
	 if(ESP8266_Send_AT_Cmd("AT+CIPSTATUS",500))
	 {
		 if(strstr(ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:2\r\n" ))
			 return 2;
		 
		 else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:3\r\n" ) )
       return 3;

     else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:4\r\n" ) )
       return 4;
		 
		 else if ( strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "STATUS:5\r\n" ) )
       return 5;
		 
	 }
	 return 0;
}

uint8_t ESP8266_Get_MqttStatus(void)//��ȡMQTT����״̬
{
	if(ESP8266_Send_AT_Cmd("AT+TCMQTTSTATE?",500))
	 {
		 if(strstr(ESP8266_Fram_Record_Struct .Data_RX_BUF, "+TCMQTTSTATE:1\r\n" ))
			 return 1;
	 }
	 return 0;
}


bool ESP8266_Set_Device_Information(void)//����ƽ̨�豸��Ϣ
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCDEVINFOSET=1,\"%s\",\"%s\",\"%s\"", ProductID, DeviceName, Devicesecret_Checksum );
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}

bool ESP8266_Configure_MQTT_Connection_Parameters(void)//����MQTT���Ӳ���
{
	 return ESP8266_Send_AT_Cmd ( "AT+TCMQTTCONN=1,5000,240,1,0", 1000 );
}


bool ESP8266_Subscribe_To_MQTT_Topic_Information(void)//����MQTT������Ϣ
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCMQTTSUB=\"$thing/down/property/%s/%s\",0",ProductID,DeviceName );
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}

bool ESP8266_Post_MQTT_Topic_Information(void)//����MQTT������Ϣ
{
	 char cCmd[120];
	 sprintf ( cCmd, "AT+TCMQTTPUB =\"$thing/up/property/%s/%s\",0,\"{\"method\":\"report\",\"clientToken\":\"123\",\"params\":{\"temp\":%.1f,\"humidity\":%.1f}}\"",ProductID,DeviceName,Temperature,Humility);
	 return ESP8266_Send_AT_Cmd ( cCmd, 1000 );
}


static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];


    va_list ap;
    va_start(ap, Data);

    while ( * Data != 0 )     // �ж������Ƿ񵽴������
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //�س���
                USART_SendData(USARTx, 0x0d);
                Data ++;
                break;

                case 'n':                                     //���з�
                USART_SendData(USARTx, 0x0a);   
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }

        else if ( * Data == '%')
        {                                     
            switch ( *++Data )
            {               
                case 's':                                         //�ַ���
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                Data++;
                break;

                case 'd':           
                    //ʮ����
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
                    USART_SendData(USARTx,*s);
                    while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else USART_SendData(USARTx, *Data++);
        while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );

    }
}

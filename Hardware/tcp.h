#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"

#define ProductID "SP1NINUEPT"
#define DeviceName "f103"
#define Devicesecret_Checksum "cRvC3RuFnFyaB7ClFWDVIQ=="
//#define data "hello_world"
#define Temperature (T1/100)*10+((T1/10)%10)+(T1%10)*0.1
#define Humility    (H1/100)*10+((H1/10)%10)+(H1%10)*0.1

extern uint32_t  H1;  //Humility
extern uint32_t  T1;  //Temperature

extern volatile uint8_t TcpClosedFlag;  //连接状态标志

void Tencent_AT(void);

#endif

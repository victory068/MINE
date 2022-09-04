#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//STM32F103ºËÐÄ°åÀý³Ì
//¿âº¯Êý°æ±¾Àý³Ì
/********** mcudev.taobao.com ³öÆ·  ********/

//	 
//STM32¿ª·¢°å
//´®¿Ú1³õÊ¼»¯		   

#define USART_REC_LEN  			200  	//¶¨Òå×î´ó½ÓÊÕ×Ö½ÚÊý 200
#define EN_USART1_RX 			1		    //Ê¹ÄÜ£¨1£©/½ûÖ¹£¨0£©´®¿Ú1½ÓÊÕ
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.Ä©×Ö½ÚÎª»»ÐÐ·û 
extern u16 USART_RX_STA;         		//½ÓÊÕ×´Ì¬±ê¼Ç	
//Èç¹ûÏë´®¿ÚÖÐ¶Ï½ÓÊÕ£¬Çë²»Òª×¢ÊÍÒÔÏÂºê¶¨Òå
void uart_init(u32 bound);
#endif


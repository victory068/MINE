#include "delay-1.h"
#include "sys.h"

//STM32F103o?D?��?��y3��
//?ao����y��?��?��y3��
/********** mcudev.taobao.com 3??��  ********/


// 	 
//��?1?��1��?ucos,?���㨹����????��?����???t?��?��.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ��1��?	  
#endif
//	 

//STM32?a�����?
//��1��?SysTick��???����??��y?�꨺????��3��??DD1������
//�㨹����delay_us,delay_ms

// 	 
static u8  fac_us=0;//us��ʱ���ٳ���
static u16 fac_ms=0;//ms��ʱ���ٳ���
#ifdef OS_CRITICAL_METHOD 	//��?1?OS_CRITICAL_METHOD?����?��?,?��?�¨�1��?ucosII��?.
//systick?D??��t??o����y,��1��?ucos������?��?
void SysTick_Handler(void)
{				   
	OSIntEnter();		//??��??D??
    OSTimeTick();       //�̡¨�?ucos��?����?����t??3��D��               
    OSIntExit();        //�䣤���騨????D??����?D??
}
#endif

//3?��??��?��3��o����y
//�̡���1��?ucos��?����o��,��?o����y?��3?��??��ucos��?����?��?��??
//SYSTICK��?����?��1��?��?aHCLK����?����?1/8
//SYSCLK:?�̨�3����?��
void delay_init()	 
{

#ifdef OS_CRITICAL_METHOD 	//��?1?OS_CRITICAL_METHOD?����?��?,?��?�¨�1��?ucosII��?.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//72000 0000/8=9000 000  HCLK/8
	fac_us=SystemCoreClock/8000000;	//fac_us=72000 000/8000 000=9
	 
#ifdef OS_CRITICAL_METHOD 	//��?1?OS_CRITICAL_METHOD?����?��?,?��?�¨�1��?ucosII��?.
	reload=SystemCoreClock/8000000;		//?????����???��y��?��y �̣�???aK	   
	reload*=1000000/OS_TICKS_PER_SEC;//?��?YOS_TICKS_PER_SEC����?����?3?����??
							//reload?a24????��??��,��?�䨮?��:16777216,?��72M??,??o?1.86s��������	
	fac_ms=1000/OS_TICKS_PER_SEC;//�䨲����ucos?����??��������?��?�����̣�??	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//?a??SYSTICK?D??
	SysTick->LOAD=reload; 	//??1/OS_TICKS_PER_SEC???D??��?��?	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//?a??SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;//fac_ms=9*1000=9000
#endif
}								    

#ifdef OS_CRITICAL_METHOD	//��1��?��?ucos
//?������nus
//nus?a��a?��������?us��y.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	//LOAD��??��	    	 
	ticks=nus*fac_us; 			//D����a��??��??��y	  		 
	tcnt=0;
	told=SysTick->VAL;        	//????��?������???��y?��?��
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//?a��?���騰a��???SYSTICK��?��???��Y??��???��y?��?��?����?��?.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//����??3?1y/�̨�������a?��3����?����??,?����?3?.
		}  
	}; 									    
}
//?������nms
//nms:��a?��������?ms��y
void delay_ms(u16 nms)
{	
	if(OSRunning==TRUE)//��?1?os��??-?��?����?	    
	{		  
		if(nms>=fac_ms)//?��������?����??�䨮����ucos��?��?��������???��?�� 
		{
   			OSTimeDly(nms/fac_ms);//ucos?������
		}
		nms%=fac_ms;				//ucos��??-?T��������1??a?��D?��??��������?,2����???������?��??������    
	}
	delay_us((u32)(nms*1000));	//??������?��??������,��?����ucos?T����???���̡�?��.
}
#else//2?��?ucos����
//?������nus
//nus?a��a?��������?us��y.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����  	(SysTick->LOADΪ24bit) ��װ����ֵ�Ĵ��� 	 
	SysTick->VAL=0x00;        //��ռ�����  ��ǰ��ֵ�Ĵ���
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����   ���Ƽ�״̬�Ĵ��� 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��  
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  
}

//?������nms
//���騰anms��?��??��
//SysTick->LOAD?a24????��??��,?����?,��?�䨮?������?a:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK�̣�???aHz,nms�̣�???ams
//??72M��??t??,nms<=1864 

//ע��MS�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ��������������ʱΪ
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λ��MHz,nm��λΪms
//��48M������,nms<=2796
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����  	    
} 
#endif

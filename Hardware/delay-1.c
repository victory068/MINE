#include "delay-1.h"
#include "sys.h"

//STM32F103o?D?°?ày3ì
//?aoˉêy°?±?ày3ì
/********** mcudev.taobao.com 3??·  ********/


// 	 
//è?1?ê1ó?ucos,?ò°üà¨????μ?í·???t?′?é.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ê1ó?	  
#endif
//	 

//STM32?a·￠°?
//ê1ó?SysTickμ???í¨??êy?￡ê????ó3ù??DD1üàí
//°üà¨delay_us,delay_ms

// 	 
static u8  fac_us=0;//us延时倍速乘数
static u16 fac_ms=0;//ms延时倍速乘数
#ifdef OS_CRITICAL_METHOD 	//è?1?OS_CRITICAL_METHOD?¨ò?á?,?μ?÷ê1ó?ucosIIá?.
//systick?D??·t??oˉêy,ê1ó?ucosê±ó?μ?
void SysTick_Handler(void)
{				   
	OSIntEnter();		//??è??D??
    OSTimeTick();       //μ÷ó?ucosμ?ê±?ó·t??3ìDò               
    OSIntExit();        //′￥·￠è????D??èí?D??
}
#endif

//3?ê??ˉ?ó3ùoˉêy
//μ±ê1ó?ucosμ?ê±oò,′?oˉêy?á3?ê??ˉucosμ?ê±?ó?ú??
//SYSTICKμ?ê±?ó1ì?¨?aHCLKê±?óμ?1/8
//SYSCLK:?μí3ê±?ó
void delay_init()	 
{

#ifdef OS_CRITICAL_METHOD 	//è?1?OS_CRITICAL_METHOD?¨ò?á?,?μ?÷ê1ó?ucosIIá?.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//72000 0000/8=9000 000  HCLK/8
	fac_us=SystemCoreClock/8000000;	//fac_us=72000 000/8000 000=9
	 
#ifdef OS_CRITICAL_METHOD 	//è?1?OS_CRITICAL_METHOD?¨ò?á?,?μ?÷ê1ó?ucosIIá?.
	reload=SystemCoreClock/8000000;		//?????óμ???êy′?êy μ￥???aK	   
	reload*=1000000/OS_TICKS_PER_SEC;//?ù?YOS_TICKS_PER_SECéè?¨ò?3?ê±??
							//reload?a24????′??÷,×?′ó?μ:16777216,?ú72M??,??o?1.86s×óóò	
	fac_ms=1000/OS_TICKS_PER_SEC;//′ú±íucos?éò??óê±μ?×?éùμ￥??	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//?a??SYSTICK?D??
	SysTick->LOAD=reload; 	//??1/OS_TICKS_PER_SEC???D??ò?′?	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//?a??SYSTICK    
#else
	fac_ms=(u16)fac_us*1000;//fac_ms=9*1000=9000
#endif
}								    

#ifdef OS_CRITICAL_METHOD	//ê1ó?á?ucos
//?óê±nus
//nus?aòa?óê±μ?usêy.		    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;	//LOADμ??μ	    	 
	ticks=nus*fac_us; 			//Dèòaμ??ú??êy	  		 
	tcnt=0;
	told=SysTick->VAL;        	//????è?ê±μ???êy?÷?μ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//?aà?×￠òaò???SYSTICKê?ò???μY??μ???êy?÷?í?éò?á?.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//ê±??3?1y/μèóúòa?ó3ùμ?ê±??,?òí?3?.
		}  
	}; 									    
}
//?óê±nms
//nms:òa?óê±μ?msêy
void delay_ms(u16 nms)
{	
	if(OSRunning==TRUE)//è?1?osò??-?ú?üá?	    
	{		  
		if(nms>=fac_ms)//?óê±μ?ê±??′óóúucosμ?×?éùê±???ü?ú 
		{
   			OSTimeDly(nms/fac_ms);//ucos?óê±
		}
		nms%=fac_ms;				//ucosò??-?T·¨ìá1??a?′D?μ??óê±á?,2éó???í¨·?ê??óê±    
	}
	delay_us((u32)(nms*1000));	//??í¨·?ê??óê±,′?ê±ucos?T·¨???ˉμ÷?è.
}
#else//2?ó?ucosê±
//?óê±nus
//nus?aòa?óê±μ?usêy.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //时间加载  	(SysTick->LOAD为24bit) 重装载数值寄存器 	 
	SysTick->VAL=0x00;        //清空计数器  当前数值寄存器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数   控制及状态寄存器 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达  
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  
}

//?óê±nms
//×￠òanmsμ?·??§
//SysTick->LOAD?a24????′??÷,?ùò?,×?′ó?óê±?a:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLKμ￥???aHz,nmsμ￥???ams
//??72Mì??t??,nms<=1864 

//注意MS的范围
//SysTick->LOAD为24位寄存器，所以最大延时为
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位国MHz,nm单位为ms
//对48M条件下,nms<=2796
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器  	    
} 
#endif

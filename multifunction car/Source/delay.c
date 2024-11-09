
#include "delay.h"	


/*****************************************************************
功能描述：延时函数
入口参数：uint16 x ，该值为1时，延时1ms
返回值：无
*****************************************************************/
void delay_ms(uint16 x) 
{  
	uint16 j,i;   
	for(j=0;j<x;j++)   
	{    
		for(i=0;i<1100;i++);   
	}  
}

/*******************************************************************
功能描述：延时函数，延时1us
入口参数：无
返回值：无
********************************************************************/
void Delay1us()		
{
	uint8 i;

	i = 3;
	while (--i);
}



/*******************************************************************
功能描述：延时函数
入口参数：uint16 x ，该值为1时，延时1us
返回值：无
********************************************************************/
void delay_us(uint16 t)
{
	while(t--)
	{
		Delay1us();
	}
}

/*********************************END FILE********************************************/	


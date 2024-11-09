
#include "STC12C5A60S2.h"
#include  <intrins.h>
#include "stdio.h"
#include "string.h"
#include "ps2.h"

//定义SPI通信的引脚
//这里可以修改对应IO口
sbit  DATA=P1^4;	   //相当于DI/MOS
sbit  CMND=P1^3;       //相当于DO/MIS
sbit  ATT=P1^2;        //相当于片选信号
sbit  CLK=P1^5;        //相当于时钟信号

//初始化PS2时我们需要通过主机给手柄发送的数据来配置的
uchar code lx1[5]={0x01,0x43,0x00,0x01,0x00};						// 进入配置模式
uchar code lx2[9]={0x01,0x44,0x00,0x01,0x03,0x00,0x00,0x00,0x00}; 	// 设置模式
uchar code lx3[9]={0x01,0x4f,0x00,0xff,0xff,0x03,0x00,0x00,0x00}; 	// 设置字节长度
uchar code lx4[9]={0x01,0x43,0x00,0x00,0x5A,0x5A,0x5A,0x5A,0x5A}; 	// 退出配置模式

//定义我们主机需要发给我们手柄的数据
uchar code scan[9]={0x01,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//{0x01,0x42,0x00,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};	// 类型读取
//存储由手柄反馈给我们手机的数据
uchar  out[9];
//保存我们所需要的data[3]和data[4]即手柄的按键值	    
uint FunBtn = 0xffff;
		   	
stPS2KeyValue stPS2key;
stPS2KeyValue * stPS2 = &stPS2key;

/**
* Function       delayms
* @author        Danny
* @date          2017.08.16
* @brief         延时函数
* @param         xms：延时时间（ms）
* @retval        void
* @par History   无
*/	
void delayms(uint xms)	         
{
   uint i,j;   
   for(i=xms;i>0;i--)
      for(j=850;j>0;j--);
}

/**
* Function       delay
* @author        Danny
* @date          2017.08.16
* @brief         延时函数
* @param         延时一个指令周期
* @retval        void
* @par History   无
*/	

static void delay(uint n) 		  //计时不准确
{
	uint i;
	for(i=0;i<=n;i++) 
	    _nop_();                 //空指令延时一个指令周期
		_nop_();	
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
		_nop_();				
}


/**
* Function       spi_write
* @author        Danny
* @date          2017.08.16
* @brief         向SPI器件（手柄）中写入一个的数据
* @param         command:要写入到手柄的数据
* @retval        void
* @par History   无
*/
void spi_write(uchar command)			 
{
    uchar i;
    for(i=0; i<=7; i++)              //将数据一位一位的写入SPI器件 
    {
		if(command&0x01)	         //取出写入的数据的最右边的一位
			CMND=1;                  //代表主机入从机出
			else
			CMND=0;
		command=command>>1;         //将要写入的数据右移一位，
		_nop_();	                //必须要加延时，否则会因为操作太快而不响应。
		_nop_();
		CLK=0;                      //上升沿写入数据
		delay(10*10);                   
		CLK=1;
		delay(1*10);
    }
	CMND=1;
}

/**
* Function       spi_read
* @author        Danny
* @date          2017.08.16
* @brief         从SPI器件中读出一个的数据
* @param         void
* @retval        void
* @par History   无
*/
static uchar spi_read(uchar command)      
{
    uchar i,j=1;
	uchar res=0; 
    for(i=0; i<=7; i++)          
    {
		if(command&0x01)             //将数据一位一位的从SPI器件中读取出来
			CMND=1;					 //主入从出
		else
			CMND=0;
		command = command >> 1;     //将要读出的数据右移一位，
		_nop_();	                //必须要加延时，否则会因为操作太快而不响应。
		_nop_();
		CLK=0;
		delay(2);
		if(DATA)                   //如果DATA为真的话，则代表主机出从机入（接收模块发送出数据，也就是从SPI器件中读出一个数据）
		res = res + j;
		j=j<<1; 
		CLK=1;
		delay(5);		 
    }
    CMND=1;
    return res;	                   //res是读出来的数据,也就是手柄反馈给主机的数据
}
 
/**
* Function       PS2_Init
* @author        Danny
* @date          2017.08.16
* @brief         PS2初始化
* @param         void
* @retval        void
* @par History   无
*/	
void PS2_Init()
{
    uchar i =0;
    //out数组初始化
	for(i=0; i<9; i++)  	
	{
    	out[i]=0x00;	
	}  
	//片选信号清零,主从机在通信的过程中需要将片选线拉低的
	ATT=0;
	delay(7);
	for(i=0;i<9;i++)	
	{
		out[i]=spi_read(scan[i]);
		delay(7);
	} 
	ATT=1;
	delayms(15);
 
   	//片选信号清零,主从机在通信的过程中需要将片选线拉低的
	//通过主机向手柄中写入数据来配置手柄
	ATT=0;
	delay(7);
	for(i=0; i<5; i++)  
	{
		spi_write(lx1[i]);		   //进入配置模式
		delay(7);	
	} 
    ATT=1;
	delayms(8); 

    //片选信号清零,主从机在通信的过程中需要将片选线拉低的
	//通过主机向手柄中写入数据来配置手柄
	ATT=0;
	delay(7);
	for(i=0; i<9; i++)	 		  //配置模式
	{
		spi_write(lx2[i]);
		delay(7);																
	} 
	ATT=1;
	delayms(8); 

    //片选信号清零,主从机在通信的过程中需要将片选线拉低的
	//通过主机向手柄中写入数据来配置手柄
	ATT=0;
	delay(7);	   
	for(i=0; i<9; i++)	
	{							  //设置字节长度
		spi_write(lx3[i]);
		delay(7);	
	} 
	ATT=1;
	delayms(8); 
	
	//片选信号清零,主从机在通信的过程中需要将片选线拉低的
	//通过主机向手柄中写入数据来配置手柄
	ATT=0;
	delay(7);	
	for(i=0; i<9; i++)			  //退出配置模式
	{
		spi_write(lx4[i]);
		delay(7);	
	} 
	ATT=1;
	delayms(8); 
		
}

/**
* Function       ReadPS2KeyValue
* @author        Danny
* @date          2017.08.16
* @brief         PS2按键检测
* @param         void
* @retval        void
* @par History   无
*/	
stPS2KeyValue* ReadPS2KeyValue(void)
{
    
    //保存我们所需要的data[3]和data[4]即手柄的按键值
	//只不过我们这里用的是局部变量
	uint funbtn = 0xffff;
	uchar i =0;
	ATT=0;
	for(i=0;i<9;i++)	//更新扫描按键
	{
		out[i]=spi_read(scan[i]);	
	} 
	ATT=1;
	//把读到的data[3]和data[4]保存在funbtn中去
	funbtn = ((uint)out[4] << 8) | out[3];
	//把从手柄检测按键按下的数据提取出来赋给全局变量
	stPS2->FunBtn = funbtn;
	//后面的5,6,7,8则分别是我们的摇杆数据
	stPS2->X1 = out[7];
	stPS2->Y1 = out[8];
	stPS2->X2 = out[5];
	stPS2->Y2 = out[6];
	return stPS2;
}


/*********************************************************
作者：		Jellina电子
说明：进行小开发板测试的例程，共三个现象：
		1：
		使用串口调试助手，选择对应串口号，波特率9600，无校验，1位停止位	   			
		使用“文本模式”接收字符：	
		"STC12/15 Series MCU Check OK!"
		"Uart1 Test OK!"
		
		2：
		串口的两个LED都闪烁
**********************************************************/
#include "reg51.h"
#include "intrins.h"
typedef unsigned char BYTE;
typedef unsigned int WORD;
#define FOSC 11059200L  //System frequency
#define BAUD 9600  //UART baudrate

sfr AUXR = 0x8e; //辅助寄存器
sbit P30        =   P3^0;

bit busy;
void SendData(BYTE dat);
void SendStringByUart1(char *s);
void BYTEToStr(BYTE n, BYTE outstr[]);
void Delay_1ms(WORD x);

void UartInit(void)		//9600bps@11.0592MHz
{

	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TH1 = TL1 = -(FOSC/12/32/BAUD);		//设定定时初值 和 重装值
	
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1


	ES = 1;  //Enable UART interrupt
	EA = 1;  //Open master interrupt switch
}




void main()
{
	BYTE num = 0;
	BYTE str[4];
	UartInit();
	while(1)
	{
		SendStringByUart1("\r\nSTC12/15 Series MCU Check OK!\r\nUart1 Test OK!\r\n");
		BYTEToStr(num, str);
		SendStringByUart1(str);
		Delay_1ms(1000);
		P30 = ~P30;	//12/15系列RXD1
		Delay_1ms(1000);
		num++;
	}
	
}
/*----------------------------
UART interrupt service routine
----------------------------*/
void Uart_Isr() interrupt 4 using 1
{
	if (RI)
	{
		RI = 0;  //Clear receive interrupt flag		
	}
	if (TI)
	{
		TI = 0;  //Clear transmit interrupt flag
		busy = 0;  //Clear transmit busy flag
	}
}
/*----------------------------
Send a byte data to UART
Input: dat (data to be sent)
Output:None
----------------------------*/
void SendData(BYTE dat)
{
	while (busy);  //Wait for the completion of the previous data is sent	
	busy = 1;
	SBUF = dat;  //Send data to UART buffer
}
/*----------------------------
Send a string to UART
Input: s (address of string)
Output:None
----------------------------*/
void SendStringByUart1(char *s)
{
 while (*s)  //Check the end of the string
 {
  SendData(*s++);  //Send current char and increment string ptr
 }
}


void BYTEToStr(BYTE n, BYTE outstr[])
{
	if(n < 0)
	{
	 	outstr[0] = '-';
		n = -n; 
	}
	else
	{
		outstr[0] =  n / 100 + '0';
	}
	n = n%100;

	outstr[1] = n/10 + '0';
	outstr[2] = n%10 + '0';
	outstr[3] = '\0';
}

  /*********************************************   延迟程序  *********************************************/
void Delay_1ms(WORD x) 
{  
	WORD j,i;   
	for(j=0;j<x;j++)   
	{    
		for(i=0;i<120;i++);   //延时X乘以120  
	}  
}
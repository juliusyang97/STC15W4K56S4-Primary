/*********************************************************
���ߣ�		Jellina����
˵��������С��������Ե����̣�����������
		1��
		ʹ�ô��ڵ������֣�ѡ���Ӧ���ںţ�������9600����У�飬1λֹͣλ	   			
		ʹ�á��ı�ģʽ�������ַ���	
		"STC12/15 Series MCU Check OK!"
		"Uart1 Test OK!"
		
		2��
		���ڵ�����LED����˸
**********************************************************/
#include "reg51.h"
#include "intrins.h"
typedef unsigned char BYTE;
typedef unsigned int WORD;
#define FOSC 11059200L  //System frequency
#define BAUD 9600  //UART baudrate

sfr AUXR = 0x8e; //�����Ĵ���
sbit P30        =   P3^0;

bit busy;
void SendData(BYTE dat);
void SendStringByUart1(char *s);
void BYTEToStr(BYTE n, BYTE outstr[]);
void Delay_1ms(WORD x);

void UartInit(void)		//9600bps@11.0592MHz
{

	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TH1 = TL1 = -(FOSC/12/32/BAUD);		//�趨��ʱ��ֵ �� ��װֵ
	
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1


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
		P30 = ~P30;	//12/15ϵ��RXD1
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

  /*********************************************   �ӳٳ���  *********************************************/
void Delay_1ms(WORD x) 
{  
	WORD j,i;   
	for(j=0;j<x;j++)   
	{    
		for(i=0;i<120;i++);   //��ʱX����120  
	}  
}
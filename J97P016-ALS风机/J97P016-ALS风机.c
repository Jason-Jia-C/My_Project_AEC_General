/*
项目名称: ALS风机
芯片型号: HT66F002 内部8M 指令周期4分频
功能:
待机休眠功耗 ？uA

2路输出，按下按键显示电量


脚位定义
			 -------
			|VDD VSS|
        KEY |PA6 PA0| 
     Status |PA5 PA1| SN
	        |PA7 PA2| 
			 -------

*/
#include "HT66F002.h"
#define bool	bit
#define uint8 	unsigned char 
#define uint16	unsigned int 

#define SP_FLAG	0

#define KEY     	_pa6
#define Status		_pa5
#define	Status_c	_pac5
#define SN			_pa1
#define SW			_pa2


#pragma	vector int0_int @ 0x04
#pragma vector tb0_int	@ 0x08 		//tb0中断
#pragma vector tb1_int	@ 0x0C
#pragma vector mf0_int	@ 0x10
#pragma vector eep_int	@ 0x14
#pragma vector adc_int	@ 0x18
void int0_int(void)	{_inte=0;_intf=0;}
void tb1_int(void)	{_tb1e=0;_tb1f=0;}
void mf0_int(void)	{_mf0e=0;_mf0f=0;}
void eep_int(void)	{_dee=0;_def=0;}
void adc_int(void)	{_ade=0;_adf=0;}
void 	Seg_Con();
void 	IO_Con();
void 	Var_Con();
#pragma rambank0

//Halt
bool	b_halt_flag;
uint8	u8_tm_halt_100ms;
//TM
bool	b_tm_1ms_flag;
bool	b_tm_10ms_flag;
bool	b_tm_100ms_flag;
bool	b_tm_1s_flag;

uint8	u8_tm_1ms_cnt;
uint8	u8_tm_10ms_cnt;
uint8	u8_tm_100ms_cnt;
//KEY
bool 	b_key1;		
bool 	b_key_trg1;
bool 	b_key_date1;
bool 	b_key_cont1;	
uint8 	u8_tm_key1_1ms;

bool 	b_key_flag;
//FAN
uint8	u8_fan_mode;
	
#pragma norambank
void Var_Con()
{
//Halt
	b_halt_flag=0;
	u8_tm_halt_100ms=0;
//TM
	b_tm_1ms_flag=0;
	b_tm_10ms_flag=0;
	b_tm_100ms_flag=0;
	b_tm_1s_flag=0;

	u8_tm_1ms_cnt=0;
	u8_tm_10ms_cnt=0;
	u8_tm_100ms_cnt=0;
//KEY
 	b_key1=0;
 	b_key_trg1=0;
 	b_key_date1=0;
 	b_key_cont1=0;	
	u8_tm_key1_1ms=0;
 
//Fan
	 u8_fan_mode=0;
}

void Seg_Con()	
{
	_smod	=0b00000001; 	// 系统时钟选择  FSYS=FH
    if(b_halt_flag==1)
    {
        _intc0	=0b00000000; 	//中断设置
        _tbc	=0B00000000;  	//时基设置
        _wdtc	=0b10101111; 	
    }
    else
    {
        _intc0	=0b00000101; 	//中断设置
        _tbc	=0B11000011;  	//时基设置  tb0  976hz   1ms  
        _wdtc	=0b01010111; 
    }
	_smod1	=0b00000000;	
	_sadc0	=0B00000000;
	_sadc1	=0B00000000;
	_sadc2	=0B00000000;
	_integ	=0b00000000;	
	_intc1	=0b00000000;	//中断设置
	_mfi0	=0b00000000;	//多功能中断设置
}
/*			 -------
			|VDD VSS|
        KEY |PA6 PA0| 
     Status |PA5 PA1| SN
	        |PA7 PA2| SW 
			 -------				*/
void IO_Con()
{
	if(b_halt_flag==1)
	{
		_pa		=0b01000000;	//
		_pac	=0b01100010;	//输入输出设置 	0输出 1输入
		_papu	=0b01000000;	//上拉电阻  	0除能 1使能 
		_pawu	=0b01000000;	//唤醒功能位  	0除能 1使能 
		_pasr	=0b00000000;    //功能选择   
		_pa		=0b01000000;	
	}
	else
	{
		_pa		=0b01000000;	//
		_pac	=0b01000000;	//输入输出设置 	0输出 1输入
		_papu	=0b01000000;	//上拉电阻  	0除能 1使能 
		_pawu	=0b01000000;	//唤醒功能位  	0除能 1使能 
		_pasr	=0b00000000;    //功能选择   
		_pa		=0b01000000;	
	}
}

void KEY_San()
{
	if(KEY!=b_key1)
	{
		if(u8_tm_key1_1ms>15)
		{
			b_key1=KEY;
			u8_tm_key1_1ms=0;
		} 
	}
	else
	{
		u8_tm_key1_1ms=0;
		b_key_date1=b_key1^0x01;
		b_key_trg1=b_key_date1&(b_key_date1^b_key_cont1);
		b_key_cont1=b_key_date1;
	}
}
void KEY_Con()
{
	KEY_San();
	if(b_key_cont1==1&&b_key_trg1==1)
	{
		u8_fan_mode++;
		if(u8_fan_mode>2)
			u8_fan_mode=0;
		if(u8_fan_mode==0)
		{
			SN=0;
			SW=0;
			Status_c=0;
			Status=0;
		}
		else 
		{
			SN=1;
			SW=1;
			if(u8_fan_mode==1)
			{
				Status_c=1;	
			}
			else
			{
				Status_c=0;
				Status=0;
			}
		}
	}
}
void tb0_int()
{
	b_tm_1ms_flag=1;
	_pa7=~_pa7;
}
void TM_Con()
{
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	if(b_tm_1ms_flag==1)
		u8_tm_1ms_cnt++;
	if(u8_tm_1ms_cnt>=10)
	{
		u8_tm_1ms_cnt=0;
		b_tm_10ms_flag=1;
		u8_tm_10ms_cnt++;
	}
	if(u8_tm_10ms_cnt>=10)
	{
		u8_tm_10ms_cnt=0;
		b_tm_100ms_flag=1;
		u8_tm_100ms_cnt++;
	}
	if(u8_tm_100ms_cnt>=10)
	{
		u8_tm_100ms_cnt=0;
		b_tm_1s_flag=1;
	}
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	/*******************************************/
	if(b_tm_1ms_flag==1)
	{
		b_tm_1ms_flag=0;
		u8_tm_key1_1ms++;
	}
	if(b_tm_10ms_flag==1)
	{
		b_tm_10ms_flag=0;	
	}
	if(b_tm_100ms_flag==1)
	{
		b_tm_100ms_flag=0;
		u8_tm_halt_100ms++;
	}
	if(b_tm_1s_flag==1)
	{
		b_tm_1s_flag=0;
	}
}
void Halt_Con()
{
	if(u8_fan_mode>0)
		u8_tm_halt_100ms=0;
	if(u8_tm_halt_100ms>49)
	{
		u8_tm_halt_100ms=0;
		b_halt_flag=1;	
	}
	if(b_halt_flag==1)
	{
		IO_Con();
		Seg_Con();
		_nop();
		_nop();
		_halt();
		_nop();
		_nop();
		Var_Con();
		Seg_Con();
		IO_Con();
	}
}

void main()
{
	Var_Con();
	IO_Con();
	Seg_Con();
	while(1)
	{
		_clrwdt();
		
		TM_Con();
		KEY_Con();
		Halt_Con();
///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
///////////////////////////HALT//////////////////////////////////////////////////////////////////////////
	}

}


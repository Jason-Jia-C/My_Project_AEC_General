#include "HT66F002.h"

#define		bool	bit
#define		uint8	unsigned char
#define		uint16	unsigned int

#define		LED_TOP				_pa5	//pin3
#define		LED_FRONT			_pa2	//pin5
#define		KEY					_pa1	//pin6

/////////////////////////////////////////////////////////////
#pragma 	vector		ext0_int	@ 0x04		//外部中断0
#pragma 	vector		tb0_int		@ 0x08		//Time Base 0
#pragma 	vector		tb1_int		@ 0x0C		//Time Base 1
#pragma 	vector		mf0_int		@ 0x10 		//多功能0 stm中断
#pragma		vector		eeprom_int	@ 0x14		//EEPROM
#pragma 	vector		adc_int		@ 0x18		//AD

/////////////////////////////////////////////////////////////

void	ext0_int		(void){_intf=0;_inte=0;}
void	mf0_int			(void){_mf0f=0;_mf0e=0;}
void	tb1_int			(void){_tb1e = 0;}
void	eeprom_int		(void){_dee = 0;}
void    adc_int		    (void){_ade = 0;_adf=0;}

/////////////////////////////////////////////////////////////
#pragma	rambank0
	//t	
	bool	b_tm_1ms_flag;
	bool	b_tm_25ms_flag;
	bool	b_tm_500ms_flag;
	
	uint8	u8_tm_1ms;
	uint8	u8_tm_25ms;
	
	//KEY
	bool 	b_key1;
	bool 	b_key_trg1;
	bool 	b_key_date1;
	bool 	b_key_cont1;
	bool 	b_key_flag1;
	uint8 	u8_tm_key_check_1ms;
	uint8 	u8_tm_key_25ms;	
	
	//LED
	bool	b_led_shutdown_flag;
	
	uint8	u8_led_step;
	uint8	u8_led_mode;
	uint8	u8_led_mode1;
	
	//halt
	
	bool	b_halt_flag;
	uint8	u8_tm_halt_1ms;
	uint8	u8_tm_halt_250ms;


#pragma	norambank



void system_csh()
{
	_smod	=0b00000001;
	_smod1	=0b00000000;
	_wdtc	=0b01010111;
	_rstc	=0b01010101;
	
	_pa		=0b00000010;
	_pac	=0b00000010;
	_papu	=0b00000010;
	_pawu	=0b00000010;
	_pasr	=0b00000000;
	_pa		=0b00000010;
	
	_ifs0	=0b00000000;
	
	_stm0c0	=0b10000000;
	_stm0c1	=0b00000000;
	
	_sadc0	=0b00000000;
	_sadc1	=0b00000000;
	_sadc2	=0b00000000;
	
	_integ	=0b00000000;
	_intc0	=0b00000101;
	_intc1	=0b00000000;
	_mfi0	=0b00000000;
	
	_tbc	=0b11000011;	//1ms		
}

void value_csh()
{
	//t	
	b_tm_1ms_flag=0;
	b_tm_25ms_flag=0;
	b_tm_500ms_flag=0;
	
	u8_tm_1ms=0;
	u8_tm_25ms=0;
	
	//KEY
	b_key1=1;
	b_key_trg1=0;
	b_key_date1=0;
	b_key_cont1=0;
	b_key_flag1=0;
	u8_tm_key_check_1ms=0;
	u8_tm_key_25ms=0;	
	
	//LED
	b_led_shutdown_flag=0;
	
	u8_led_step=0;
	u8_led_mode=0;
	u8_led_mode1=0;		
	
	//	
	b_halt_flag=0;
	u8_tm_halt_1ms=0;
	u8_tm_halt_250ms=0;
	
}

uint8 eeprom_read(uint8 u8_eerom_eea)
{
	_eea   = u8_eerom_eea;
	_mp1   = 0x40;
	_bp    = 0X01;
	_iar1 |= 0x02;
	_iar1 |= 0x01;
	while((_iar1&0x01) == 0x01)
	{
		_clrwdt();
	}
	_iar1  = 0;
	_bp    = 0;
	return _eed;
}
void eeprom_wirte(uint8 u8_eerom_eea,uint8 u8_eeprom_eed)
{
	_eea   = u8_eerom_eea;
	_eed   = u8_eeprom_eed;
	_mp1   = 0x40;
	_bp    = 0X01;
	_emi   = 0;			//写周期执行前总中断位除能
	_iar1 |= 0x08;
	_iar1 |= 0x04;
	_emi   = 1;			//写周期执行后总中断位使能
	while((_iar1&0x04) == 0x04)
	{
		_clrwdt();
	}
	_iar1  = 0;
	_bp    = 0;
}


void tb0_int()		//1ms
{
	b_tm_1ms_flag=1;
}

void main()
{
	value_csh();
	if(_lvrf==1)
	{
		u8_led_mode=eeprom_read(1);
		_lvrf=0;
	}
	system_csh();
	
	
	while(1)
	{
		_clrwdt();
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////	
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////			
		
		
		
		if(b_tm_1ms_flag==1)
		{
			b_tm_1ms_flag=0;
			u8_tm_1ms++;
			if(u8_tm_1ms>25)
			{
				u8_tm_1ms=0;
				u8_tm_key_25ms++;
			}
			if(KEY!=b_key1)
				u8_tm_key_check_1ms++;
			else
				u8_tm_key_check_1ms=0;
			if(u8_led_mode==0)
				u8_tm_halt_1ms++;
			else
				u8_tm_halt_1ms=0;
		}
		if(u8_led_mode==0)
		{
			if(u8_tm_halt_1ms>250)
			{
				u8_tm_halt_1ms=0;
				u8_tm_halt_250ms++;
			}
		}
		else
			u8_tm_halt_250ms=0;
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////	
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////			

////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////
		if(KEY!=b_key1)
		{
			if(u8_tm_key_check_1ms>25)
			{
				b_key1=KEY;
				u8_tm_key_check_1ms=0;
			}
		}
		else
		{
			b_key_date1=b_key1^0x01;
			b_key_trg1=b_key_date1&(b_key_date1^b_key_cont1);
			b_key_cont1=b_key_date1;
		}
		if(b_key_cont1==1&&b_key_trg1==1)
		{
			b_key_flag1=1;
		}
		if(b_key_cont1==0&&b_key_flag1==1)
		{
			u8_led_step=1;
			b_key_flag1=0;
		}
		if(b_key_cont1==0&&u8_led_mode>0)
		{
			if(b_led_shutdown_flag==0)
			{
				if(u8_tm_key_25ms>=200)
				{
					b_led_shutdown_flag=1;
					u8_tm_key_25ms=0;
				}	
			}
			else
				u8_tm_key_25ms=0;
		}
		else
			u8_tm_key_25ms=0;
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////

////////////////////////////LED//////////////////////////////////////////////////////////////////////////
////////////////////////////LED//////////////////////////////////////////////////////////////////////////
		if(u8_led_step==1)
		{
			if(b_led_shutdown_flag==0)
			{ 
				if(u8_led_mode1>0)
				{
					u8_led_mode=u8_led_mode1;
				}
				else
				{
					u8_led_mode++;
				}
				if(u8_led_mode>2)
				{
					u8_led_mode=0;
				}
				u8_led_mode1=0;
				u8_led_step=0;
			}
			else
			{
				u8_led_mode1=u8_led_mode;
				u8_led_mode=0;
				u8_led_step=0;
				b_led_shutdown_flag=0;
			}	
			eeprom_wirte(1,u8_led_mode);
		}
		if(u8_led_mode==0)
		{
			LED_TOP=0;
			LED_FRONT=0;	
		}
		else if(u8_led_mode==1)
		{
			LED_TOP=1;
			LED_FRONT=0;	
		}
		else if(u8_led_mode==2)
		{
			LED_TOP=0;
			LED_FRONT=1;	
		}
		
		if(u8_tm_halt_250ms>=20)
		{
			b_halt_flag=1;
			u8_tm_halt_250ms=0;
		}
			
		if(b_halt_flag==1)
		{
			system_csh();
			_intc0	=0x00;
			_tbc	=0x00;
			_wdtc	=0b10101111;
			_nop();
			_nop();
			_halt();
			_nop();
			_nop();
			system_csh();
			b_halt_flag=0;
		}
		
////////////////////////////LED//////////////////////////////////////////////////////////////////////////
////////////////////////////LED//////////////////////////////////////////////////////////////////////////
		
		
		
		
	}
		
}
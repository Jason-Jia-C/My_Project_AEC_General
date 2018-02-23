	/**

	             ___________________________
	         	 |PB0/STP1_1       PWML/PC4| LED_DIM
	       		 |PB1		       PWMH/PC5| 
	     LED_SHDN|PB2        			VDD| +VCC
	       LED_SN|PB3        			VSS| -VSS
	             |PA5		   		AN6/PA6| IREF1
	       	 KEY1|PA4   			AN7/PA7| 
	             |PA3        		AN0/PA0| 
	       	   	 |PA2		   		AN1/PA1| 
	             ----------------------------

	
	*/
#include "HT45F3430.h"
#define bool	bit
#define uint8 	unsigned char 
#define uint16	unsigned int

#define KEY1			_pa4
#define IREF1			_pa6	
#define LED_DIM			_pc4	
#define LED_SN			_pb3
#define	LED_SHDN		_pb2

#define	ADC_BAT				0
#define	ADC_OCP				1

#define IOUT_1				0x02DA
#define IOUT_2				0x0280
#define IOUT_3				0x04CA
#define VIN_1				0x0700
#define VIN_2				0x077C
#define	VIN_3				0x077C
#define VIN_M				0x0686

#define VIN_LOW				0x061A
#define VIN_REC				0x07CD

/////////////////////////////////////////////////////////////
#pragma		vector		ocp_int		@ 0x04	
#pragma		vector		ovp_int		@ 0x08	
#pragma 	vector		ext0_int	@ 0x0C	
#pragma 	vector		mf_int		@ 0x10 
#pragma 	vector		adc_int		@ 0x18		//ADC
#pragma		vector		eeprom_int	@ 0x1C		//EEPROM
#pragma 	vector		tb0_int		@ 0x20		//Time Base 0
#pragma 	vector		tb1_int		@ 0x24		//Time Base 1
#pragma 	vector		ext1_int	@ 0x28	
/////////////////////////////////////////////////////////////
void	ocp_int			(void){}
void	ovp_int			(void){}
void	ext0_int		(void){_int0e = 0; _int0f = 0;}
void	mf_int			(void){_mfe = 0; _mff = 0;}
void    adc_int		    (void){_ade = 0;}
void	eeprom_int		(void){_dee = 0;}
void	tb1_int			(void){_tb1e = 0;}
void 	ext1_int		(void){_int1e = 0; _int1f = 0;}
//////////////////////////////////////////////////////////////
#pragma rambank0
//Halt
	bool	b_halt_flag;
	uint8	u8_tm_halt_100ms_cnt;
//TM
	bool	b_tm_1ms_flag;
	bool	b_tm_10ms_flag;
	bool	b_tm_100ms_flag;
	bool	b_tm_1s_flag;

	uint8	u8_tm_1ms_cnt;
	uint8	u8_tm_10ms_cnt;
	uint8	u8_tm_100ms_cnt;
	

//LED
	bool	b_led_sw_flag;
	bool	b_led_pwm_flag;
	bool	b_tm_led_20ms_flag;
	bool	b_led_shutdown_flag;

	uint8	u8_led_mode;
	uint8	u8_led_mode1;
	uint8   u8_led_step;
	uint8	u8_tm_led_1ms_cnt;
	
	//PWM
	uint8	u8_pwm_add_cnt;
	uint8	u8_pwm_add_ref;
	uint8	u8_pwm_cut_cnt;
	uint8	u8_pwm_cut_ref;
	uint8	u8_pwm_duty_cycle;
	//pre
	bool	b_pwm_pre_flag;
	uint8	u8_pwm_pre_var;
	//flash
	bool	b_led_flash_flag;
	bool	b_bat_low_flag;
	bool	b_tm_bat_low_1s_flag;
	bool	b_led_flash_two_flag;
	
	
	uint8	u8_bat_low_cnt;
	uint8	u8_tm_flash_100ms_cnt;
	uint8	u8_bat_rec_cnt;
	uint8	u8_led_flash_cnt;
	uint8	u8_tm_bat_low_1s_cnt;
//IFR	
	bool	b_ifr_sw_flag;
	bool	b_ifr_res_flag;
	bool	b_ifr_con_led_flag;
	bool	b_tm_ifr_1ms_flag;
	
	uint8	u8_ifr_step;
	uint8	u8_tm_ifr_1ms;
	uint8	u8_tm_ifr_10ms;

//ADC
	bool	b_adc_cal_flag;
	bool	b_adc_mode;

	bool	b_adc_bat_final_flag;
	uint8	u8_adc_bat_cnt1;
	uint8	u8_adc_bat_cnt2;
	uint16	u16_adc_bat_vdd;
	uint16	u16_adc_bat_sum1;
	uint16	u16_adc_bat_sum2;
	uint16	u16_adc_bat_max;
	uint16	u16_adc_bat_min;
	uint16	u16_adc_bat_final;

	bool	b_adc_ocp_final_flag;
	uint8	u8_adc_ocp_cnt1;
	uint8	u8_adc_ocp_cnt2;
	uint8	u8_adc_ocp_cnt3;
	uint16	u16_adc_ocp_vdd;
	uint16	u16_adc_ocp_sum1;
	uint16	u16_adc_ocp_sum2;
	uint16	u16_adc_ocp_sum3;
	uint16	u16_adc_ocp_max;
	uint16	u16_adc_ocp_min;
	uint16	u16_adc_ocp_final;

//KEY
	bool 	b_key1;		
	bool 	b_key_trg1;
	bool 	b_key_date1;
	bool 	b_key_cont1;	
	bool 	b_key1_flag;	
	uint8 	u8_tm_key1_1ms;
	
	bool 	b_key2;		
	bool 	b_key_trg2;
	bool 	b_key_date2;
	bool 	b_key_cont2;	
	bool 	b_key2_flag;	
	uint8 	u8_tm_key2_1ms;
		
	uint8 	u8_tm_key1_100ms_cnt;
	
	
//SP
	bool 	b_sp_flag;
	uint8	u8_sp_value[11];
	uint8 	u8_tm_sp_10ms;
	uint8 	u8_sp_cnt;
	uint16	u16_sp_buf_value;




#pragma norambank
void Var_Con()
{
	//Halt
	u8_tm_halt_100ms_cnt=0;
//TM
	b_tm_1ms_flag=0;
	b_tm_10ms_flag=0;
	b_tm_100ms_flag=0;
	b_tm_1s_flag=0;

	u8_tm_1ms_cnt=0;
	u8_tm_10ms_cnt=0;
	u8_tm_100ms_cnt=0;
//LED
	b_led_sw_flag=0;
	b_led_pwm_flag=0;
	b_tm_led_20ms_flag=0;
	b_led_shutdown_flag=0;

	u8_led_mode=0;
	u8_led_mode1=0;
	u8_led_step=0;
	u8_tm_led_1ms_cnt=0;
//IFR
	b_ifr_sw_flag=0;
	b_ifr_res_flag=0;
	b_ifr_con_led_flag=0;
	b_tm_ifr_1ms_flag=0;

	u8_ifr_step=0;
	u8_tm_ifr_1ms=0;


	
	//PWM
	u8_pwm_add_cnt=0;
	u8_pwm_add_ref=0;
	u8_pwm_cut_cnt=0;
	u8_pwm_cut_ref=0;
	u8_pwm_duty_cycle;
	//pre
	b_pwm_pre_flag=0;
	u8_pwm_pre_var=0;
	//flash
	b_led_flash_flag=0;
	b_bat_low_flag=0;
	b_tm_bat_low_1s_flag=0;
	b_led_flash_two_flag=0;
	
	u8_bat_low_cnt=0;
	u8_tm_flash_100ms_cnt=0;
	u8_bat_rec_cnt=0;
	u8_led_flash_cnt=0;
	u8_tm_bat_low_1s_cnt=0;

//ADC
	b_adc_cal_flag=0;
	b_adc_mode=0;

	b_adc_bat_final_flag=0;
	u8_adc_bat_cnt1=0;
	u8_adc_bat_cnt2=0;
	u16_adc_bat_vdd=0;
	u16_adc_bat_sum1=0;
	u16_adc_bat_sum2=0;
	u16_adc_bat_max=0;
	u16_adc_bat_min=0xFFFF;
	u16_adc_bat_final=0;

	b_adc_ocp_final_flag=0;
	u8_adc_ocp_cnt1=0;
	u8_adc_ocp_cnt2=0;
	u8_adc_ocp_cnt3=0;
	u16_adc_ocp_vdd=0;
	u16_adc_ocp_sum1=0;
	u16_adc_ocp_sum2=0;
	u16_adc_ocp_sum3=0;
	u16_adc_ocp_max=0;
	u16_adc_ocp_min=0xFFFF;
	u16_adc_ocp_final=0;

//KEY
 	b_key1=0;
 	b_key_trg1=0;
 	b_key_date1=0;
 	b_key_cont1=0;	
 	b_key1_flag=0;
 	u8_tm_key1_1ms=0;
  	b_key2=0;		
 	b_key_trg2=0;
 	b_key_date2=0;
 	b_key_cont2=0;	
 	b_key2_flag=0;	
 	u8_tm_key2_1ms=0;
 	u8_tm_key1_100ms_cnt=0;
	
//SP
	b_sp_flag=0;
	u8_tm_sp_10ms=0;
	u8_sp_cnt=0;
	u16_sp_buf_value=0;
	
}

void Seg_Con()	
{
	/*
		Clock
	*/
    _smod	= 0b00000001;	//Fsys=Fh	
    _smod1 &= 0b00000100;	
    _wdtc	= 0b01010111;
    /*
    	Interrupt
    */
    if(b_halt_flag==0)
    {
	    _integ	= 0b00000000;	
	    _intc0	= 0b00000001;	//Open Interrupt		
	    _intc1	= 0b00000000;
	    _intc2	= 0b00000001;	//Open Time0
	    _mfi	= 0b00000000;
	    _tbc	= 0b11000011;	//1ms	
    }
    else
    {
	    _integ	= 0b00000000;	
	    _intc0	= 0b00000000;	//Open Interrupt		
	    _intc1	= 0b00000000;
	    _intc2	= 0b00000000;	//Open Time0
	    _mfi	= 0b00000000;
	    _tbc	= 0b00000000;	//1ms	
    }
    /*
    	TM
    */
    _stmc0	= 0b00000000;
    _stmc1	= 0b00000000;
    /*
    	ADC
    */
    _sadc0	= 0b00000000;
    _sadc1	= 0b00000000;
    _sadc2	= 0b00000000;
    /*
    	OCP&OVP
    */
    _ocpc0	= 0b00000000;
    _ocpc1	= 0b00000000;
    _ocpocal= 0b00000000;
    _ocpccal= 0b00000000;
    _ovpc0	= 0b00000000;
    _ovpc1	= 0b00000000;
    _ovpda	= 0b00000000;
    /*
    	PWM
    */
    _pwmp	= 0b00000000;
    _pwmd	= 0b00000000;
    _dll	= 0b00000000;
    _cpr	= 0b00000000;
    _cpor	= 0b00000000;
        
    _ascr	= 0b00000000;	//
    _ovpc0	= 0b00000000;	//
    _ovpc1	= 0b00000000;
    _ovpda	= 0b00000000;
}
/*
#define KEY1			_pa4
#define	KEY2			_pa5

#define IFR_Send		_pb0	
#define IFR_Receive 	_pb1
#define IFR_LED			_pb2

#define IREF1			_pa6	
#define IREF2			_pa1	

#define LED_PWM_OUT1	_pc4	
#define LED_PWM_OUT2	_pc5

#define KEY1			_pa4
#define IREF1			_pa6		
#define LED_DIM			_pc4			//PWM信号输入脚，高电平工作，低电平停止	
#define LED_SN			_pb3			//8050控制脚，高电平开启			
#define	LED_SHDN		_pb2			//5815关断脚，高电平关闭，低电平开启

*/
void IO_Con()
{
	if(b_halt_flag==1)
	{	
	}
	else
	{
	}
	_pas0	=0b00000000;	
	_pas1	=0b00000000;	
	_pa		=0b00010000;	
	_pac	=0b00010010;	
	_papu	=0b00010000;	
	_pawu	=0b00010000;	
	_pa		=0b00010000;

	_pbs0	=0b00000000;	
    _pb		=0b00000100;
    _pbc	=0b00000000;	
    _pbpu	=0b00000000;
    _pb		=0b00000100;

	_pcs0	=0b00000000;
    _pc		=0b00000000;
    _pcc	=0b00000000;
    _pcpu	=0b00000000;
    _pc		=0b00000000;
}
#if 0
#define LED_DIM			_pc4			//PWM信号输入脚，高电平工作，低电平停止	
#define LED_SN			_pb3			//8050控制脚，高电平开启			
#define	LED_SHDN		_pb2			//5815关断脚，高电平关闭，低电平开启
#endif
void PWM_Drive()
{
	if(u8_led_mode==0)
	{
		_pcs0	=	0b00000000;
		LED_DIM=0;
		LED_SHDN=1;
		LED_SN=0;	
	}
	if(u8_led_mode==1)
	{
		LED_SN=1;
		_nop();
		LED_SHDN=0;
		_nop();
		LED_DIM=1;
		_nop();
	}
	else if(u8_led_mode>1)
	{
		_pcs0	=	0b00010000;
		_cpr	= 	0B00001000;		//<7>DLLLKEN; <6>DLLEN; <5:4>PWMCK; <3>PWMON; <2:0>DT
		_cpor	=	0b11110000;
		_pwmp	=	0xFF;
		_pwmd	=	0x80;
		_dll	=	0x00;
	}
}
void tb0_int()
{
	b_tm_1ms_flag=1;
#if 0
    if(u16_sp_buf_value > 0)	
    {
        _pb3 = (u16_sp_buf_value & 0x01);
        u16_sp_buf_value >>= 1;
    }
    else
    {
        b_sp_flag = 0;
        _pb3=1;
    }	
#endif
}
void Tm_Con()
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
		u8_tm_key2_1ms++;
		u8_tm_led_1ms_cnt++;
		if(u8_tm_led_1ms_cnt>19)
		{
			u8_tm_led_1ms_cnt=0;
			b_tm_led_20ms_flag=1;
		}	
		if(b_ifr_sw_flag==1)
		{
			if(u8_tm_ifr_1ms<255)
				u8_tm_ifr_1ms++;
		}
		else
			u8_tm_ifr_1ms=0;
		b_tm_ifr_1ms_flag=1;
	}
	if(b_tm_10ms_flag==1)
	{
		b_tm_10ms_flag=0;	
		u8_tm_sp_10ms++;
		if(u8_ifr_step==4||u8_ifr_step==5)
			u8_tm_ifr_10ms++;
		else
			u8_tm_ifr_10ms=0;
	}
	if(b_tm_100ms_flag==1)
	{
		b_tm_100ms_flag=0;
		u8_tm_halt_100ms_cnt++;
		if(b_led_shutdown_flag==0)
		{
			if(b_led_sw_flag==1&&b_key_cont1==0)
			{
				u8_tm_key1_100ms_cnt++;
			}	
			else
				u8_tm_key1_100ms_cnt=0;
		}
		else
		{
			u8_tm_key1_100ms_cnt=0;	
		}
		if(b_led_flash_flag==1&&b_bat_low_flag==1)
		{
			if(u8_led_flash_cnt<30)
			{
				u8_tm_flash_100ms_cnt++;
				if(u8_tm_flash_100ms_cnt<=5)
					_pwmd=0;
				else if(u8_tm_flash_100ms_cnt<=9)
					_pwmd=u8_pwm_duty_cycle;	
				else
				{
					u8_tm_flash_100ms_cnt=0;
					if(b_led_flash_two_flag==0)
 						u8_led_flash_cnt++;	
					else
						u8_led_flash_cnt+=10;
				}
			}
		}
		else
			u8_tm_flash_100ms_cnt=0;

	}
	if(b_tm_1s_flag==1)
	{
		b_tm_1s_flag=0;
		b_tm_bat_low_1s_flag=1;
		if(b_led_flash_flag==1&&b_bat_low_flag==1)
		{
			if(u8_led_flash_cnt>29)
			{
				_pwmd=u8_pwm_duty_cycle;
				u8_tm_bat_low_1s_cnt++;
				if(u8_tm_bat_low_1s_cnt>60)
				{
					u8_tm_bat_low_1s_cnt=0;	
					u8_led_flash_cnt=0;
					b_led_flash_two_flag=1;
				}
			}
		}
		else
		{
			u8_tm_bat_low_1s_cnt=0;
			u8_led_flash_cnt=0;
			b_led_flash_two_flag=0;
		}
	}
}
void main()
{
	if(_pdf==1)
		b_halt_flag=1;
	else 
		b_halt_flag=0;
	Seg_Con();
	IO_Con();
	if(b_halt_flag==0)
		Var_Con();
		u8_led_mode=0;
		PWM_Drive();
	while(1)
	{
		_clrwdt();
/*		if(b_halt_flag==1)
		{
			Seg_Con();
			IO_Con();	
			_nop();
			_nop();
			_halt();
			_nop();
			_nop();
			b_halt_flag=0;
			Seg_Con();
			IO_Con();	
		}
		if(b_led_sw_flag==1||b_ifr_sw_flag==1)
		{
			u8_tm_halt_100ms_cnt=0;
		}
		if(u8_tm_halt_100ms_cnt>=50)
		{
			b_halt_flag=1;
			u8_tm_halt_100ms_cnt=0;
		}
*/
		Tm_Con();
		if(KEY1!=b_key1)
		{
			if(u8_tm_key1_1ms>35)
			{
				b_key1=KEY1;
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
		if(b_key_cont1==1&&b_key_trg1==1)
		{
				u8_led_mode++;
				if(u8_led_mode>2)
					u8_led_mode=0;
					PWM_Drive();
		}
		
	}
}



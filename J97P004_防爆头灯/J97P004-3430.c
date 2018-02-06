	/**

	             ___________________________
	         Send|PB0/STP1_1       PWML/PC4| PWM2
	      Receive|PB1		       PWMH/PC5| PWM1
	          LED|PB2        			VDD| +VCC
	             |PB3        			VSS| -VSS
	         KEY2|PA5		   		AN6/PA6| IREF1
	       	 KEY1|PA4   			AN7/PA7| 
	             |PA3        		AN0/PA0| 
	       	   	 |PA2		   		AN1/PA1| IREF2
	             ----------------------------

	
	*/

	#include "HT45F3430.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int
	
	#define IFR_Send		_pb0	
	#define IFR_Receive 	_pb1
	#define IFR_LED			_pb2
	
	#define IREF1			_pa6	
	#define IREF2			_pa1	
	
	#define LED_PWM_OUT1	_pc5	
	#define LED_PWM_OUT2	_pc4	
	
	
	#define KEY1			_pa4	
	#define KEY2			_pa5	
	
	#define	OCP_ADC				0
	#define	V5V_ADC				1
	
	#define LED_NO_BRIGHT 		0		
	#define LED_TOP_BRIGHT 		1		
	#define LED_FRONT_WEAK 		2		
	#define LED_FRONT_STRONG 	3		

	#define	PWM_TOP				150
	#define	PWM_WEAK			112			
	#define	PWM_STRONG			220
	
	#define Volt_TOP			2030	
	#define	Volt_WEAK			1570	
	#define Volt_STRONG			3000				//3000	
	
	#define	Low_Volt_Turn_Off	2580		//2.62V
	
	#define OPEN				1
	#define CLOSE 				0
		
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
	
	#pragma rambank0
	
	//HALT
	bool 	b_halt_flag;
	uint8 	u8_tm_halt_25ms;
	
	//
	bool 	b_tm_1ms_flag;
	bool 	b_tm_25ms_flag;
	bool 	b_tm_200ms_flag;
	bool 	b_tm_1s_flag;
	

	uint8 	u8_tm_1ms;
	uint8 	u8_tm_25ms;
	uint8	u8_tm_200ms;
	
	//
	bool 	b_IFR_open_flag;			
	bool 	b_IFR_Receive_flag; 			
	bool	b_IFR_LED_flag;
	uint8 	u8_IFR_step;
	uint8 	u8_tm_IFR_1ms; 
	
	//ADC
	uint8	u8_ADC_MODE;
	
	uint8	u8_adc_cnt_OCP;	
	uint16	u16_adc_vdd_OCP;
	uint16	u16_adc_sum_OCP;
	uint16	u16_adc_max_OCP;
	uint16	u16_adc_min_OCP;
	uint16	u16_adc_final_OCP; 

	uint8	u8_adc_cnt_V5V;	
	uint16	u16_adc_vdd_V5V;
	uint16	u16_adc_sum_V5V;
	uint16	u16_adc_max_V5V;
	uint16	u16_adc_min_V5V;
	uint16	u16_adc_final_V5V; 
	
	uint8	u8_LowV_cnt;	
	
	
	//SP
	bool 	b_sp_flag;
	uint8	u8_sp_value[8];
	uint8 	u8_tm_sp_25ms;
	uint8 	u8_sp_cnt;
	uint16	u16_sp_buf_value;
	
	//KEY
	bool 	b_key1;
	bool 	b_key2;
	bool 	b_key_errors;
	
	bool 	b_key_trg1;
	bool 	b_key_date1;
	bool 	b_key_cont1;
	bool 	b_key_trg2;
	bool 	b_key_date2;
	bool 	b_key_cont2;
	
	bool 	b_key1_flag;
	bool 	b_key2_flag;
	
	uint8 	u8_tm_key_check_1ms1;
	uint8	u8_tm_key_check_1ms2;
	uint8 	u8_tm_key_25ms;
	
	//LED
	bool 	b_light_flag;
	bool 	b_led_set_flag;
	bool 	b_led_IFR_flag;
	bool 	b_led_shutdown_flag;
	bool	b_pwm_adj_flag;
	
	uint8 	u8_pwm_value;
	uint8	u8_dll_value;
	uint8 	u8_led_mode;
	uint8	u8_led_mode1;
	uint8 	u8_led_step;	

	
	
	bool	b_volt_adj_flag;
	uint8	u8_volt_adj_step;
	uint8	u8_tm_volt_adj_25ms;
	uint8	u8_volt_adj_mode;
	uint8	u8_volt_adj_high_value;
	uint8	u8_volt_adj_low_value;
	
	
	
	bool	b_pwm_adj_finish_flag;
	uint8	u8_pwm_adj_fast_cnt;
	uint8 	u8_pwm_adj_slow_cnt;
	uint8	u8_pwm_add_cnt;
	uint8	u8_pwm_cut_cnt;
	uint16	u16_Volt_value;
	uint16	u16_adc_DR_assign1;
	uint16	u16_adc_DR_assign2;
	
	#pragma norambank
	
	void systemset();
	void systemhalt();
	void ioset();
	uint8 eeprom_read(uint8 u8_eerom_eea);
	void eeprom_wirte(uint8 u8_eerom_eea,uint8 u8_eeprom_eed);
	void adc_test_OCP();
	void adc_test_V5V();
	void adc_DR_assign();
	void stm_out(uint8 u8_mode);
	void led_contrl(uint8 u8_mode);
	void value_csh();
	
	void systemset()	
	{
		_smod	=0b00000001;	// 
		_smod1 &=0b00000100;	
		
		_integ	=0b00000000;	//
		_intc0	=0b00000001;	//
		_intc1	=0b00000000;
		_intc2	=0b00000001;	//
		_mfi	=0b00000000;
		_tbc	=0b11000011;	//
		_wdtc	=0b01010111;
			
		_ascr	=0b00000000;	//
		_ovpc0	=0b00000000;	//
		_ovpc1	=0b00000000;
		_ovpda	=0b00000000;
	}
	void systemhalt()
	{
		_smod	=0b00000001; 	// 
		_smod1 &=0b00000100;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		
		_integ	=0b00000000;	
		_intc0	=0b00000000; 	
		_intc1	=0b00000000;	
		_intc2	=0b00000000;
		_mfi	=0b00000000;	
		_tbc	=0B00000000;  

		
		_ascr	=0b00000000;	
		_ovpc0	=0b00000000;	
		_ovpc1	=0b00000000;
		_ovpda	=0b00000000; 	
	}
	void ioset()
	{
		_pas0	=0b00001100;	
		_pas1	=0b00110000;	
		
		_pbs0	=0b00000001;	
		_pcs0	=0b00000000;	
		
		_pa		=0b00110000;	
		_pac	=0b01110010;	
		_papu	=0b00110000;	
		_pawu	=0b00110000;	
		_pa		=0b00110000;
		
		_pb		=0b00000000;
		_pbc	=0b00000010;	
		_pbpu	=0b00000000;
		_pb		=0b00000000;
		
		_pc		=0b11111111;
		_pcc	=0b00000000;
		_pcpu	=0b00000000;
		_pc		=0b1111111;		
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
		_emi   = 0;			
		_iar1 |= 0x08;
		_iar1 |= 0x04;
		_emi   = 1;			
		while((_iar1&0x04) == 0x04)
		{
			_clrwdt();
		}
		_iar1  = 0;
		_bp    = 0;
	}
	void tb0_int()
	{
		b_tm_1ms_flag=1;
	/*	if(u16_sp_buf_value > 0)	
		{
			_pb3 = (u16_sp_buf_value & 0x01);
			u16_sp_buf_value >>= 1;
		}
		else
		{
			b_sp_flag = 0;
		}*/
	}
	void adc_test_OCP()
	{
		_sadc0	=0b01111111;
		_sadc1	=0b10000011;  
		_sadc2	=0b10011110; 
		_start	=0;
		_start	=1;
		_start	=0;
		while(_adbz);
		u16_adc_vdd_OCP=((uint16)_sadoh<<8)+(_sadol);
		if(u16_adc_vdd_OCP>=u16_adc_max_OCP)
		u16_adc_max_OCP=u16_adc_vdd_OCP;
		if(u16_adc_vdd_OCP<=u16_adc_min_OCP)
		u16_adc_min_OCP=u16_adc_vdd_OCP;
		u16_adc_sum_OCP+=u16_adc_vdd_OCP;
		u8_adc_cnt_OCP++;
		if(u8_adc_cnt_OCP>=10)
		{
			u16_adc_sum_OCP-=u16_adc_max_OCP;
			u16_adc_sum_OCP-=u16_adc_min_OCP;
			u16_adc_sum_OCP>>=3;
			u16_adc_final_OCP=u16_adc_sum_OCP;
			b_pwm_adj_flag=1;
			u8_adc_cnt_OCP=0;
			u16_adc_max_OCP=0;
			u16_adc_sum_OCP=0;	
			u16_adc_min_OCP=0xFFFF;
		}	
	}	
	void adc_test_V5V()
	{
		_sadc0	=0b01111111;
		_sadc1	=0b00100011;   //7-4 A/D VDD/2   2-0 Fsys/8
		_sadc2	=0b10011101;  //3-0 Vbg(1.04V)* 2 =2.08
		_start	=0;
		_start	=1;
		_start	=0;
		while(_adbz);
		u16_adc_vdd_V5V=((uint16)_sadoh<<8)+(_sadol);
	
		if(u16_adc_vdd_V5V>u16_adc_max_V5V)
			u16_adc_max_V5V=u16_adc_vdd_V5V;
			
		if(u16_adc_vdd_V5V<u16_adc_min_V5V)
		u16_adc_min_V5V=u16_adc_vdd_V5V;
		
		u16_adc_sum_V5V+=u16_adc_vdd_V5V;
		u8_adc_cnt_V5V++;	
		
		if(u8_adc_cnt_V5V>=10)
		{
			u16_adc_sum_V5V-=u16_adc_max_V5V;
			u16_adc_sum_V5V-=u16_adc_min_V5V;
			u16_adc_sum_V5V>>=3;
			u16_adc_final_V5V=u16_adc_sum_V5V;	
			u8_adc_cnt_V5V=0;
			u16_adc_max_V5V=0;
			u16_adc_sum_V5V=0;
			u16_adc_min_V5V=0xFFFF;
		}		
	}
	void stm_out(uint8 u8_mode)
	{
		if(u8_mode==OPEN)
		{
			_stmdl	=0x00;
			_stmdh	=0x00;
			_stmal	=0xD4;
			_stmah	=0x00;
			_stmc0	=0b00011001;	
			_stmc1	=0b10100011;	
		}
		else if(u8_mode==CLOSE)
		{
			_stmdl	=0x00;
			_stmdh	=0x00;
			_stmal	=0xD4;
			_stmah	=0x00;
			_stmc0	=0b00000000;	
			_stmc1	=0b00000000;			
		}
		
	}
	void led_set()
	{
		if(b_led_set_flag==0)
		{
			if(u8_led_mode==0)
			{
				_ifs0	&=0b00000000;
				_ocpc0	=0b00000000;		
				_ocpc1	=0b00000000;		
				_ocpda	=0b00000000;		
				_ocpocal=0b00000000;
				_ocpccal=0b00000000;

				_pcs0				=0x00;	
				_dll				=0x00;
				_cpr				=0x00;	
				_cpor				=0x00;
				_pwmp				=0x00;
				LED_PWM_OUT1		=1;
				LED_PWM_OUT2		=1;
			}
			else
			{
				_ocpc0	=0b01000000;		
				_ocpc1	=0b00001101;		//R2/R1=5
				_ocpda	=0b00000000;		
				_ocpocal=0b00000000;
				_ocpccal=0b00000000;//Vout=(1+R2/R1)*Vin
		/*		u8_adc_cnt_OCP=0;
				u16_adc_sum_OCP=0;
				u16_adc_vdd_OCP=0;
				u16_adc_max_OCP=0;
				u16_adc_min_OCP=0xFFFF;*/
				if(u8_led_mode==1)
				{
					_ifs0&=0b11110111;
					_pcs0				=0x10;					
					_dll				=0x00;
					_cpr				=0x28;		
					_cpor				=0x80;		
					_pwmp				=0xFA;		
				}
				else 
				{
					_ifs0|=0b00001000;	
					_pcs0				=0x20;			
					_dll				=0x00;
					_cpr				=0x28;		
					_cpor				=0xC0;			
					_pwmp				=0xFA;			
				}
			}
			b_led_set_flag=1;	
			b_pwm_adj_flag=1;
			u8_dll_value=0;
			u8_pwm_add_cnt=0;
			u8_pwm_cut_cnt=0;
			u8_pwm_adj_fast_cnt=0;
			u8_pwm_adj_slow_cnt=0;
		}
	}
	void adc_DR_assign()
	{
		u16_Volt_value=(uint16)eeprom_read(u8_led_mode);
		u16_Volt_value<<=8;
		u16_Volt_value+=(uint16)eeprom_read((u8_led_mode+3));
		if(u8_led_mode==1)
			u8_pwm_value=PWM_TOP;
		else if(u8_led_mode==2)
			u8_pwm_value=PWM_WEAK;
		else if(u8_led_mode==3)
			u8_pwm_value=PWM_STRONG;
		if(u16_Volt_value==0)
		{
			if(u8_led_mode==1)
				u16_Volt_value=Volt_TOP;
			else if(u8_led_mode==2)
				u16_Volt_value=Volt_WEAK;
			else if(u8_led_mode==3)
				u16_Volt_value=Volt_STRONG;
		}
		u16_adc_DR_assign1=u16_adc_final_V5V;
		if(b_light_flag==0)
			u16_adc_DR_assign2=3199;
		else
			u16_adc_DR_assign2=3140;
		if(u16_adc_DR_assign1>u16_adc_DR_assign2)
		{
			u16_adc_DR_assign1-=u16_adc_DR_assign2;
			u16_adc_DR_assign1>>=3;
			u16_adc_DR_assign2=(u8_pwm_value&0x00FF);
			if(u16_adc_DR_assign1<u16_adc_DR_assign2)
				u16_adc_DR_assign2-=u16_adc_DR_assign1;
			else
				u16_adc_DR_assign2=0;
			if(u8_led_mode==2)
			{
				if(u16_adc_DR_assign2<90)
					u16_adc_DR_assign2=90;	
			}
			else
			{
				if(u16_adc_DR_assign2<150)
					u16_adc_DR_assign2=150;
			}
		}
		else if(u16_adc_DR_assign1<u16_adc_DR_assign2)
		{
			u16_adc_DR_assign1=((u16_adc_DR_assign2-u16_adc_DR_assign1)>>3);
			u16_adc_DR_assign2=((u8_pwm_value&0x00FF)+u16_adc_DR_assign1);
			if(u8_led_mode==2)
			{
				if(u16_adc_DR_assign2>200)
					u16_adc_DR_assign2=200;	
			}
			else
			{
				if(u16_adc_DR_assign2>255)
					u16_adc_DR_assign2=255;	
			}
		}
		else
			u16_adc_DR_assign2=(u8_pwm_value&0x00FF);
		u8_pwm_value=(u16_adc_DR_assign2&0xFF);
	}
	void led_adj()
	{
		if(u8_led_mode>0&&b_pwm_adj_flag==1)
		{
			if(u16_adc_final_OCP<u16_Volt_value)	// OCP>V	
			{
				if(u8_pwm_cut_cnt>0)
				{
					u8_pwm_cut_cnt=0;
					u8_pwm_adj_fast_cnt=0;
					u8_pwm_adj_slow_cnt=0;
				}
				if(u16_adc_final_OCP<(u16_Volt_value-100))
				{
					u8_pwm_adj_fast_cnt++;
					if(u8_pwm_adj_slow_cnt>0)
						u8_pwm_adj_slow_cnt--;
				}
				if(u16_adc_final_OCP>(u16_Volt_value-30))
				{
					u8_pwm_adj_slow_cnt++;
					if(u8_pwm_adj_fast_cnt>0)
						u8_pwm_adj_fast_cnt--;
				}
				if(u8_pwm_adj_fast_cnt>3)
					u8_pwm_add_cnt++;
				if(u8_pwm_adj_fast_cnt>10)
					u8_pwm_add_cnt++;
				u8_pwm_add_cnt++;
			}
			else if(u16_adc_final_OCP>u16_Volt_value)
			{
				if(u8_pwm_add_cnt>0)
				{
					u8_pwm_add_cnt=0;
					u8_pwm_adj_fast_cnt=0;
					u8_pwm_adj_slow_cnt=0;
				}
				if(u16_adc_final_OCP>(u16_Volt_value+100))
				{
					u8_pwm_adj_fast_cnt++;
					if(u8_pwm_adj_slow_cnt>0)
						u8_pwm_adj_slow_cnt--;
				}
				if(u16_adc_final_OCP<(u16_Volt_value+30))
				{
					u8_pwm_adj_slow_cnt++;
					if(u8_pwm_adj_fast_cnt>0)
						u8_pwm_adj_fast_cnt--;
				}
				if(u8_pwm_adj_fast_cnt>3)
					u8_pwm_cut_cnt++;
				if(u8_pwm_adj_fast_cnt>10)
					u8_pwm_cut_cnt++;
				u8_pwm_cut_cnt++;
			}
			if(u8_pwm_add_cnt>20)
			{
				if(u8_pwm_adj_slow_cnt<7)
				{
					if(u8_led_mode==2)
					{
						if(u8_pwm_value<200)
							u8_pwm_value++;
						else
							u8_dll_value=15;
					}
					else
					{
						if(u8_pwm_value<255)
							u8_pwm_value++;
						else
							u8_dll_value=15;
					}
				}
				else
				{
					u8_dll_value++;
					if(u8_dll_value>15)
					{
						if(u8_led_mode==2)
						{
							if(u8_pwm_value<200)
							{
								u8_pwm_value++;
								u8_dll_value=0;
							}
							else
								u8_dll_value=15;
						}
						else
						{
							if(u8_pwm_value<255)
							{
								u8_pwm_value++;
								u8_dll_value=0;
							}
							else
								u8_dll_value=15;
						}
					}
				}
				b_pwm_adj_finish_flag=1;
			}
			if(u8_pwm_cut_cnt>20)
			{
				if(u8_pwm_adj_slow_cnt<7)
				{
					if(u8_led_mode==2)
					{
						if(u8_pwm_value>90)
							u8_pwm_value--;
						else
							u8_dll_value=0;
					}
					else
					{
						if(u8_pwm_value>150)
							u8_pwm_value--;
						else
							u8_dll_value=0;
					}		
				}
				else
				{
					u8_dll_value--;
					if(u8_dll_value==0)
					{
						if(u8_led_mode==2)
						{
							if(u8_pwm_value>90)
							{
								u8_pwm_value--;
								u8_dll_value=15;
							}
							else
								u8_dll_value=1;
						}
						else
						{
							if(u8_pwm_value>150)
							{
								u8_pwm_value--;
								u8_dll_value=15;
							}
							else
								u8_dll_value=1;
						}
					}
				}
				b_pwm_adj_finish_flag=1;
			}
			if(b_pwm_adj_finish_flag==1)
			{
				u8_pwm_add_cnt=0;
				u8_pwm_cut_cnt=0;
				u8_pwm_adj_fast_cnt=0;
				u8_pwm_adj_slow_cnt=0;
				b_pwm_adj_finish_flag=0;
				_pwmd	=u8_pwm_value;
				_dll	=u8_dll_value;
			}
		}
	}
	void volt_adj()
	{
		if(b_key_errors==1&&u8_volt_adj_step==0&&b_volt_adj_flag==0)
		{
			u8_volt_adj_step=1;
		}
		if(u8_volt_adj_step==1)
		{
			if(b_key_errors==1)
			{
				if(u8_tm_volt_adj_25ms>79)		
					u8_volt_adj_step=2;
			}
			else																			
			{
				u8_tm_volt_adj_25ms=0;
				u8_volt_adj_step=0;
			}
		}
		else if(u8_volt_adj_step==2)			
		{
			if(b_key_errors==0)					
				u8_volt_adj_step=5;
			u8_tm_volt_adj_25ms=0;
		}
		else if(u8_volt_adj_step==5)
		{
			if(b_key_cont1==0&&b_key_cont2==0)
			{
				if(u8_tm_volt_adj_25ms>120)		
				{
					u8_volt_adj_step=0;
					u8_tm_volt_adj_25ms=0;
				}
			}
			else 
			{
				u8_volt_adj_step=6;
				u8_tm_volt_adj_25ms=0;
			}
		}
		else if(u8_volt_adj_step==6)
		{
			if(b_key_cont1==0&&b_key_cont2==0)
			{
				if(u8_tm_volt_adj_25ms>120)		
				{
					u8_volt_adj_step=0;
					u8_tm_volt_adj_25ms=0;
					u8_volt_adj_high_value=((u16_Volt_value>>8)&0x0F);
					u8_volt_adj_low_value =(u16_Volt_value&0xFF);
					eeprom_wirte(u8_led_mode+3,u8_volt_adj_low_value);
					eeprom_wirte((u8_led_mode),u8_volt_adj_high_value);	
				}
			}
			else
			{
				if(b_key_cont1==1&&b_key_trg1==1)
				{
					if(u16_Volt_value<4073)
						u16_Volt_value+=20;
					else if(u16_Volt_value<4083)
						u16_Volt_value+=10;
					else
						u16_Volt_value=4095;
				}
				if(b_key_cont2==1&&b_key_trg2==1)
				{
					if(u16_Volt_value>20)
						u16_Volt_value-=10;
				}
				if(b_key_errors==1)
				{
					if(u8_led_mode==1)
						u16_Volt_value=Volt_TOP;
					else if(u8_led_mode==2)
						u16_Volt_value=Volt_WEAK;
					else if(u8_led_mode==3)
						u16_Volt_value=Volt_STRONG;
				}
				u8_tm_volt_adj_25ms=0;
			}	
		}
	}
	void value_csh()
	{
	//HALT
		b_halt_flag=0;
		u8_tm_halt_25ms=0;
	//TM
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		b_tm_200ms_flag=0;
		b_tm_1s_flag=0;
	
		u8_tm_1ms=0;
		u8_tm_25ms=0;
		u8_tm_200ms=0;
	//IFR
		b_IFR_open_flag=0;				
		b_IFR_Receive_flag=0; 				
		b_IFR_LED_flag=0;
		u8_IFR_step=0;
		u8_tm_IFR_1ms=0; 
	//ADC
		u8_ADC_MODE=0;
		u8_adc_cnt_OCP=0;	
		u16_adc_vdd_OCP=0;
		u16_adc_sum_OCP=0;
		u16_adc_max_OCP=0;
		u16_adc_min_OCP=0xFFFF;
		u16_adc_final_OCP=0; 

		u8_adc_cnt_V5V=0;	
		u16_adc_vdd_V5V=0;
		u16_adc_sum_V5V=0;
		u16_adc_max_V5V=0;
		u16_adc_min_V5V=0xFFFF;
		u16_adc_final_V5V=0; 
		
		u8_LowV_cnt=0;
	//SP
		b_sp_flag=0;
	//	u8_sp_value[6];
		u8_tm_sp_25ms=0;
		u8_sp_cnt=0;
		u16_sp_buf_value=0;
	//KEY
		b_key1=1;
		b_key2=1;
		b_key_errors=0;
	
		b_key_trg1=0;
		b_key_date1=0;
		b_key_cont1=0;
		b_key_trg2=0;
		b_key_date2=0;
		b_key_cont2=0;
		b_key1_flag=0;
		b_key2_flag=0;
		
		u8_tm_key_check_1ms1=0;
		u8_tm_key_check_1ms2=0;
		u8_tm_key_25ms=0;
	//LED
		b_light_flag=0;
		b_led_set_flag=0;
		b_led_IFR_flag=0;
		b_led_shutdown_flag=0;
		b_pwm_adj_flag=0;
		u8_pwm_value=0;
		u8_dll_value=0;
		u8_led_mode=0;
		u8_led_mode1=0;
		u8_led_step=0;	
		u16_Volt_value=0;
		
		b_volt_adj_flag=0;
		u8_volt_adj_step=0;
		u8_tm_volt_adj_25ms=0;
		u8_volt_adj_mode=0;
		u8_volt_adj_high_value=0;
		u8_volt_adj_low_value=0;
	
	}
	void main()
	{
		if(_pdf==1)
		{
			b_halt_flag=1;
		}
		else
		{
			systemset();
			ioset();
			value_csh();	
		}
		if(_lvrf==1)
		{
			u8_led_mode=eeprom_read(0);
			u8_led_step=10;
			_lvrf=0;
		}
		while(1)
		{
			
			_clrwdt();		
			if(u8_led_mode>0)
			b_light_flag=1;
			else
			b_light_flag=0;
			if(b_volt_adj_flag==0)
			{
				b_volt_adj_flag=eeprom_read(7);
			}
		
			if(b_light_flag==1||b_IFR_open_flag==1)
			{
				u8_tm_halt_25ms=0;
			}
			if(u8_tm_halt_25ms>=200)
			{
				b_halt_flag=1;
			}
	//		if(b_halt_flag==1)
	//			b_halt_flag=0;
			if(b_halt_flag==1)
			{
				systemhalt();
				ioset();	
				_nop();
				_nop();
				_halt();
				_nop();
				_nop();
				systemset();
				ioset();
				u8_tm_halt_25ms=0;
				b_halt_flag=0;
			}
////////////////////////////TIME//////////////////////////////////
////////////////////////////TIME//////////////////////////////////
			if(b_tm_1ms_flag==1)
			{
				u8_tm_1ms++;
				if(u8_tm_1ms>=25)
				{
					u8_tm_1ms=0;
					u8_tm_25ms++;
					b_tm_25ms_flag=1;
					if(u8_tm_25ms>=8)
					{
						u8_tm_25ms=0;
						u8_tm_200ms++;
						b_tm_200ms_flag=1;
						if(u8_tm_200ms>=5)
						{
							u8_tm_200ms=0;
							b_tm_1s_flag=1;
						}
					}
				}
			}
			if(b_tm_1ms_flag==1)
			{
				b_tm_1ms_flag=0;
				if(b_IFR_open_flag==1&&u8_tm_IFR_1ms<255)
					u8_tm_IFR_1ms++;
				u8_tm_key_check_1ms1++;
				u8_tm_key_check_1ms2++;
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				
				if(b_light_flag==1&&b_IFR_open_flag==0&&u8_tm_key_25ms<255)
					u8_tm_key_25ms++;
				if(u8_volt_adj_step>0)
					u8_tm_volt_adj_25ms++;
				u8_tm_sp_25ms++;
				u8_tm_halt_25ms++;
			}
			if(b_tm_200ms_flag==1)
			{
				
				b_tm_200ms_flag=0;
			}
			if(b_tm_1s_flag==1)
			{
				b_tm_1s_flag=0;
			}
	
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////

////////////////////////////ADC//////////////////////////////////////////////////////////////////////////
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////
			if(u8_ADC_MODE==OCP_ADC)
			{
				adc_test_OCP();
				u8_ADC_MODE=V5V_ADC;
			}
			else if(u8_ADC_MODE==V5V_ADC)
			{
				adc_test_V5V();
				u8_ADC_MODE=OCP_ADC;
			}
			if(u16_adc_final_V5V<Low_Volt_Turn_Off&&u16_adc_final_V5V>0)
			{
				u8_LowV_cnt++;
				u16_adc_final_V5V=0;
			}
			if(u16_adc_final_V5V>Low_Volt_Turn_Off)
			{
				if(u8_LowV_cnt>0)
				{
					u16_adc_final_V5V=0;
					u8_LowV_cnt--;
				}
			}
			if(u8_LowV_cnt>10)
			{
				b_led_shutdown_flag=0;
				u8_led_mode=0;
				u8_LowV_cnt=0;
				b_halt_flag=1;	
			}
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////

////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////
			if(KEY1!=b_key1)
			{
				if(u8_tm_key_check_1ms1>25)
					b_key1=KEY1;
			}
			else
			{
				u8_tm_key_check_1ms1=0;
				b_key_date1=b_key1^0x01;
				b_key_trg1=b_key_date1&(b_key_date1^b_key_cont1);
				b_key_cont1=b_key_date1;
			}
			if(KEY2!=b_key2)
			{
				if(u8_tm_key_check_1ms2>25)
					b_key2=KEY2;
			}
			else
			{
				u8_tm_key_check_1ms2=0;
				b_key_date2=b_key2^0x01;
				b_key_trg2=b_key_date2&(b_key_date2^b_key_cont2);
				b_key_cont2=b_key_date2;
			}
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////

//////////////////////////////KEY1///////////////////////////////////////////////////////////////////////
			if(b_key_cont1==1&&b_key_cont2==1)
				b_key_errors=1;
			else if(b_key_cont1==0&&b_key_cont2==0)
				b_key_errors=0;
			volt_adj();
			if(b_key_errors==0&&u8_volt_adj_step==0)
			{
				if(b_key_cont1==1&&b_key_trg1==1)
				{
					b_key1_flag=1;
				}
				
				if(b_key_cont1==0&&b_key1_flag==1)
				{
					u8_led_step=1;
					b_key1_flag=0;
					u8_tm_key_25ms=0;
					if(b_IFR_open_flag==1)					
					{
						if(b_light_flag==1)					
						{
							b_IFR_open_flag=0;
							b_led_shutdown_flag=1;
						}
						else								
						{
							b_led_shutdown_flag=0;
							b_IFR_open_flag=0;
							u8_led_mode=LED_NO_BRIGHT;
						}
					}  	
				}
				else if(b_key_cont1==0&&b_volt_adj_flag==1)	
				{
					if(b_led_shutdown_flag==0)
	     			{
						if(u8_tm_key_25ms>=200)	
						{
							u8_tm_key_25ms=0;
							b_led_shutdown_flag=1;
							u8_led_mode1=u8_led_mode;
						}
			   		}
		 			else
					{
						u8_tm_key_25ms=0;
					}	
				}
//////////////////////////////KEY1/////////////////////////////////////////////////////////////////
//////////////////////////////KEY2/////////////////////////////////////////////////////////////////
				if(b_key_cont2==1&&b_key_trg2==1)
					b_key2_flag=1;
				if(b_key_cont2==0&&b_key2_flag==1)
				{
					b_key2_flag=0;
					b_IFR_open_flag=~b_IFR_open_flag;
					if(b_IFR_open_flag==1)				
					{
						if(b_light_flag==1)				
						{
							u8_led_step=2;
							b_led_IFR_flag=1;			
						}
						else
						{
							u8_led_step=2;
							b_led_IFR_flag=0;
						}
					}
					else								
					{
						u8_led_step=1; 
						b_led_shutdown_flag=1;
					}
				}
			}
			else if(b_key_errors==1)
			{
				b_key1_flag=0;
				b_key2_flag=0;	
			}
////////////////////////////KEY2////////////////////////////////////////////////////////////////////

////////////////////////////LED//////////////////////////////////////////////////////////////////////////	
////////////////////////////LED//////////////////////////////////////////////////////////////////////////	
			if(u8_led_step==1)
			{
				if(b_led_shutdown_flag==0)
				{ 
					if(u8_led_mode1>LED_NO_BRIGHT)
					{
						u8_led_mode=u8_led_mode1;
					}
					else
					{
						u8_led_mode++;
					}
					if(u8_led_mode>LED_FRONT_STRONG)
					{
						u8_led_mode=LED_NO_BRIGHT;
						if(b_volt_adj_flag==0)
						{
							b_volt_adj_flag=1;
							eeprom_wirte(7,b_volt_adj_flag);
						}
					}
					if(u8_led_mode>0)
						eeprom_wirte(0,u8_led_mode);
					u8_led_mode1=0;
					u8_led_step=10;
				}
				else
				{
					b_led_shutdown_flag=0;
				/*	if(b_volt_adj_flag==0)
					{
						b_volt_adj_flag=1;
						eeprom_wirte(7,b_volt_adj_flag);
					}*/
					u8_led_mode=LED_NO_BRIGHT;
					u8_led_step=10;
				}	
			}
			else if(u8_led_step==2)					
			{
				if(b_IFR_Receive_flag==1)				
				{
					if(b_light_flag==1)			
					{
						b_led_IFR_flag=0;
					}
					else						
					{
						b_led_IFR_flag=1;
					}
					b_IFR_Receive_flag=0;			
				}	
				if(b_led_IFR_flag==1)			
				{
					u8_led_mode=LED_FRONT_STRONG;	
				}
				else
				{
					u8_led_mode=LED_NO_BRIGHT;		
				}
				u8_led_step=10;
			}
			if(u8_led_step==10)						
			{	
				if(u8_led_mode>0)
				{
					adc_DR_assign();
				}
				b_led_set_flag=0;	
				b_pwm_adj_flag=1;
				u8_led_step=11;
			}
			if(u8_led_step==11)
			{
				led_set();
				led_adj();	
	//			led_contrl(u8_led_mode);			
			}
////////////////////////////LED//////////////////////////////////////////////////////////////////////////
////////////////////////////LED//////////////////////////////////////////////////////////////////////////
	
////////////////////////////IFR//////////////////////////////////////////////////////////////////////////
////////////////////////////IFR//////////////////////////////////////////////////////////////////////////
			if(b_IFR_open_flag==1)
			{
				if(b_IFR_LED_flag==0)
				{
					IFR_LED=1;
					stm_out(OPEN);
					b_IFR_LED_flag=1;
					u8_IFR_step=0;
					u8_tm_IFR_1ms=0;
				}
				if(u8_IFR_step==0)
				{
					if(IFR_Receive==1)
					{
						if(u8_tm_IFR_1ms>30)
							u8_IFR_step=1;
					}
					else
						u8_tm_IFR_1ms=0;
				}
				else if(u8_IFR_step==1)
				{
					if(IFR_Receive==0)
						u8_IFR_step=2;
					u8_tm_IFR_1ms=0;
				}
				else if(u8_IFR_step==2)
				{
					if(IFR_Receive==0)
					{
						if(u8_tm_IFR_1ms>22)
						{
							u8_IFR_step=3;
							u8_tm_IFR_1ms=0;
						}
					}
					else
					{
						u8_IFR_step=0;
						u8_tm_IFR_1ms=0;
					}
				}
				else if(u8_IFR_step==3)
				{
					if(IFR_Receive==0)
					{
						if(u8_tm_IFR_1ms>180)
						{
							u8_IFR_step=0;
							u8_tm_IFR_1ms=0;
						}
					}
					else
					{
						u8_IFR_step=4;
						u8_tm_IFR_1ms=0;
					}
				}
				else if(u8_IFR_step==4)
				{
					if(IFR_Receive==1)
					{
						if(u8_tm_IFR_1ms>10)
						{
							u8_IFR_step=5;
							u8_tm_IFR_1ms=0;
						}
					}
					else
					{
						u8_IFR_step=0;
						u8_tm_IFR_1ms=0;
					}
				}
				else if(u8_IFR_step==5)
				{
					u8_led_step=2;
					b_IFR_Receive_flag=1;	
					u8_IFR_step=0;
					u8_tm_IFR_1ms=0;
				}
			}
			else 
			{
				if(b_IFR_LED_flag==1)				
				{
					stm_out(CLOSE);	
					IFR_LED=0;
				}
				b_IFR_LED_flag=0;
				b_IFR_Receive_flag=0;
				u8_IFR_step=0;
				u8_tm_IFR_1ms=0;	
				
			}

	////////////////////////////IFR//////////////////////////////////////////////////////////////////////////
	////////////////////////////IFR//////////////////////////////////////////////////////////////////////////
	/*					
			if(u8_tm_sp_25ms>20)
			{
				u8_sp_value[0] = ((u16_adc_final_OCP>>8)&0xFF);
				
				u8_sp_value[1] = (u16_adc_final_OCP&0xFF);

				
				u8_sp_value[2] = ((u16_adc_final_V5V>>8)&0xFF);
				
				u8_sp_value[3] = (u16_adc_final_V5V&0xFF);
				
				
				u8_sp_value[4] = u8_pwm_value;
				
				u8_sp_value[5] = u8_dll_value;
				
				u8_sp_value[6] =((u16_Volt_value>>8)&0xFF);
				u8_sp_value[7] =(u16_Volt_value&0xFF);
				
				u8_tm_sp_25ms=0;
				u8_sp_cnt=0;
			}
			if(u8_sp_cnt<8&&b_sp_flag==0)
			{
				u16_sp_buf_value=(((uint16)u8_sp_value[u8_sp_cnt]<<1)|0x0200);
				u8_sp_cnt++;
				b_sp_flag=1;
			}	*/
		}
	//////////////////////////////////////////		
	}

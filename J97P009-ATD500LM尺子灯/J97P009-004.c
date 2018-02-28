/*
项目名称：ATD500LM尺子灯
芯片型号: HT66F004 内部8M 指令周期4分频2M
功能：
锂电池电压检测

	滑动变阻器 往上推 正面逐步变亮
	滑动变阻器 往下推 顶部逐步变亮
	滑动变阻器 在中间 所有灯不亮
	
	按键3按下 所有灯关闭
	按键2按下 顶部灯亮（直接到最亮）
	两个按键一起按下 顶部灯亮（直接到最亮）  
	
	按键的优先级高于滑动变阻器，当按键松开时，再响应滑动变阻器的控制


脚位定义
             ________________________
            |VSS                  VDD|
       		|PC0         PB0/INT0/AN0| KEY2(KEY1)按键1处关灯
       		|PC1         PB1/INT1/AN1| KEY3(KEY2)按键2处开顶部
     		|PC2        PB2/PTCK0/AN2| HD_KEY(1)滑动
     		|PA0/PTP0   PA4/PTCK1/AN3| 
        	|PA1/PTP0I   PA5/AN4/VREF| HD_KEY(2)接地
            |PA2        PA6/AN5/VREFO| LED_SN_TOP(5711CE)
         	|PA3/PTP1I   PA7/PTP1/AN6| LED_SN_FRONT(5711CE*2)
             ------------------------

*/	
	#include "HT66F004.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int 
	
	#define SP_FLAG 0
	
	#define KEY1	_pb1			//KEY2
	#define	KEY2	_pb0			//KEY3
	
	#define LED_SN_TOP		_pa6	//顶部SN			
	#define LED_SN_FRONT	_pa7	//正面SN
	
	#define	HD_1	_pb2			//滑动变阻器1
	#define HD_2	_pa5			//滑动变阻器2

	#define HALT_START_TM			200			//休眠计时，25ms为单位			5S
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	//Clock
	bool b_tm_1ms_flag;
	bool b_tm_25ms_flag;
	bool b_tm_200ms_flag;
	bool b_tm_1s_flag;
	
	uint8 u8_tm_1ms;
	uint8 u8_tm_25ms;
	uint8 u8_tm_200ms;	
	
	//KEY
	bool b_key1;
	bool b_key_trg1;
	bool b_key_date1;
	bool b_key_cont1;
	
	bool b_key2;
	bool b_key_trg2;
	bool b_key_date2;
	bool b_key_cont2;
	
	uint8 u8_tm_key1_1ms;
	uint8 u8_tm_key2_1ms;
	
	//LED
	uint8 u8_led_mode;
	uint8 u8_tm_tg_25ms;
	uint16 u16_tg_value;
	
	//ADC
	uint8 u8_adc_cnt;
	uint16 u16_adc_vdd;
	uint16 u16_adc_max;
	uint16 u16_adc_min;
	uint16 u16_adc_sum;
	uint16 u16_adc_final_value;
	
	uint8 u8_adc_cnt1;
	uint16 u16_adc_vdd1;
	uint16 u16_adc_max1;
	uint16 u16_adc_min1;
	uint16 u16_adc_sum1;
	
	uint16 u16_tg_value_var;
	bool b_tm_tg_1ms;
	uint8 u8_tg_add_cnt;
	uint8 u8_tg_cut_cnt;
	
#if (SP_FLAG==1)
	//SP
	bool 	b_sp_flag;
	uint8 	u8_tm_sp_25ms;
	uint8 	u8_sp_cnt;
	uint16	u16_sp_buf_value;
	uint8	u8_sp_value[5];
#endif		
	#pragma norambank
	
	/////////////////////////////////////////////////////////////
	#pragma 	vector		ext0_int	@ 0x04		//外部中断0
	#pragma 	vector		tb0_int		@ 0x08		//Time Base 0
	#pragma 	vector		tb1_int		@ 0x0C		//Time Base 1
	#pragma 	vector		mf0_int		@ 0x10 		//多功能0 stm中断
	#pragma		vector		eeprom_int	@ 0x14		//EEPROM
	#pragma 	vector		adc_int		@ 0x18		//AD
	#pragma 	vector		ext1_int	@ 0x1C		//外部中断1
	/////////////////////////////////////////////////////////////
	
	void	ext0_int		(void){_int0e = 0; _int0f = 0;}
	void	tb1_int			(void){_tb1e = 0;}
	void	mf0_int			(void){_mf0e = 0; _mf0f = 0;}
	void	eeprom_int		(void){_dee = 0;}
	void    adc_int		    (void){_ade = 0;}
	void 	ext1_int		(void){_int1e = 0; _int1f = 0;}

	
	void 	systemset();
	void 	systemhalt();
	void 	ioset();
	void 	value_csh();
	
	void systemset()	
	{
		_smod	= 0b00000001; 	// 系统时钟选择  FSYS=FH
		_smod1	= 0b00000000;	
		_sadc0	= 0B00000000;
		_sadc1	= 0B00000000;
		_sadc2	= 0B00000000;	
		_ptm0c0	= 0b00000000;	
		_ptm0c1 = 0b00000000;
		_ptm1c0	= 0b00000000;	
		_ptm1c1 = 0b00000000;
		_integ	= 0b00000000;	//INTN中断脚中断边沿控制，双沿触发 PB0脚为充电感应脚，当充电时触发INT0，唤醒单片机
		_intc0	= 0b00000101; 	//中断设置
		_intc1	= 0b00000000;	//中断设置
		_mfi0	= 0b00000000;	//多功能中断设置
		_tbc	= 0B11000011;  	//时基设置  tb0  976hz   1ms  
		_wdtc	= 0b01010111; 
	}

	void systemhalt()
	{
		_smod	= 0b00000001; 	// 系统时钟选择  FSYS=FH
		_smod1	= 0b00000000;	
		_sadc0	= 0B00000000;
		_sadc1	= 0B00000000;
		_sadc2	= 0B00000000;
		_ptm0c0	= 0b00000000;	
		_ptm0c1 = 0b00000000;		
		_ptm1c0	= 0b00000000;	
		_ptm1c1 = 0b00000000;	
		_integ	= 0b00001111;	//INTN中断脚中断边沿控制，上升沿触发 PB1脚为充电感应脚，当开灯时触发INT1，唤醒单片机
		_intc0	= 0b00000011; 	//中断设置
		_intc1	= 0b00001000;	//中断设置
		_mfi0	= 0b00000000;	//多功能中断设置
		_tbc	= 0B00000000;  	//时基设置
			//7(PTP) 6(LED_OUT1) 5(hd-2) 4() 3( ) 2( ) 1( ) 0()
		_pa		= 0b00000000;	
		_pac	= 0b00000000;	
		_papu	= 0b00000000;	
		_pawu	= 0b00000000;
		_pasr	= 0b00000000;	
		_pa		= 0b00000000;	

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(HD_1) 1(KEY2) 0(KEY1)
		_pb		= 0b00000011;	
		_pbc	= 0b00000011;			
		_pbpu	= 0b00000011;	
		_pbsr	= 0b00000000;	
		_pb		= 0b00000011;	

		//7( ) 6( ) 5( ) 4( ) 3( ) 2( ) 1( ) 0( )
		_pc		= 0b00000000;	//初始全为低电平
		_pcc	= 0b00000000;	//初始全为输出	
		_pcpu	= 0b00000000;	//全无上拉电阻
		_pc		= 0b00000000;	//初始全为低电平
		
		_wdtc	= 0b10101111; 	
	}

	void ioset()
	{
		//7(PTP) 6(LED_OUT1) 5(hd-2) 4() 3( ) 2( ) 1( ) 0()
		_pa		=0b00000000;	
		_pac	=0b00000000;	
		_papu	=0b00000000;	
		_pawu	=0b00000000;
		_pasr	=0b01000000;	
		_pa		=0b00000000;	

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(HD_1) 1(KEY2) 0(KEY1)
		_pb		=0b00000011;	
		_pbc	=0b00000111;			
		_pbpu	=0b00000011;	
		_pbsr	=0b00000100;	
		_pb		=0b00000011;	

		//7( ) 6( ) 5( ) 4( ) 3( ) 2( ) 1( ) 0( )
		_pc		=0b00000000;	//初始全为低电平
		_pcc	=0b00000000;	//初始全为输出	
		_pcpu	=0b00000000;	//全无上拉电阻
		_pc		=0b00000000;	//初始全为低电平
	}
	
/*	uint8 eeprom_read()
	{
		uint8	u8_eeprom_value;
		_eea   = 0x01;
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
		
		u8_eeprom_value = _eed;
		return u8_eeprom_value;
	}
	
	void eeprom_wirte(uint8	u8_mode)
	{
		_eea   = 0x01;
		_eed   = u8_mode;
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
	}*/
	
	void adc_test()
	{
		_sadc0	=0b01110010;
		_sadc1	=0b10000100;   //7-5 AN2   2-0 Fsys/16
		_sadc2	=0b11001100;  //3-0 Vbg*4
		_start	=0;
		_start	=1;
		_start	=0;
		while(_adbz);
		u16_adc_vdd=((uint16)_sadoh<<8)+(_sadol);
		if(u16_adc_vdd>=u16_adc_max)
		u16_adc_max=u16_adc_vdd;
		if(u16_adc_vdd<=u16_adc_min)
		u16_adc_min=u16_adc_vdd;
		u16_adc_sum+=u16_adc_vdd;
		u8_adc_cnt++;
		if(u8_adc_cnt>=10)
		{
			u16_adc_sum-=u16_adc_max;
			u16_adc_sum-=u16_adc_min;
			u16_adc_sum>>=3;
	//		u16_adc_final_value=u16_adc_sum;
			u16_adc_vdd1=u16_adc_sum;
			u8_adc_cnt=0;
			u16_adc_vdd=0;
			u16_adc_max=0;
			u16_adc_min=0xffff;
			u16_adc_sum=0;	
			if(u16_adc_vdd1>=u16_adc_max1)
			u16_adc_max1=u16_adc_vdd1;
			if(u16_adc_vdd1<=u16_adc_min1)
			u16_adc_min1=u16_adc_vdd1;
			u16_adc_sum1+=u16_adc_vdd1;
			u8_adc_cnt1++;
			if(u8_adc_cnt1>=10)
			{
				u16_adc_sum1-=u16_adc_max1;
				u16_adc_sum1-=u16_adc_min1;
				u16_adc_sum1>>=3;
				u16_adc_final_value=u16_adc_sum1;
		//		u16_adc_vdd1=u16_adc_sum;
				u8_adc_cnt1=0;
				u16_adc_vdd1=0;
				u16_adc_max1=0;
				u16_adc_min1=0xffff;
				u16_adc_sum1=0;		
			}
			
		}
		
	}
	
	void tb0_int()
	{
		b_tm_1ms_flag=1;
#if (SP_FLAG==1)
		if(u16_sp_buf_value > 0)				//波特率 970
		{
			_pc1 = (u16_sp_buf_value & 0x01);
			u16_sp_buf_value >>= 1;
		}
		else
		{
			b_sp_flag = 0;
		}
#endif
	}

	void PTP_contrl(bool b_PTP_flag)
	{
		if(b_PTP_flag==1)	//开始PTP输出
		{
			_ptm1c0	= 0b00001000;	//(7) 0开始 1暂停 （6-4）
			_ptm1c1	= 0b10101000;
			_ptm1dl = 0b00000000;
			_ptm1dh = 0b00000000;
			_ptm1rpl= 0b00111111;
			_ptm1rph= 0b00000011;	//周期设置为1023
//			_ptm1al	= 0b10010000;
//			_ptm1ah = 0b00000001;	//占空比暂设置为400
		}
		else				//关闭PTP输出
		{
			_ptm1c0	= 0b00000000;	
			_ptm1c1 = 0b00000000;
			_ptm1dl = 0b00000000;
			_ptm1dh = 0b00000000;
			_ptm1rpl= 0b00000000;
			_ptm1rph= 0b00000000;
			_ptm1al = 0b00000000;
			_ptm1ah = 0b00000000;
		}
	}
	
	void value_csh()
	{
		//HALT
		b_halt_flag=0;
		u8_tm_halt_25ms=0;
		
		//软件时钟
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		u8_tm_1ms=0;
	
		//KEY
		b_key1=0;
		b_key_trg1=0;
		b_key_date1=0;
		b_key_cont1=0;
		u8_tm_key1_1ms=0;
		
		b_key2=0;
		b_key_trg2=0;
		b_key_date2=0;
		b_key_cont2=0;
		u8_tm_key2_1ms=0;
	
		//LED
		u8_led_mode=0;	
		u8_tm_tg_25ms=0;
		//
		u8_adc_cnt=0;
		u16_adc_vdd=0;
		u16_adc_max=0;
		u16_adc_min=0xFFFF;
		u16_adc_sum=0;
		u16_adc_final_value=0;
		
		u8_adc_cnt1=0;
		u16_adc_vdd1=0;
		u16_adc_max1=0;
		u16_adc_min1=0xFFFF;
		u16_adc_sum1=0;
		u16_tg_value_var=0;
	}
	void LED_adj()
	{
		if(u8_led_mode==2)
		{
			if(b_tm_tg_1ms==1)
			{
				b_tm_tg_1ms=0;
				if(u16_adc_final_value<2048)
				{
					u16_tg_value=0;
				}
				else
				{
					u16_tg_value=(u16_adc_final_value-2048);
					u16_tg_value>>=1;
				}
				if(u16_tg_value<20)
				{
				//	u16_tg_value_var=0;
					u8_tg_cut_cnt=50;
				}
				else
				{
					if(u16_tg_value>(u16_tg_value_var+20))
					{
						u8_tg_add_cnt++;
						if((u16_tg_value-u16_tg_value_var)>500)
						{
							u8_tg_add_cnt+=50;
						}
						else if((u16_tg_value-u16_tg_value_var)>400)
						{
							u8_tg_add_cnt+=40;
						}
						else if((u16_tg_value-u16_tg_value_var)>300)
						{
							u8_tg_add_cnt+=30;
						}
						else if((u16_tg_value-u16_tg_value_var)>200)
						{
							u8_tg_add_cnt+=20;
						}
						else if((u16_tg_value-u16_tg_value_var)>100)
						{
							u8_tg_add_cnt+=10;
						}
						u8_tg_cut_cnt=0;	
					}
					else if(u16_tg_value_var>(u16_tg_value+20))
					{
						u8_tg_cut_cnt++;
						if((u16_tg_value_var-u16_tg_value)>500)
						{
							u8_tg_cut_cnt+=50;
						}
						else if((u16_tg_value_var-u16_tg_value)>400)
						{
							u8_tg_cut_cnt+=40;
						}
						else if((u16_tg_value_var-u16_tg_value)>300)
						{
							u8_tg_cut_cnt+=30;
						}
						else if((u16_tg_value_var-u16_tg_value)>200)
						{
							u8_tg_cut_cnt+=20;					
						}
						else if((u16_tg_value_var-u16_tg_value)>100)
						{
							u8_tg_cut_cnt+=10;
						}
						u8_tg_add_cnt=0;
					}
					else
					{							
						u8_tg_add_cnt=0;
						u8_tg_cut_cnt=0;
					}
					if(u8_tg_add_cnt>50||u8_tg_cut_cnt>50)
					{
						if(u8_tg_add_cnt>50)
						{
							if(u16_tg_value_var<829)
							{
								u16_tg_value_var++;
								if(u16_tg_value_var>810)
									u16_tg_value_var=830;		
							}
						}
						if(u8_tg_cut_cnt>50)
						{
							if(u16_tg_value_var>0)
							{
								u16_tg_value_var--;
							}
						}
						u8_tg_add_cnt=0;
						u8_tg_cut_cnt=0;
					
					}
				}
				u16_tg_value=u16_tg_value_var;
				_ptm1al=(u16_tg_value&0xFF);
				u16_tg_value>>=8;
				_ptm1ah=(u16_tg_value&0x03);	
			}
	/*		if(u8_tm_tg_25ms>1)
			{
				u8_tm_tg_25ms=0;
				_ptm1al=(u16_tg_value_var&0xFF);
				u16_tg_value>>=8;
				_ptm1ah=(u16_tg_value_var&0x03);
			}*/
		}
	}
	void main()
	{
		
	/*	if(_lvrf==1)
		{
			u8_led_mode=eeprom_read();
			_lvrf=0;
				if(u8_led_mode==1)
			LED_SN=1;
		}*/
		systemset();
		ioset();
		value_csh();
//		PTP_contrl(0);
		while(1)
		{
			_clrwdt();
////////////////////////////TIME//////////////////////////////////
////////////////////////////TIME//////////////////////////////////
			if(b_tm_1ms_flag==1)
			{
				u8_tm_1ms++;
				 b_tm_tg_1ms=1;
				if(u8_tm_1ms>=25)
				{
					u8_tm_1ms=0;
					b_tm_25ms_flag=1;
				}
			}
			if(b_tm_1ms_flag==1)
			{
				b_tm_1ms_flag=0;
				u8_tm_key1_1ms++;	
				u8_tm_key2_1ms++;	
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				if(u8_led_mode==2)
				{
					u8_tm_tg_25ms++;
				}
				else
				{
					u8_tm_tg_25ms=0;
				}
				u8_tm_halt_25ms++;		//休眠倒计时
#if (SP_FLAG==1)
				u8_tm_sp_25ms++;
#endif
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////	
			adc_test();

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
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
			if(KEY2!=b_key2)
			{
				if(u8_tm_key2_1ms>35)
				{
					b_key2=KEY2;
					u8_tm_key2_1ms=0;
				} 
			}
			else
			{
				u8_tm_key2_1ms=0;
				b_key_date2=b_key2^0x01;
				b_key_trg2=b_key_date2&(b_key_date2^b_key_cont2);
				b_key_cont2=b_key_date2;
			}
			if(b_key_cont1==0)
			{
				if(b_key_cont2==1)
				{
					if(b_key_trg2==1)
					{
						u8_led_mode=1;	
						LED_SN_TOP=1;
						PTP_contrl(0);
					}
				}
				else
				{
					u8_led_mode=0;
					LED_SN_TOP=0;
					PTP_contrl(0);
					u16_tg_value_var=0;	
				}
			}
			else
			{
				u8_led_mode=2;
				LED_SN_TOP=0;
				PTP_contrl(1);
				
			}
			LED_adj();

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////
#if (SP_FLAG==1)
			if(u8_tm_sp_25ms>20)
			{

				u8_sp_value[0] = ((u16_adc_final_value>>8)&0xFF);
				u8_sp_value[1] = (u16_adc_final_value&0xFF);
				u8_sp_value[2] = ((u16_tg_value>>8)&0xFF);
				u8_sp_value[3] = (u16_tg_value&0xFF);
				u8_sp_value[4] = 0xFF;
				
				u8_tm_sp_25ms=0;
				u8_sp_cnt=0;
			}
			if(u8_sp_cnt<5&&b_sp_flag==0)
			{
				u16_sp_buf_value=(((uint16)u8_sp_value[u8_sp_cnt]<<1)|0x0200);
				u8_sp_cnt++;
				b_sp_flag=1;
			}	
#endif




///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
			if(u8_led_mode!=0)
			{
				u8_tm_halt_25ms=0;
			}
			if(u8_tm_halt_25ms>=HALT_START_TM)
			{
				b_halt_flag=1;
				systemhalt();
				_pbc2=0;
				_pb2=1;
				_pac5=0;
				_pa5=1;
				
			//7() 6(LED_WS) 5(LED_OUT) 4( ) 3( ) 2(KEY2) 1(KEY1) 0(  )
			/*	_pa		=0b00000110;	//LED_WS(6) KEY2(2) KEY1(1)初始高电平，其余初始低电平
				_pac	=0b00000110;	//输入输出设置 0输出 1输入 KEY2(2)和KEY1(1)设置为输入，其余输出	
				_papu	=0b00000110;	//上拉电阻  0除能  1使能   KEY2(2)和KEY1(1)设置上拉，其余无上拉
				_pawu	=0b00000110;	//唤醒功能位  0除能 1使能  KEY2(2)和KEY1(1)设置唤醒，其余无唤醒
				_pasr	=0b00000000;
				_pa		=0b00000110;	//LCD_BG(7) KEY(1)初始高电平，其余初始低电平*/			
			}
			if(b_halt_flag==1)
			{
				_nop();
				_nop();
				_halt();
				_nop();
				_nop();
				systemset();
				ioset();
				_wdtc=0b01010111;
				u8_tm_halt_25ms=0;
				b_halt_flag=0;	
			}
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////
		}
	
	}


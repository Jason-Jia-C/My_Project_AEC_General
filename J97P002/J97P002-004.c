	#include "HT66F004.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int 
	
	#define LED_OUT1 _pb2//11
	#define LED_OUT2 _pb1//10
	
	#define KEY1 _pa1  //1
	#define V5V_IN _pb0//9
	
	#define SEG1 _pc2//8
	#define SEG2 _pc0//6
	#define SEG3 _pa3//12
	#define SEG4 _pc1//7
	#define SEG5 _pa5 //3
	
	#define LCD_BG 	_pa7//5
	#define LCD_COM _pa6//4
	#define CHG_DET _pa4//2
	
	#define TEST_FLAG	1
	#define TIME_FLAG 	0           //系统计时
	#define STATUS_FLAG 0			//系统状态
	#define ADC_FLAG 	1			//ADC转换程序
	#define BAT_FLAG 	1			//电池电压监控程序
	#define FLASH_FLAG	0			//低电压闪烁判断程序
	#define KEY_FLAG 	1			//按键检测程序
	#define LED_FLAG 	1			//LED执行程序
	#define LCD_FLAG 	0			//LCD执行程序
	#define HALT_FLAG 	1			//HALT执行程序
	#define SP_FLAG		1			//SP处理程序
	
	#define LED_NO_BRIGHT 		0			//无灯亮
	#define LED_TOP_BRIGHT 		1			//顶部灯亮
	#define LED_FRONT_WEAK 		2			//正面弱
	#define LED_FRONT_STRONG 	3			//正面强
		
	#define LED_PWM_NO 			0   		//PWM无
	#define LED_PWM_STRONG 		70			//PWM强
	#define LED_PWM_WEAK 		30			//PWM弱
	#define LED_PWM_PERIOD 		70			//PWM周期
	
	#define KEY_CHECK_TM 			20          //按键检验时间  1ms为单位		20MS
	#define KEY_LCD_OPEN_TM 		59			//LCD开启计时 25ms为单位		1.5S
	#define KEY_LED_SHUTDOWN_TM 	200			//LED直接关闭计时 25ms为单位	5S
	#define HALT_START_TM			200			//休眠计时，25ms为单位			5S
	#define LCD_CLOSE_TM			80			//LCD 关闭计时,	25ms为单位		2S
	#define BAT_FLASH_TM			60			//低电量闪烁检测计时 1s为单位 	1M
	#define BAT_FULL_TM 			180			//BAT充满计时 1S为单位			3M
	#define BAT_CHANGE_CHECK_TM 	240			//BAT级别改变计时 1S为单位		4M
	#define BAT_CHANGE_CHECK_4_TM 	600		//充电时第四格电量改变计时		10M
	#define FLASH_TWO_TM			60			//flash第二段闪烁间隔时间		1M
	#define FLASH_ONE_CNT			60			//flash第一段闪烁次数			60次(30组)
	#define FLASH_TWO_CNT			6			//flash第二段闪烁次数			6次(3组)
	
	#define BAT_LOW_LEVEL 1452 		//闪烁电压 2.95V负载中
	
	//使用时电压
	#define BAT_FZ_LEVEL2 1547		//	3.144V
	#define BAT_FZ_LEVEL3 1582		//  3.215V
	#define BAT_FZ_LEVEL4 1625	 	//  3.302v
	#define BAT_FZ_LEVEL5 1675		//  3.402V

	//空暇时电压
	#define BAT_KZ_LEVEL2 1675		//	3.4
	#define BAT_KZ_LEVEL3 1772		//	3.6
	#define BAT_KZ_LEVEL4 1870		//	3.8
	#define BAT_KZ_LEVEL5 1993		//	4.05
	
	//充电时空载电压
	#define BAT_CHG_KZ_LEVEL2 1952	//	3.965
	#define BAT_CHG_KZ_LEVEL3 1974	//	4.01
	#define BAT_CHG_KZ_LEVEL4 1993	//	4.05
	
	//充电时负载电压
	#define BAT_CHG_FZ_LEVEL2 1575	//	3.2
	#define BAT_CHG_FZ_LEVEL3 1673	//	3.4
	#define BAT_CHG_FZ_LEVEL4 1772	//	3.6

/////////////////////////////Sp_date/////////////////////////////////////////
	#define CHAR_BIT 8
	#define BITMASK(b)		(1<<((b)%CHAR_BIT))
	#define BITPASK(b)		((1<<((b)%CHAR_BIT))^0xff)
	#define BITSLOT(b)		((b)/CHAR_BIT)
	#define BITSET(a,b) 	((a)[BITSLOT(b)]|=BITMASK(b))
	#define BITCLEAR(a,b)	((a)[BITSLOT(b)]&=(BITPASK(b)))
	#define BITTEST(a,b)	((a)[BITSLOT(b)]&BITMASK(b))
	#define BITNSLOTS(nb)	(((nb)+CHAR_BIT-1)/CHAR_BIT)
/////////////////////////////Sp_date//////////////////////////////////////////
	
	
	#pragma vector tb0_int	@ 0x08 		//tb0中断
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	//CHG								    
	bool b_chg_flag;
	bool b_chg_det_flag;
	
	//软件时钟
	bool b_tm_1ms_flag;
	bool b_tm_25ms_flag;
	bool b_tm_200ms_flag;
	bool b_tm_1s_flag;
	
	uint8 u8_tm_128us;
	uint8 u8_tm_1ms;
	uint8 u8_tm_25ms;
	uint8 u8_tm_200ms;	
	
	//低电压闪烁
	bool b_flash_flag;
	bool b_bat_low_flag;
	bool b_part_one_flag;
	bool b_part_two_flag;
	bool b_flash_flip_flag;
	bool b_tm_flash_flag;
	
	uint8 u8_flash_cnt;
	uint8 u8_tm_part_one_25ms;
	uint8 u8_tm_part_two_25ms;
	uint8 u8_tm_part_two_1s;
	uint8 u8_tm_flash_1s;
	
	//KEY
	bool b_key;
	bool b_key_trg;
	bool b_key_date;
	bool b_key_cont;
	bool b_key_flag;
	bool b_led_shutdown_flag;
	
	uint8 u8_tm_key_down_1ms;
	uint8 u8_tm_key_up_1ms;
	uint8 u8_tm_key_1ms;
	uint8 u8_tm_key_25ms;
	
	//SP
	bool b_sp_flag;	
	
	uint8 u8_sp_num;
	uint8 u8_sp_cnt;
	uint8 u8_tm_sp_128us;
	uint8 u8_sp_date[BITSLOT(24)];  //数据内容存放数组
	
	//ADC
	uint8 u8_adc_cnt;
	uint8 u8_v_cnt;
	uint8 u8_bat_level;
	uint8 u8_bat_new_level;
	uint8 u16_tm_bat_level_1s;
	uint8 u8_tm_bat_full_1s;
	
	uint16 u16_adc_vdd;
	uint16 u16_adc_max;
	uint16 u16_adc_min;
	uint16 u16_adc_sum;
	uint16 u16_adc_real_vdd;
	uint16 u16_adc_sp_vdd;
	uint16 u16_adc_sum_vdd;
	
	//LCD
	bool b_seg1;
	bool b_seg2;
	bool b_seg3;
	bool b_seg4;
	bool b_seg5;
	bool b_lcd_com;
	bool b_lcd_bg;
	bool b_lcd_flag;
	bool b_lcd_open;
	bool b_lcd_close;
	
	uint8 u8_tm_lcd_25ms;
	
	//LED
	uint8 u8_tm_pwm_add;
	uint8 u8_led_pwm_cnt;		
	uint8 u8_led_mode;
	uint8 u8_led_mode1;
	uint8 u8_led_step;	
	uint8 b_light_flag;
	
	//TEST
	bool b_tm_test_flag;
	bool b_tm_test_mode_flag;
	
	uint8 u8_test_mode;
	uint8 u8_tm_test_mode_25ms;
	uint8 u8_tm_test_25ms;
	uint8 u8_tm_sp_25ms;
	uint8 u8_tm_charge_1ms;
	

		
	#pragma norambank
	
	void systemset();
	void systemhalt();
	void ioset();
	void adc_test();
	void eeprom_read();
	void eeprom_wirte(uint8 u8_keys);
	void value_csh();
	
	void systemset()	
	{
		_smod	=0b00000001; 	// 系统时钟选择  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000001;	//INTN中断脚中断边沿控制，双沿触发 PB0脚为充电感应脚，当充电时触发INT0，唤醒单片机
		_intc0	=0b00000101; 	//中断设置
		_intc1	=0b00000000;	//中断设置
		_mfi0	=0b00000000;	//多功能中断设置
		_tbc	=0B11000000;  	//时基设置  tb0   7812.5HZ   128us  
		_wdtc	=0b01010111; 
	}

	void systemhalt()
	{
		_smod	=0b00000001; 	// 系统时钟选择  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000011;	//INTN中断脚中断边沿控制，双沿触发 PB0脚为充电感应脚，当充电时触发INT0，唤醒单片机
		_intc0	=0b00000011; 	//中断设置
		_intc1	=0b00000000;	//中断设置
		_mfi0	=0b00000000;	//多功能中断设置
		_tbc	=0B00000000;  	//时基设置  tb0   7812.5HZ   128us  
		_wdtc	=0b10101111; 	
	}

	void ioset()
	{
		//7(LCD_BG) 6(LCD_COM) 5(SEG5) 4(CHG_DET) 3(SEG3) 2( ) 1(KEY) 0( )
		_pa		=0b10000010;	//LCD_BG(7) KEY(1)初始高电平，其余初始低电平
		_pac	=0b00010110;	//输入输出设置 0输出 1输入 CHG_DET(4)和KEY(1)和sp(2)设置为输入，其余输出	
		_papu	=0b00010010;	//上拉电阻  0除能  1使能   CHG_DET(4)和KEY(1)设置上拉，其余无上拉
		_pawu	=0b00000010;	//唤醒功能位  0除能 1使能	KEY(1)设置唤醒，其余无唤醒
		_pa		=0b10000010;	//LCD_BG(7) KEY(1)初始高电平，其余初始低电平

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(LED_OUT1) 1(LED_OUT2) 0(V5V_IN)
		_pb		=0b00000000;	//初始全为低电平 LED1(2) LED2(1) V5V_IN(0)
		_pbc	=0b00000001;	//V5V_IN设置成输入，其余为输出		
		_pbpu	=0b00000000;	//全无上拉电阻
		_pbsr	=0b00000000;	//全为I/O口或INTx
		_pb		=0b00000000;	//初始全为低电平 LED1(2) LED2(1) V5V_IN(0)

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(SEG1) 1(SEG4) 0(SEG2)
		_pc		=0b00000000;	//初始全为低电平
		_pcc	=0b00000000;	//初始全为输出	
		_pcpu	=0b00000000;	//全无上拉电阻
		_pc		=0b00000000;	//初始全为低电平
		
	}
	
	void eeprom_read()
	{
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
		
		u8_led_mode1 = _eed;
	}
	
	void eeprom_wirte(uint8 u8_keys)
	{
		_eea   = 0x01;
		_eed   = u8_keys;
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
	
	void adc_test()
	{
		_sadc0	=0b01110000;
		_sadc1	=0b01100100;   //7-5 AVDD/4    2-0 Fsys/16
		_sadc2	=0b11001010;  //3-0 Vbg*2
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
		if(u8_adc_cnt>=28)
		{
			u16_adc_sum-=u16_adc_max;
			u16_adc_sum-=u16_adc_min;
			u16_adc_vdd=(u16_adc_sum/26);
			u16_adc_max=0;
			u16_adc_min=0xffff;
			u16_adc_sum=0;	
		}
	}
	
	void tb0_int()
	{
		u8_tm_128us++;
		u8_tm_sp_128us++;
		if(u8_tm_128us>7)
		{
			u8_tm_128us=0;
			b_tm_1ms_flag=1;
		}
		if(u8_tm_sp_128us==8)
		{
			u8_tm_sp_128us=0;
			if(b_sp_flag==1)
			{
				_pac2=0;
				if(u8_sp_cnt==0||u8_sp_cnt==10||u8_sp_cnt==20)
				_pa2=0;
				else if(u8_sp_cnt==9||u8_sp_cnt==19||u8_sp_cnt==29)
				_pa2=1;
				else
				{
					_pa2=BITTEST(u8_sp_date,(u8_sp_cnt-(u8_sp_cnt/10*2)-1));
				}
				u8_sp_cnt++;
			}	
		}
		if(u8_led_mode==LED_FRONT_WEAK||u8_led_mode==LED_FRONT_STRONG)           //软件PWM控制
		{
			if(u8_led_mode==LED_FRONT_WEAK)
			{
				u8_tm_pwm_add++;
				if(u8_tm_pwm_add<u8_led_pwm_cnt)
				{
					LED_OUT2=1;
				}
				else if(u8_tm_pwm_add<LED_PWM_PERIOD)
				{
					LED_OUT2=0;
				}
				else 
				{
					u8_tm_pwm_add=0;
				}
			}
			else
			{
				LED_OUT2=1;
			}
		}
	}

	void bat_choice()
	{
		if(b_chg_flag==0)                //未充电
		{
			if(u8_led_mode>1)         //有负载(顶部负载把其放在无负载中计算)
			{
				if(u16_adc_real_vdd>BAT_FZ_LEVEL5)
				{
					u8_bat_new_level=5;
				}
				else if(u16_adc_real_vdd>BAT_FZ_LEVEL4)
				{
					u8_bat_new_level=4;
				}
				else if(u16_adc_real_vdd>BAT_FZ_LEVEL3)
				{
					u8_bat_new_level=3;
				}
				else if(u16_adc_real_vdd>BAT_FZ_LEVEL2)
				{
					u8_bat_new_level=2;
				}
				else
				{
					u8_bat_new_level=1;
				}
			}
			else                   //无负载（包括顶部负载）
			{
				if(u16_adc_real_vdd>BAT_KZ_LEVEL5)
				{
					u8_bat_new_level=5;
				}
				else if(u16_adc_real_vdd>BAT_KZ_LEVEL4)
				{
					u8_bat_new_level=4;
				}
				else if(u16_adc_real_vdd>BAT_KZ_LEVEL3)
				{
					u8_bat_new_level=3;
				}
				else if(u16_adc_real_vdd>BAT_KZ_LEVEL2)
				{
					u8_bat_new_level=2;
				}
				else
				{
					u8_bat_new_level=1;
				}
			}
		}
		else   //在充电
		{
			if((u8_led_mode>1&&u16_adc_real_vdd<BAT_CHG_FZ_LEVEL4)||(u8_led_mode<2&&u16_adc_real_vdd<BAT_CHG_KZ_LEVEL4)||b_chg_det_flag==0)
			{
				u8_tm_bat_full_1s=0;
				u8_bat_new_level=1;
			}
			else
			{
				if(u8_tm_bat_full_1s>=BAT_FULL_TM)
				{
					u8_tm_bat_full_1s=0;
					u8_bat_new_level=5;
				}
			}
			if(u8_bat_new_level<5)
			{
				if(u8_led_mode<2)             //充电时空载运行
				{
					if(u16_adc_real_vdd>BAT_CHG_KZ_LEVEL4)
					{
						u8_bat_new_level=4;
					}
					else if(u16_adc_real_vdd>BAT_CHG_KZ_LEVEL3)
					{
						u8_bat_new_level=3;
					}
					else if(u16_adc_real_vdd>BAT_CHG_KZ_LEVEL2)
					{
						u8_bat_new_level=2;
					}
					else
					{
						u8_bat_new_level=1;
					}
				}
				else							//充电时负载运行
				{
					if(u16_adc_real_vdd>BAT_CHG_FZ_LEVEL4)
					{
						u8_bat_new_level=4;
					}
					else if(u16_adc_real_vdd>BAT_CHG_FZ_LEVEL3)
					{
						u8_bat_new_level=3;
					}
					else if(u16_adc_real_vdd>BAT_CHG_FZ_LEVEL2)
					{
						u8_bat_new_level=2;
					}
					else
					{
						u8_bat_new_level=1;
					}
				}
			}
		}
	}

	void value_csh()
	{
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		b_tm_200ms_flag=0;
		b_tm_1s_flag=0; 		  
		b_halt_flag=0;
		b_light_flag=0;
		b_chg_flag=0;
		b_chg_det_flag=0;
		b_flash_flag=0;
		b_bat_low_flag=0;
		b_part_one_flag=0;
		b_part_two_flag=0;
		b_flash_flip_flag=0;
		b_lcd_flag=0;
		b_lcd_open=0;
		b_lcd_close=0; 
		b_key=1;///////////////////////////////////////初始状态为1
		b_key_trg=0;
		b_key_date=0;
		b_key_cont=0;
		b_key_flag=0;
		b_led_shutdown_flag=0;
		u8_tm_pwm_add=0;
		u8_led_pwm_cnt=0;							
		u8_led_mode=0;
		u8_led_mode1=0;
		u8_led_step=0;					
		u8_adc_cnt=0;
		u8_bat_level=0;
		u8_bat_new_level=0;
		u8_flash_cnt=0;	
		u8_tm_128us=0;
		u8_tm_1ms=0;
		u8_tm_25ms=0;
		u8_tm_200ms=0;						
		u8_tm_key_down_1ms=0;
		u8_tm_key_up_1ms=0;
		u8_tm_key_1ms=0;
		u8_tm_charge_1ms=0;
		u8_tm_key_25ms=0;		
		u8_tm_halt_25ms=0;
		u8_tm_lcd_25ms=0;
		u8_tm_part_one_25ms=0;
		u8_tm_part_two_25ms=0;	
		u16_tm_bat_level_1s=0;
		u8_tm_bat_full_1s=0;
		u8_tm_part_two_1s=0;
		u8_tm_flash_1s=0;	
		u16_adc_vdd=0;
		u16_adc_max=0;
		u16_adc_min=0xffff;
		u16_adc_sum=0;
		
		u16_adc_real_vdd=0;
		u16_adc_sp_vdd=0;
		u16_adc_sum_vdd=0;
		b_tm_flash_flag=0;
		
		u8_test_mode=0;
		u8_tm_test_25ms=0;
		u8_tm_test_mode_25ms=0;
		b_tm_test_flag=0;
		b_tm_test_mode_flag=0;
		
		b_sp_flag=0;
		u8_sp_cnt=0;
		u8_v_cnt=0;
		u8_tm_sp_25ms=0;
		u8_sp_num=0;
	}
	
	void main()
	{
		systemset();
		ioset();
		value_csh();
		while(1)
		{
			_clrwdt();
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
				u8_tm_key_down_1ms++;     //按键按下检验计时
				u8_tm_key_up_1ms++;		  //按键松开检验计时
				u8_tm_key_1ms++;
				if(u8_tm_charge_1ms<21)	  //充电检验计时
				{
					u8_tm_charge_1ms++;
				}
				if(b_lcd_open==1)		  //LCD翻转
				{
					LCD_COM	=~LCD_COM;
					SEG1	=~SEG1;
					SEG2	=~SEG2;
					SEG3	=~SEG3;
					SEG4	=~SEG4;
					SEG5	=~SEG5;
				}
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				u8_tm_key_25ms++;		//按键判定时间	
				u8_tm_part_one_25ms++; //第一段闪烁计时
				u8_tm_part_two_25ms++;	//第二段闪烁计时
				u8_tm_lcd_25ms++;      //LCD熄灭倒计时
				u8_tm_halt_25ms++;		//休眠倒计时
				u8_tm_sp_25ms++;           //数据传送计时
				if(b_tm_test_mode_flag==0)
				{
					u8_tm_test_mode_25ms++;
				}
			}
			if(b_tm_200ms_flag==1)         //每个数值在第一次计数时，会有1ms~100ms之间的误差
			{
				b_tm_200ms_flag=0;	
			}
			if(b_tm_1s_flag==1)          //无清除程序的值，第一次计数时，会有100到900ms之间的误差
			{
			//	LED_OUT1=1;
				b_tm_1s_flag=0;	
				u16_tm_bat_level_1s++;  //电量级别更新计时		//对时间不敏感，不清除
				u8_tm_bat_full_1s++;	//电量充满检验计时		//对时间不敏感，不清除
				u8_tm_part_two_1s++;	//第二段闪烁计时        //已有清除程序
				u8_tm_flash_1s++;		//闪烁判定计时          //已有清除程序
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////

////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////
////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////
	#if (TEST_FLAG==1)
			if(u8_tm_test_mode_25ms<=20&&b_tm_test_mode_flag==0&&u16_adc_real_vdd>1969)//只有前500ms
			{
			//	LED_OUT2=1;
				if(b_chg_flag==1)
				{
					if(b_key_cont==0)   ///////////////////////  正常模式，但是会先以一格电压显示3分钟
					{
						b_lcd_flag=1;
						u8_test_mode=0;
						u8_bat_level=1;		//如果开机没插电池，直接接入充电器，并且没有按下按键，那么显示3分钟1格电量后自动切回普通模式
					}
					else                   //如果开机没插电池，直接接入充电器，并且按下按键，进入测试模式1
					{
						u8_test_mode=1;  ///////////////// 测试模式1    	
					}
				}
				else
				{
					if(b_key_cont==1)
					{
						u8_test_mode=2;  /////////////////////测试模式2
					}
					else
					{
						u8_test_mode=0;///////////////////////////正常模式	
					}
				}
			}
			else if(u8_tm_test_mode_25ms>20)
			{
				b_tm_test_mode_flag=1;
			}
			while(u8_test_mode==1||u8_test_mode==2)
			{
				_clrwdt();
				b_tm_test_mode_flag=1;
				if(b_tm_1ms_flag==1)
				{
					b_tm_1ms_flag=0;
					u8_tm_1ms++;
					if(u8_tm_1ms>=25)
					{
						u8_tm_1ms=0;
						u8_tm_test_25ms++;
					}
					if(LCD_BG==0)
					{
						LCD_COM	=~LCD_COM;
						SEG1	=~SEG1;
						SEG2	=~SEG2;
						SEG3	=~SEG3;
						SEG4	=~SEG4;
						SEG5	=~SEG5;
					}
				}
				if(u8_tm_test_25ms<=40&&b_tm_test_flag==0)
				{
					b_tm_test_flag=1;
					b_lcd_open=1;
					LCD_BG=0;
					LCD_COM=0;
					SEG1=1;
					SEG2=1;
					SEG3=0;
					SEG4=1;
					SEG5=0;
				}
				else if(u8_tm_test_25ms>40&&u8_tm_test_25ms<50&&b_tm_test_flag==1)
				{
					b_tm_test_flag=0;
					u8_tm_test_25ms=50;
				}
				else if(u8_tm_test_25ms<=89&&b_tm_test_flag==0)
				{
					b_tm_test_flag=1;
					LCD_BG=0;
					LCD_COM=0;
					SEG1=0;
					SEG2=0;
					SEG3=1;
					SEG4=0;
					SEG5=1;	
				}
				else if(u8_tm_test_25ms>89)
				{
					if(u8_test_mode==1)
					{
						b_lcd_flag=1;
						u8_bat_level=1;
						u8_test_mode=0;
					}
					else
					{
						b_lcd_open=1;
						b_lcd_flag=1;
						u8_test_mode=10;
						b_tm_test_flag=0;	
					}
					b_key=1;
					b_tm_test_flag=0;
					b_tm_1ms_flag=0;
					u8_tm_1ms=0;
					
				}	
			}
	#endif
////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////
////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////

////////////////////////////SP//////////////////////////////////////////////////////////////////////////
////////////////////////////SP//////////////////////////////////////////////////////////////////////////
	#if (SP_FLAG==1)
			if(u8_tm_sp_25ms>=40)	  //数据传输
			{
				u16_adc_sp_vdd=u16_adc_real_vdd;
				for(u8_sp_num=0;u8_sp_num<12;u8_sp_num++)
				{
					if(((u16_adc_sp_vdd>>u8_sp_num)&0b0000000000000001)==1)        //电量数据
						BITSET(u8_sp_date,u8_sp_num);
					else
						BITCLEAR(u8_sp_date,u8_sp_num);
				}
				
				if(u8_led_mode==0)				//模式数据
				{
					BITCLEAR(u8_sp_date,12);
					BITCLEAR(u8_sp_date,13);	
				}
				else if(u8_led_mode==1)
				{
					BITSET(u8_sp_date,12);
					BITCLEAR(u8_sp_date,13);	
				}
				else if(u8_led_mode==2)
				{
					BITSET(u8_sp_date,13);
					BITCLEAR(u8_sp_date,12);	
				}
				else if(u8_led_mode==3)
				{
					BITSET(u8_sp_date,12);
					BITSET(u8_sp_date,13);	
				}
				if(b_chg_flag==1)           //充电数据
				{
					BITSET(u8_sp_date,14);	
				}
				else
				{
					BITCLEAR(u8_sp_date,14);
				}
				if(b_chg_det_flag==1)		//充满数据
				{
					BITSET(u8_sp_date,15);	
				}
				else
				{
					BITCLEAR(u8_sp_date,15);
				}
				
				if(b_bat_low_flag==1)		//低电压标志数据
				{
					BITSET(u8_sp_date,16);	
				}
				else
				{
					BITCLEAR(u8_sp_date,16);
				}
				if(b_flash_flag==1)		//低电压锁标数据
				{
					BITSET(u8_sp_date,17);	
				}
				else
				{
					BITCLEAR(u8_sp_date,17);
				}	
				if(b_part_one_flag==1)		//闪烁阶段标志数据
				{
					BITSET(u8_sp_date,18);	
				}	
				else
				{
					BITCLEAR(u8_sp_date,18);	
				}
				if(u8_flash_cnt>0)			//闪烁频率数据
				{
					BITSET(u8_sp_date,19);	
				}	
				else
				{
					BITCLEAR(u8_sp_date,19);	
				}		
				if(u8_bat_level==5)		//实时电压等级数据
				{
					BITSET(u8_sp_date,20);
					BITCLEAR(u8_sp_date,21);
					BITSET(u8_sp_date,22);
				}
				else if(u8_bat_level==4)
				{
					BITCLEAR(u8_sp_date,20);
					BITCLEAR(u8_sp_date,21);
					BITSET(u8_sp_date,22);	
				}
				else if(u8_bat_level==3)
				{
					BITSET(u8_sp_date,20);
					BITSET(u8_sp_date,21);
					BITCLEAR(u8_sp_date,22);	
				}
				else if(u8_bat_level==2)
				{
					BITCLEAR(u8_sp_date,20);
					BITSET(u8_sp_date,21);
					BITCLEAR(u8_sp_date,22);	
				}
				else if(u8_bat_level==1)
				{
					BITSET(u8_sp_date,20);
					BITCLEAR(u8_sp_date,21);
					BITCLEAR(u8_sp_date,22);	
				}
				BITCLEAR(u8_sp_date,23);
				u8_tm_sp_25ms=0;
				b_sp_flag=1;
			}
			if(u8_sp_cnt==30)
			{
				_pac2=1;
				u8_sp_cnt=0;
				b_sp_flag=0;
			}
	#endif
////////////////////////////SP//////////////////////////////////////////////////////////////////////////
////////////////////////////SP//////////////////////////////////////////////////////////////////////////
	
///////////////////////////STATUS//////////////////////////////////////////////////////////////////////////
///////////////////////////STATUS//////////////////////////////////////////////////////////////////////////
			if(V5V_IN==0)         //未在充电
			{
				if(u8_tm_charge_1ms>20)
				{
					b_lcd_open=0;
				}
				u8_tm_charge_1ms=0;
				b_chg_flag=0;  
				b_chg_det_flag=0;
			}	
			else					 //充电中
			{
				if(u8_tm_charge_1ms>=20)
				{
					b_lcd_open=1;
					if(b_lcd_bg==1)
					b_lcd_flag=1;
					b_chg_flag=1; 
				}
			}
			
			if(b_chg_flag==1)  //判断充电中的状态
			{
				if(CHG_DET==0)		//充满电了
				{
					b_chg_det_flag=1;  
				}
				else				 //未充满
				{
					b_chg_det_flag=0;
				}
			}
			if(u8_led_mode==LED_NO_BRIGHT)
			{
				b_light_flag=0;
			}
			else 
			{
				b_light_flag=1;
			}
			if(u8_test_mode==10)
			{
				b_lcd_open=1;
			}
///////////////////////////STATUS//////////////////////////////////////////////////////////////////////////
///////////////////////////STATUS//////////////////////////////////////////////////////////////////////////

///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////
	#if (ADC_FLAG==1)
			adc_test();
			if(u8_adc_cnt>=28)
			{
				if(u8_led_mode==LED_FRONT_WEAK)
				{
					if(u8_tm_pwm_add>0&&u8_v_cnt<28)
					{
						u16_adc_sum_vdd+=u16_adc_vdd;
						u8_v_cnt++;
					}
					else
					{
						if(u8_v_cnt>0)
						{
							u16_adc_real_vdd=u16_adc_sum_vdd/u8_v_cnt;
							u16_adc_sum_vdd=0;
							u8_v_cnt=0;
						}
					}
				}
				else
				{	
					u16_adc_real_vdd=u16_adc_vdd;
				}
			}
	#endif
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	

///////////////////////////BAT//////////////////////////////////////////////////////////////////////////
///////////////////////////BAT//////////////////////////////////////////////////////////////////////////
	#if (BAT_FLAG==1)
			if(u8_adc_cnt>=28) 
			{
				u8_adc_cnt=0;
				if(u16_adc_real_vdd>0)
				{
					bat_choice();
				}
				if(u8_bat_new_level==u8_bat_level)
				{
					u16_tm_bat_level_1s=0;
				}
				else if((u8_bat_level==0)||(u8_test_mode==10))   //更新电压等级
				{
					u16_tm_bat_level_1s=0;
					u8_bat_level=u8_bat_new_level;
					b_lcd_flag=1;
				}
			/*	else if(b_chg_flag==1&&u8_bat_level==3&&u8_bat_new_level==4&&u16_tm_bat_level_1s>=BAT_CHANGE_CHECK_4_TM)
				{
					u16_tm_bat_level_1s=0;
					u8_bat_level=u8_bat_new_level;
					b_lcd_flag=1;	
				}	*/
				else if(u16_tm_bat_level_1s>=BAT_CHANGE_CHECK_TM)
				{
					u16_tm_bat_level_1s=0;
					u8_bat_level=u8_bat_new_level;
					b_lcd_flag=1;
				}
				if(b_chg_flag==1&&u8_bat_level>1)  //充电冲到2格电以上的时候，将闪烁主标志位复位
				{
					b_flash_flag=0;
				}
			}
	#endif
///////////////////////////BAT//////////////////////////////////////////////////////////////////////////
///////////////////////////BAT//////////////////////////////////////////////////////////////////////////

///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
	#if (FLASH_FLAG==1)   //闪烁部分
			if(b_chg_flag==1||b_light_flag==0)//充电时，关灯时 都会将闪烁计时清除
			{
				u8_tm_flash_1s=0;
				b_bat_low_flag=0;  //在这两种情况的时候，闪烁功能都会被强制关闭
				b_tm_flash_flag=0; //闪烁时间初始化标志
			//	LED_OUT2=0;
			}
			if((b_light_flag==1&&u16_adc_real_vdd>BAT_LOW_LEVEL)||b_flash_flag==1) //亮灯时，但是电量高于闪烁电压，也会将闪烁计时清零
			{
				u8_tm_flash_1s=0;	
				if(b_bat_low_flag==0)
				{
					b_tm_flash_flag=0;//闪烁时间初始化标志
				}
			}
			
			/////////////////////////////在计时之前先把FLASH的TM清零
			if(b_light_flag==1&&u16_adc_real_vdd<BAT_LOW_LEVEL&&b_tm_flash_flag==0&&b_bat_low_flag==0)
			{
				u8_tm_200ms=0;
				u8_flash_cnt=0;
				b_tm_flash_flag=1;
			}
			/////////////////////////////在计时之前先把TM清零
			if(u8_tm_flash_1s>=BAT_FLASH_TM&&b_flash_flag==0)
			{
				u8_tm_flash_1s=0;
				b_flash_flag=1;                    //闪烁主标志
				b_bat_low_flag=1;					//闪烁次标志
				b_part_one_flag=1;					//第一次闪烁标志
				u8_led_step=10;
			//	LED_OUT2=1;
			}
			if(b_bat_low_flag==0)          //如果闪烁次标志为0
			{
				b_part_one_flag=0;
				b_part_two_flag=0;
				u8_tm_part_one_25ms=0;
				u8_tm_part_two_25ms=0;
				u8_tm_part_two_1s=0;
			}
			else                          //如果闪烁次标志为1
			{
				if(b_part_one_flag==0)        //如果不在第一段闪烁
				{
					u8_tm_part_one_25ms=0;
				}
				else					//如果在第一段闪烁，那么肯定不会在第二段闪烁
				{
					u8_tm_part_two_25ms=0;
					u8_tm_part_two_1s=0;
				}	
		/////////////////////////////第一部分闪烁，1HZ频率闪烁30次
				if(u8_tm_part_one_25ms>=20&&b_part_one_flag==1)
				{
					u8_tm_part_one_25ms=0;
					b_flash_flip_flag=(~b_flash_flip_flag);
					u8_flash_cnt++;
				}
				if(b_part_one_flag==1&&u8_flash_cnt>=FLASH_ONE_CNT)  /////第一段闪烁结束
				{
					u8_flash_cnt=0;
					b_part_one_flag=0;
					b_flash_flip_flag=0;
	/////////////////////////////////////////////////////
				//	b_flash_flip_flag=0;
				//	u8_led_step=3;
	//////////////////////////////////////////////////////				
					
	//////////////////在进入第二段计时之前先把计时器清零
					u8_tm_200ms=0;
	//////////////////在进入第二段计时之前先把计时器清零
				//	LED_OUT2=1;

				}
		///////////////////////////第二部分闪烁，每隔一分钟闪烁3次		
				if(u8_tm_part_two_1s<FLASH_TWO_TM&&b_part_one_flag==0)
				{
				//	LED_OUT2=1;
					u8_flash_cnt=0;
					u8_tm_part_two_25ms=0;
					b_part_two_flag=0;
				}
				else
				{
					b_part_two_flag=1;                                   
					if(u8_tm_part_two_25ms>=20)
					{
						u8_tm_part_two_25ms=0;
						b_flash_flip_flag=~b_flash_flip_flag;
						u8_flash_cnt++;
					}
				}
				if(b_part_two_flag==1&&u8_flash_cnt==FLASH_TWO_CNT) 
				{
				//	LED_OUT2=1;
					
					u8_tm_part_two_1s=0;
				}
		//////////////////////////////闪烁主标志清除部分
			}
	#endif
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
			if(KEY1!=b_key)                   //按键自检程序，确认按下
			{
				if(KEY1==0)
				{
					if(u8_tm_key_down_1ms>=KEY_CHECK_TM)
					{
						b_key=KEY1;
						u8_tm_key_1ms=0;
						u8_tm_key_25ms=0;
						u8_tm_key_down_1ms=0;
					}
					u8_tm_key_up_1ms=0;
				}
				else
				{
					if(u8_tm_key_up_1ms>=KEY_CHECK_TM)
					{
						b_key=KEY1;
						u8_tm_key_1ms=0;
						u8_tm_key_25ms=0;
						u8_tm_key_up_1ms=0;
					}
					u8_tm_key_down_1ms=0;
				}
			}
			else
			{
				u8_tm_key_down_1ms=0;
				u8_tm_key_up_1ms=0;
				b_key_date=b_key^0x01;
				b_key_trg=b_key_date&(b_key_date^b_key_cont);
				b_key_cont=b_key_date;
			}						
			if(b_key_cont==1&&b_key_trg==1&&b_tm_test_mode_flag==1)     //按键按下通过检验的第一瞬间
			{
				b_key_flag=1;					  //先默认是进行LED操作的
				u8_tm_key_1ms=0;
				u8_tm_key_25ms=0;
			}
			else if(b_key_cont==1&&b_key_trg==0&&b_tm_test_mode_flag==1)  //按下持续按下
			{
				if(u8_tm_key_25ms>=KEY_LCD_OPEN_TM&&b_key_flag==1)//如果按键时间超过设定值，则将此次按键认为是电量LCD
				{
					b_lcd_close=0;
					b_key_flag=0;         //将之前默认的LED操作清除
					b_lcd_open=1;
					b_lcd_flag=1;
				}
			}
			else if(b_key_cont==0)  //按键松开
			{
				if(b_key_flag==1||b_led_shutdown_flag==1)  
				{
					if(b_key_flag==1)
					{
						b_key_flag=0;
						u8_led_step=2;
					}
					u8_tm_key_1ms=0;
					u8_tm_key_25ms=0;
				}
				if(b_light_flag==1&&b_led_shutdown_flag==0)
				{
					if(u8_tm_key_25ms>=KEY_LED_SHUTDOWN_TM) //有灯连续亮了4S
					{
						b_led_shutdown_flag=1;
						eeprom_wirte(u8_led_mode);
					}
				}
				if(b_lcd_open==1&&b_chg_flag==0&&u8_test_mode!=10)   //如果LCD打开着，并且按键松开，同时也没有在充电，那么准备关闭LCD
				{
					b_lcd_close=1;
				}
			}		
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////

///////////////////////////LED//////////////////////////////////////////////////////////////////////////	
///////////////////////////LED//////////////////////////////////////////////////////////////////////////	
			if(u8_led_step==2)
			{
				if(b_led_shutdown_flag==0)
				{
					eeprom_read();
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
					}
					u8_led_step=3;
					eeprom_wirte(0);
					b_bat_low_flag=0;
				}
				else
				{
					b_led_shutdown_flag=0;
					u8_led_mode=LED_NO_BRIGHT;
					u8_led_step=3;
				}
			}
			else if(u8_led_step==3)
			{
				if(u8_led_mode==LED_NO_BRIGHT)//没有灯亮 0
				{
					LED_OUT1=0;
					LED_OUT2=0;
					u8_led_pwm_cnt=LED_PWM_NO;
				}
				else if(u8_led_mode==LED_TOP_BRIGHT)  //顶部灯亮 1
				{
					LED_OUT1=1;
					LED_OUT2=0;
					u8_led_pwm_cnt=LED_PWM_NO;
				}
				else if(u8_led_mode==LED_FRONT_WEAK)  //正面弱 2
				{
					LED_OUT1=0;
					u8_led_pwm_cnt=LED_PWM_WEAK;
				}
				else if(u8_led_mode==LED_FRONT_STRONG) //正面强 3
				{
					LED_OUT1=0;
					u8_led_pwm_cnt=LED_PWM_STRONG;
				}
				if(b_bat_low_flag==0)
				{
					u8_led_step=4;             //空集
				}
				else
				{
					u8_led_step=10;				//闪烁集
				}
			}
			else if(u8_led_step==4)
			{
				u8_led_step=4;
			}
			else if(u8_led_step==10)
			{
				if(b_bat_low_flag==1)         
				{
					if(b_part_one_flag==1)
					{
						if(b_flash_flip_flag==1)
						{
							u8_led_step=11;
						}
						else
						{
							u8_led_step=3;
						}
					}
					else 
					{
						if(b_part_two_flag==1)
						{
							if(b_flash_flip_flag==1)
							{
								u8_led_step=11;
							}
							else
							{
								u8_led_step=3;
							}	
						}
						else
						{
							u8_led_step=3;	
						}
					}
				}
				else
				{
					u8_led_step=3;	
				}
			}
			else if(u8_led_step==11)
			{
				LED_OUT1=0;
				LED_OUT2=0;
				u8_led_pwm_cnt=LED_PWM_NO;
				u8_led_step=10;
			}	
///////////////////////////LED//////////////////////////////////////////////////////////////////////////	
///////////////////////////LED//////////////////////////////////////////////////////////////////////////

///////////////////////////LCD//////////////////////////////////////////////////////////////////////////	
///////////////////////////LCD//////////////////////////////////////////////////////////////////////////	
	#if (LCD_FLAG==1)
			if(b_lcd_open==1)
			{
				b_lcd_bg=0;
				b_lcd_com=0;
				if(u8_bat_level==5)
				{
					b_seg1=1;
					b_seg2=1;
					b_seg3=1;
					b_seg4=1;
					b_seg5=1;				
				}
				else if(u8_bat_level==4)
				{
					b_seg1=1;
					b_seg2=0;
					b_seg3=1;
					b_seg4=1;
					b_seg5=1;
				}
				else if(u8_bat_level==3)
				{
					b_seg1=1;
					b_seg2=0;
					b_seg3=0;
					b_seg4=1;
					b_seg5=1;
				}
				else if(u8_bat_level==2)
				{
					b_seg1=1;
					b_seg2=0;
					b_seg3=0;
					b_seg4=0;
					b_seg5=1;
				}
				else if(u8_bat_level==1)
				{
					b_seg1=1;
					b_seg2=0;
					b_seg3=0;
					b_seg4=0;
					b_seg5=0;
				}
				if(b_lcd_close==0)
				{
					u8_tm_lcd_25ms=0;
				}
				if(u8_tm_lcd_25ms>=LCD_CLOSE_TM)
				{
					b_lcd_open=0;
				}
			}
			else
			{
				b_lcd_close=0;
				u8_tm_lcd_25ms=0;
				b_lcd_flag=1;
				b_lcd_bg=1;
				b_lcd_com=0;
				b_seg1=0;
				b_seg2=0;
				b_seg3=0;
				b_seg4=0;
				b_seg5=0;
			}
			if(b_lcd_flag==1)
			{
				if(b_lcd_bg==1)
					LCD_BG=1;
				else
					LCD_BG=0;
				if(b_lcd_com==1)
					LCD_COM=1;
				else
					LCD_COM=0;
				if(b_seg1==1)
					SEG1=1;
				else
					SEG1=0;
				if(b_seg2==1)
					SEG2=1;
				else
					SEG2=0;
				if(b_seg3==1)
					SEG3=1;
				else
					SEG3=0;
				if(b_seg4==1)
					SEG4=1;
				else
					SEG4=0;
				if(b_seg5==1)
					SEG5=1;
				else
					SEG5=0;
				
				b_lcd_flag=0;
			}
	#endif
///////////////////////////LCD//////////////////////////////////////////////////////////////////////////	
///////////////////////////LCD//////////////////////////////////////////////////////////////////////////

///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
	#if (HALT_FLAG==1)
			if(b_light_flag==1||b_chg_flag==1||b_lcd_open==1)
			{
				u8_tm_halt_25ms=0;
			}
			if(u8_tm_halt_25ms>=HALT_START_TM)
			{
				b_halt_flag=1;
				systemhalt();
				ioset();	
				_int0e=1;		
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
				u8_tm_halt_25ms=0;
				b_halt_flag=0;
				_wdtc=0b01010111;
			}
	#endif
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////
		}
	
	}


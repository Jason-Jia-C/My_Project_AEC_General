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
	#define TIME_FLAG 	0           //ϵͳ��ʱ
	#define STATUS_FLAG 0			//ϵͳ״̬
	#define ADC_FLAG 	1			//ADCת������
	#define BAT_FLAG 	1			//��ص�ѹ��س���
	#define FLASH_FLAG	0			//�͵�ѹ��˸�жϳ���
	#define KEY_FLAG 	1			//����������
	#define LED_FLAG 	1			//LEDִ�г���
	#define LCD_FLAG 	0			//LCDִ�г���
	#define HALT_FLAG 	1			//HALTִ�г���
	#define SP_FLAG		1			//SP�������
	
	#define LED_NO_BRIGHT 		0			//�޵���
	#define LED_TOP_BRIGHT 		1			//��������
	#define LED_FRONT_WEAK 		2			//������
	#define LED_FRONT_STRONG 	3			//����ǿ
		
	#define LED_PWM_NO 			0   		//PWM��
	#define LED_PWM_STRONG 		70			//PWMǿ
	#define LED_PWM_WEAK 		30			//PWM��
	#define LED_PWM_PERIOD 		70			//PWM����
	
	#define KEY_CHECK_TM 			20          //��������ʱ��  1msΪ��λ		20MS
	#define KEY_LCD_OPEN_TM 		59			//LCD������ʱ 25msΪ��λ		1.5S
	#define KEY_LED_SHUTDOWN_TM 	200			//LEDֱ�ӹرռ�ʱ 25msΪ��λ	5S
	#define HALT_START_TM			200			//���߼�ʱ��25msΪ��λ			5S
	#define LCD_CLOSE_TM			80			//LCD �رռ�ʱ,	25msΪ��λ		2S
	#define BAT_FLASH_TM			60			//�͵�����˸����ʱ 1sΪ��λ 	1M
	#define BAT_FULL_TM 			180			//BAT������ʱ 1SΪ��λ			3M
	#define BAT_CHANGE_CHECK_TM 	240			//BAT����ı��ʱ 1SΪ��λ		4M
	#define BAT_CHANGE_CHECK_4_TM 	600		//���ʱ���ĸ�����ı��ʱ		10M
	#define FLASH_TWO_TM			60			//flash�ڶ�����˸���ʱ��		1M
	#define FLASH_ONE_CNT			60			//flash��һ����˸����			60��(30��)
	#define FLASH_TWO_CNT			6			//flash�ڶ�����˸����			6��(3��)
	
	#define BAT_LOW_LEVEL 1452 		//��˸��ѹ 2.95V������
	
	//ʹ��ʱ��ѹ
	#define BAT_FZ_LEVEL2 1547		//	3.144V
	#define BAT_FZ_LEVEL3 1582		//  3.215V
	#define BAT_FZ_LEVEL4 1625	 	//  3.302v
	#define BAT_FZ_LEVEL5 1675		//  3.402V

	//��Ͼʱ��ѹ
	#define BAT_KZ_LEVEL2 1675		//	3.4
	#define BAT_KZ_LEVEL3 1772		//	3.6
	#define BAT_KZ_LEVEL4 1870		//	3.8
	#define BAT_KZ_LEVEL5 1993		//	4.05
	
	//���ʱ���ص�ѹ
	#define BAT_CHG_KZ_LEVEL2 1952	//	3.965
	#define BAT_CHG_KZ_LEVEL3 1974	//	4.01
	#define BAT_CHG_KZ_LEVEL4 1993	//	4.05
	
	//���ʱ���ص�ѹ
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
	
	
	#pragma vector tb0_int	@ 0x08 		//tb0�ж�
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	//CHG								    
	bool b_chg_flag;
	bool b_chg_det_flag;
	
	//���ʱ��
	bool b_tm_1ms_flag;
	bool b_tm_25ms_flag;
	bool b_tm_200ms_flag;
	bool b_tm_1s_flag;
	
	uint8 u8_tm_128us;
	uint8 u8_tm_1ms;
	uint8 u8_tm_25ms;
	uint8 u8_tm_200ms;	
	
	//�͵�ѹ��˸
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
	uint8 u8_sp_date[BITSLOT(24)];  //�������ݴ������
	
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
		_smod	=0b00000001; 	// ϵͳʱ��ѡ��  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000001;	//INTN�жϽ��жϱ��ؿ��ƣ�˫�ش��� PB0��Ϊ����Ӧ�ţ������ʱ����INT0�����ѵ�Ƭ��
		_intc0	=0b00000101; 	//�ж�����
		_intc1	=0b00000000;	//�ж�����
		_mfi0	=0b00000000;	//�๦���ж�����
		_tbc	=0B11000000;  	//ʱ������  tb0   7812.5HZ   128us  
		_wdtc	=0b01010111; 
	}

	void systemhalt()
	{
		_smod	=0b00000001; 	// ϵͳʱ��ѡ��  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000011;	//INTN�жϽ��жϱ��ؿ��ƣ�˫�ش��� PB0��Ϊ����Ӧ�ţ������ʱ����INT0�����ѵ�Ƭ��
		_intc0	=0b00000011; 	//�ж�����
		_intc1	=0b00000000;	//�ж�����
		_mfi0	=0b00000000;	//�๦���ж�����
		_tbc	=0B00000000;  	//ʱ������  tb0   7812.5HZ   128us  
		_wdtc	=0b10101111; 	
	}

	void ioset()
	{
		//7(LCD_BG) 6(LCD_COM) 5(SEG5) 4(CHG_DET) 3(SEG3) 2( ) 1(KEY) 0( )
		_pa		=0b10000010;	//LCD_BG(7) KEY(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ
		_pac	=0b00010110;	//����������� 0��� 1���� CHG_DET(4)��KEY(1)��sp(2)����Ϊ���룬�������	
		_papu	=0b00010010;	//��������  0����  1ʹ��   CHG_DET(4)��KEY(1)��������������������
		_pawu	=0b00000010;	//���ѹ���λ  0���� 1ʹ��	KEY(1)���û��ѣ������޻���
		_pa		=0b10000010;	//LCD_BG(7) KEY(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(LED_OUT1) 1(LED_OUT2) 0(V5V_IN)
		_pb		=0b00000000;	//��ʼȫΪ�͵�ƽ LED1(2) LED2(1) V5V_IN(0)
		_pbc	=0b00000001;	//V5V_IN���ó����룬����Ϊ���		
		_pbpu	=0b00000000;	//ȫ����������
		_pbsr	=0b00000000;	//ȫΪI/O�ڻ�INTx
		_pb		=0b00000000;	//��ʼȫΪ�͵�ƽ LED1(2) LED2(1) V5V_IN(0)

		//7( ) 6( ) 5( ) 4( ) 3( ) 2(SEG1) 1(SEG4) 0(SEG2)
		_pc		=0b00000000;	//��ʼȫΪ�͵�ƽ
		_pcc	=0b00000000;	//��ʼȫΪ���	
		_pcpu	=0b00000000;	//ȫ����������
		_pc		=0b00000000;	//��ʼȫΪ�͵�ƽ
		
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
		_emi   = 0;			//д����ִ��ǰ���ж�λ����
		_iar1 |= 0x08;
		_iar1 |= 0x04;
		_emi   = 1;			//д����ִ�к����ж�λʹ��
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
		if(u8_led_mode==LED_FRONT_WEAK||u8_led_mode==LED_FRONT_STRONG)           //���PWM����
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
		if(b_chg_flag==0)                //δ���
		{
			if(u8_led_mode>1)         //�и���(�������ذ�������޸����м���)
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
			else                   //�޸��أ������������أ�
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
		else   //�ڳ��
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
				if(u8_led_mode<2)             //���ʱ��������
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
				else							//���ʱ��������
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
		b_key=1;///////////////////////////////////////��ʼ״̬Ϊ1
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
				u8_tm_key_down_1ms++;     //�������¼����ʱ
				u8_tm_key_up_1ms++;		  //�����ɿ������ʱ
				u8_tm_key_1ms++;
				if(u8_tm_charge_1ms<21)	  //�������ʱ
				{
					u8_tm_charge_1ms++;
				}
				if(b_lcd_open==1)		  //LCD��ת
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
				u8_tm_key_25ms++;		//�����ж�ʱ��	
				u8_tm_part_one_25ms++; //��һ����˸��ʱ
				u8_tm_part_two_25ms++;	//�ڶ�����˸��ʱ
				u8_tm_lcd_25ms++;      //LCDϨ�𵹼�ʱ
				u8_tm_halt_25ms++;		//���ߵ���ʱ
				u8_tm_sp_25ms++;           //���ݴ��ͼ�ʱ
				if(b_tm_test_mode_flag==0)
				{
					u8_tm_test_mode_25ms++;
				}
			}
			if(b_tm_200ms_flag==1)         //ÿ����ֵ�ڵ�һ�μ���ʱ������1ms~100ms֮������
			{
				b_tm_200ms_flag=0;	
			}
			if(b_tm_1s_flag==1)          //����������ֵ����һ�μ���ʱ������100��900ms֮������
			{
			//	LED_OUT1=1;
				b_tm_1s_flag=0;	
				u16_tm_bat_level_1s++;  //����������¼�ʱ		//��ʱ�䲻���У������
				u8_tm_bat_full_1s++;	//�������������ʱ		//��ʱ�䲻���У������
				u8_tm_part_two_1s++;	//�ڶ�����˸��ʱ        //�����������
				u8_tm_flash_1s++;		//��˸�ж���ʱ          //�����������
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////

////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////
////////////////////////////TESTMODE//////////////////////////////////////////////////////////////////////////
	#if (TEST_FLAG==1)
			if(u8_tm_test_mode_25ms<=20&&b_tm_test_mode_flag==0&&u16_adc_real_vdd>1969)//ֻ��ǰ500ms
			{
			//	LED_OUT2=1;
				if(b_chg_flag==1)
				{
					if(b_key_cont==0)   ///////////////////////  ����ģʽ�����ǻ�����һ���ѹ��ʾ3����
					{
						b_lcd_flag=1;
						u8_test_mode=0;
						u8_bat_level=1;		//�������û���أ�ֱ�ӽ�������������û�а��°�������ô��ʾ3����1��������Զ��л���ͨģʽ
					}
					else                   //�������û���أ�ֱ�ӽ������������Ұ��°������������ģʽ1
					{
						u8_test_mode=1;  ///////////////// ����ģʽ1    	
					}
				}
				else
				{
					if(b_key_cont==1)
					{
						u8_test_mode=2;  /////////////////////����ģʽ2
					}
					else
					{
						u8_test_mode=0;///////////////////////////����ģʽ	
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
			if(u8_tm_sp_25ms>=40)	  //���ݴ���
			{
				u16_adc_sp_vdd=u16_adc_real_vdd;
				for(u8_sp_num=0;u8_sp_num<12;u8_sp_num++)
				{
					if(((u16_adc_sp_vdd>>u8_sp_num)&0b0000000000000001)==1)        //��������
						BITSET(u8_sp_date,u8_sp_num);
					else
						BITCLEAR(u8_sp_date,u8_sp_num);
				}
				
				if(u8_led_mode==0)				//ģʽ����
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
				if(b_chg_flag==1)           //�������
				{
					BITSET(u8_sp_date,14);	
				}
				else
				{
					BITCLEAR(u8_sp_date,14);
				}
				if(b_chg_det_flag==1)		//��������
				{
					BITSET(u8_sp_date,15);	
				}
				else
				{
					BITCLEAR(u8_sp_date,15);
				}
				
				if(b_bat_low_flag==1)		//�͵�ѹ��־����
				{
					BITSET(u8_sp_date,16);	
				}
				else
				{
					BITCLEAR(u8_sp_date,16);
				}
				if(b_flash_flag==1)		//�͵�ѹ��������
				{
					BITSET(u8_sp_date,17);	
				}
				else
				{
					BITCLEAR(u8_sp_date,17);
				}	
				if(b_part_one_flag==1)		//��˸�׶α�־����
				{
					BITSET(u8_sp_date,18);	
				}	
				else
				{
					BITCLEAR(u8_sp_date,18);	
				}
				if(u8_flash_cnt>0)			//��˸Ƶ������
				{
					BITSET(u8_sp_date,19);	
				}	
				else
				{
					BITCLEAR(u8_sp_date,19);	
				}		
				if(u8_bat_level==5)		//ʵʱ��ѹ�ȼ�����
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
			if(V5V_IN==0)         //δ�ڳ��
			{
				if(u8_tm_charge_1ms>20)
				{
					b_lcd_open=0;
				}
				u8_tm_charge_1ms=0;
				b_chg_flag=0;  
				b_chg_det_flag=0;
			}	
			else					 //�����
			{
				if(u8_tm_charge_1ms>=20)
				{
					b_lcd_open=1;
					if(b_lcd_bg==1)
					b_lcd_flag=1;
					b_chg_flag=1; 
				}
			}
			
			if(b_chg_flag==1)  //�жϳ���е�״̬
			{
				if(CHG_DET==0)		//��������
				{
					b_chg_det_flag=1;  
				}
				else				 //δ����
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
				else if((u8_bat_level==0)||(u8_test_mode==10))   //���µ�ѹ�ȼ�
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
				if(b_chg_flag==1&&u8_bat_level>1)  //���嵽2������ϵ�ʱ�򣬽���˸����־λ��λ
				{
					b_flash_flag=0;
				}
			}
	#endif
///////////////////////////BAT//////////////////////////////////////////////////////////////////////////
///////////////////////////BAT//////////////////////////////////////////////////////////////////////////

///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
	#if (FLASH_FLAG==1)   //��˸����
			if(b_chg_flag==1||b_light_flag==0)//���ʱ���ص�ʱ ���Ὣ��˸��ʱ���
			{
				u8_tm_flash_1s=0;
				b_bat_low_flag=0;  //�������������ʱ����˸���ܶ��ᱻǿ�ƹر�
				b_tm_flash_flag=0; //��˸ʱ���ʼ����־
			//	LED_OUT2=0;
			}
			if((b_light_flag==1&&u16_adc_real_vdd>BAT_LOW_LEVEL)||b_flash_flag==1) //����ʱ�����ǵ���������˸��ѹ��Ҳ�Ὣ��˸��ʱ����
			{
				u8_tm_flash_1s=0;	
				if(b_bat_low_flag==0)
				{
					b_tm_flash_flag=0;//��˸ʱ���ʼ����־
				}
			}
			
			/////////////////////////////�ڼ�ʱ֮ǰ�Ȱ�FLASH��TM����
			if(b_light_flag==1&&u16_adc_real_vdd<BAT_LOW_LEVEL&&b_tm_flash_flag==0&&b_bat_low_flag==0)
			{
				u8_tm_200ms=0;
				u8_flash_cnt=0;
				b_tm_flash_flag=1;
			}
			/////////////////////////////�ڼ�ʱ֮ǰ�Ȱ�TM����
			if(u8_tm_flash_1s>=BAT_FLASH_TM&&b_flash_flag==0)
			{
				u8_tm_flash_1s=0;
				b_flash_flag=1;                    //��˸����־
				b_bat_low_flag=1;					//��˸�α�־
				b_part_one_flag=1;					//��һ����˸��־
				u8_led_step=10;
			//	LED_OUT2=1;
			}
			if(b_bat_low_flag==0)          //�����˸�α�־Ϊ0
			{
				b_part_one_flag=0;
				b_part_two_flag=0;
				u8_tm_part_one_25ms=0;
				u8_tm_part_two_25ms=0;
				u8_tm_part_two_1s=0;
			}
			else                          //�����˸�α�־Ϊ1
			{
				if(b_part_one_flag==0)        //������ڵ�һ����˸
				{
					u8_tm_part_one_25ms=0;
				}
				else					//����ڵ�һ����˸����ô�϶������ڵڶ�����˸
				{
					u8_tm_part_two_25ms=0;
					u8_tm_part_two_1s=0;
				}	
		/////////////////////////////��һ������˸��1HZƵ����˸30��
				if(u8_tm_part_one_25ms>=20&&b_part_one_flag==1)
				{
					u8_tm_part_one_25ms=0;
					b_flash_flip_flag=(~b_flash_flip_flag);
					u8_flash_cnt++;
				}
				if(b_part_one_flag==1&&u8_flash_cnt>=FLASH_ONE_CNT)  /////��һ����˸����
				{
					u8_flash_cnt=0;
					b_part_one_flag=0;
					b_flash_flip_flag=0;
	/////////////////////////////////////////////////////
				//	b_flash_flip_flag=0;
				//	u8_led_step=3;
	//////////////////////////////////////////////////////				
					
	//////////////////�ڽ���ڶ��μ�ʱ֮ǰ�ȰѼ�ʱ������
					u8_tm_200ms=0;
	//////////////////�ڽ���ڶ��μ�ʱ֮ǰ�ȰѼ�ʱ������
				//	LED_OUT2=1;

				}
		///////////////////////////�ڶ�������˸��ÿ��һ������˸3��		
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
		//////////////////////////////��˸����־�������
			}
	#endif
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
			if(KEY1!=b_key)                   //�����Լ����ȷ�ϰ���
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
			if(b_key_cont==1&&b_key_trg==1&&b_tm_test_mode_flag==1)     //��������ͨ������ĵ�һ˲��
			{
				b_key_flag=1;					  //��Ĭ���ǽ���LED������
				u8_tm_key_1ms=0;
				u8_tm_key_25ms=0;
			}
			else if(b_key_cont==1&&b_key_trg==0&&b_tm_test_mode_flag==1)  //���³�������
			{
				if(u8_tm_key_25ms>=KEY_LCD_OPEN_TM&&b_key_flag==1)//�������ʱ�䳬���趨ֵ���򽫴˴ΰ�����Ϊ�ǵ���LCD
				{
					b_lcd_close=0;
					b_key_flag=0;         //��֮ǰĬ�ϵ�LED�������
					b_lcd_open=1;
					b_lcd_flag=1;
				}
			}
			else if(b_key_cont==0)  //�����ɿ�
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
					if(u8_tm_key_25ms>=KEY_LED_SHUTDOWN_TM) //�е���������4S
					{
						b_led_shutdown_flag=1;
						eeprom_wirte(u8_led_mode);
					}
				}
				if(b_lcd_open==1&&b_chg_flag==0&&u8_test_mode!=10)   //���LCD���ţ����Ұ����ɿ���ͬʱҲû���ڳ�磬��ô׼���ر�LCD
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
				if(u8_led_mode==LED_NO_BRIGHT)//û�е��� 0
				{
					LED_OUT1=0;
					LED_OUT2=0;
					u8_led_pwm_cnt=LED_PWM_NO;
				}
				else if(u8_led_mode==LED_TOP_BRIGHT)  //�������� 1
				{
					LED_OUT1=1;
					LED_OUT2=0;
					u8_led_pwm_cnt=LED_PWM_NO;
				}
				else if(u8_led_mode==LED_FRONT_WEAK)  //������ 2
				{
					LED_OUT1=0;
					u8_led_pwm_cnt=LED_PWM_WEAK;
				}
				else if(u8_led_mode==LED_FRONT_STRONG) //����ǿ 3
				{
					LED_OUT1=0;
					u8_led_pwm_cnt=LED_PWM_STRONG;
				}
				if(b_bat_low_flag==0)
				{
					u8_led_step=4;             //�ռ�
				}
				else
				{
					u8_led_step=10;				//��˸��
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


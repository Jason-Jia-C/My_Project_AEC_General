	#include "HT66F002.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int 
	
	#define LED_OUT _pa5
	#define LED_WS	_pa6
	
	#define KEY1	_pa1
	#define KEY2	_pa2
	
	#define LED_OPEN			1			//LED��
	#define LED_CLOSE			0			//LED�ر�
	#define LED_WEAK 			1			//������
	#define LED_STRONG 			0			//����ǿ

	#define HALT_START_TM			200			//���߼�ʱ��25msΪ��λ			5S
	#define BAT_FLASH_TM			60			//�͵�����˸����ʱ 1sΪ��λ 	1M
	#define FLASH_TWO_TM			60			//flash�ڶ�����˸���ʱ��		1M
	#define FLASH_ONE_CNT			60			//flash��һ����˸����			60��(30��)
	#define FLASH_TWO_CNT			6			//flash�ڶ�����˸����			6��(3��)
	
	#define BAT_LOW_WEAK_V 			1467 		//��˸��ѹ 2.98v
	#define	BAT_LOW_STRONG_V		1477		//��˸��ѹ 3.0v
	#define BAT_HIGH_V				1773		//��˸��������λ 3.601V
	
	
	#pragma vector tb0_int	@ 0x08 		//tb0�ж�
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	
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
	bool b_part_two_step;
	
	uint8 u8_tm_part_one_25ms;
	uint8 u8_tm_part_two_25ms;
	uint8 u8_tm_part_two_1s;
	uint8 u8_tm_flash_1s;
	uint8 u8_flash_cnt;
	
	//KEY
	bool b_key_errors;
	bool b_key1;
	bool b_key_trg1;
	bool b_key_date1;
	bool b_key_cont1;
	bool b_key2;
	bool b_key_trg2;
	bool b_key_date2;
	bool b_key_cont2;

	uint8 u8_tm_key_1ms;
	
	//ADC
	uint8 u8_adc_cnt;
	uint16 u16_adc_vdd;
	uint16 u16_adc_max;
	uint16 u16_adc_min;
	uint16 u16_adc_sum;
	uint16 u16_adc_steady;

	//LED
	uint8 u8_led_mode;
	
	
	uint8	u8_tm_sp_1ms;
	uint8	u8_tm_sp_25ms;
	bool	b_sp_flag;
	uint16	u16_sp_value;
	uint8	u8_sp_cnt;

		
	#pragma norambank
	

	
	void tb0_int()
	{
		u8_tm_128us++;
		if(u8_tm_128us>7)
		{
			u8_tm_128us=0;
			b_tm_1ms_flag=1;
		}	
	}
	
	
	void main()
	{
		_smod	=0b00000001; 	// ϵͳʱ��ѡ��  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000000;	//INTN�жϽ��жϱ��ؿ��ƣ�˫�ش��� PB0��Ϊ����Ӧ�ţ������ʱ����INT0�����ѵ�Ƭ��
		_intc0	=0b00000101; 	//�ж�����
		_intc1	=0b00000000;	//�ж�����
		_mfi0	=0b00000000;	//�๦���ж�����
		_tbc	=0B11000000;  	//ʱ������  tb0   7812.5HZ   128us  
		_wdtc	=0b01010111; 

		_pa		=0b01000110;	//LED_WS(6) KEY2(2) KEY1(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ
		_pac	=0b00000110;	//����������� 0��� 1���� KEY2(2)��KEY1(1)����Ϊ���룬�������	
		_papu	=0b00000110;	//��������  0����  1ʹ��   KEY2(2)��KEY1(1)��������������������
		_pawu	=0b00000110;	//���ѹ���λ  0���� 1ʹ��  KEY2(2)��KEY1(1)���û��ѣ������޻���
		_pasr	=0b00000000;
		_pa		=0b01000110;	//LCD_BG(7) KEY(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ
////////////////////////////////////////////////////
		b_halt_flag=0;
		u8_tm_halt_25ms=0;
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		b_tm_200ms_flag=0;
		b_tm_1s_flag=0;
		u8_tm_128us=0;
		u8_tm_1ms=0;
		u8_tm_25ms=0;
		u8_tm_200ms=0;	
		b_flash_flag=0;
		b_bat_low_flag=0;
		b_part_one_flag=0;
		b_part_two_flag=0;
		b_part_two_step=0;
		u8_tm_part_one_25ms=0;
		u8_tm_part_two_25ms=0;
		u8_tm_part_two_1s=0;
		u8_tm_flash_1s=0;
		u8_flash_cnt=0;
		b_key_errors=0;
		b_key1=0;
		b_key_trg1=0;
		b_key_date1=0;
		b_key_cont1=0;
		b_key2=0;
		b_key_trg2=0;
		b_key_date2=0;
		b_key_cont2=0;
		u8_tm_key_1ms=0;
		u8_adc_cnt=0;
		u16_adc_vdd=0;
		u16_adc_max=0;
		u16_adc_min=0;
		u16_adc_sum=0;
		u16_adc_steady=0;
		u8_led_mode=0;	
////////////////////////////////////////////////////

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
		b_flash_flag = _eed;
////////////////////////////////////////////////////
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
				u8_tm_key_1ms++;		
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				if(b_part_one_flag==1)
				{
					u8_tm_part_one_25ms++; //��һ����˸��ʱ
				}
				if(b_part_two_flag==1&&b_part_two_step==1)
				{
					u8_tm_part_two_25ms++;	//�ڶ�����˸��ʱ
				}
				u8_tm_halt_25ms++;		//���ߵ���ʱ
				u8_tm_sp_25ms++;
			}
			if(b_tm_200ms_flag==1)         //ÿ����ֵ�ڵ�һ�μ���ʱ������1ms~100ms֮������
			{
				b_tm_200ms_flag=0;	
			}
			if(b_tm_1s_flag==1)          //����������ֵ����һ�μ���ʱ������100��900ms֮������
			{
				b_tm_1s_flag=0;	
				if(b_part_two_flag==1&&b_part_two_step==0)
				{
					u8_tm_part_two_1s++;	//�ڶ�����˸��ʱ        //�����������
				}
				u8_tm_flash_1s++;		//��˸�ж���ʱ          //�����������
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////

	
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////
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
			if(u8_adc_cnt>=20)
			{
				u16_adc_sum-=u16_adc_max;
				u16_adc_sum-=u16_adc_min;
				u16_adc_steady=(u16_adc_sum/18);
				u16_adc_max=0;
				u16_adc_min=0xffff;
				u16_adc_sum=0;
				u8_adc_cnt=0;	
			}
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
///////////////////////////ADC//////////////////////////////////////////////////////////////////////////	


///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////
			if(b_flash_flag==0)
			{
				if(u8_led_mode==0)
				{
					u8_tm_flash_1s=0;
				}
				else
				{
					if(u8_led_mode==1)
					{	
						if(u16_adc_steady>BAT_LOW_WEAK_V)
						{
							u8_tm_flash_1s=0;
						}
					}
					else if(u8_led_mode==2)
					{
						if(u16_adc_steady>BAT_LOW_STRONG_V)
						{
							u8_tm_flash_1s=0;
						}	
					}
				}
			} 
			else
			{
				u8_tm_flash_1s=0;
				if(u8_led_mode==0)
				{
					b_bat_low_flag=0;
				}
				if(u16_adc_steady>BAT_HIGH_V&&b_flash_flag==1)
				{
					b_flash_flag=0;
//////////////////////////////////////////////////////////
					_eea   = 0x01;
					_eed   = b_flash_flag;
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
/////////////////////////////////////////////////////////////				
				}
			}
			if(u8_tm_flash_1s>5&&b_flash_flag==0)
			{
				u8_tm_flash_1s=0;				
				b_flash_flag=1;
//////////////////////////////////////////////////////////
				_eea   = 0x01;
				_eed   = b_flash_flag;
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
/////////////////////////////////////////////////////////////										
				b_bat_low_flag=1;	
				b_part_one_flag=1;
//////////////////�ڽ����һ�μ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////////////		
				u8_tm_1ms=0;
//////////////////�ڽ����һ�μ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////////////
			}
			if(b_bat_low_flag==1)
			{
				if(b_part_one_flag==1)
				{
					if(u8_tm_part_one_25ms>=20)	
					{
						u8_tm_part_one_25ms=0;
						LED_OUT=~LED_OUT;
						u8_flash_cnt++;	
					}
					if(u8_flash_cnt>=FLASH_ONE_CNT)
					{
							u8_flash_cnt=0;
							b_part_one_flag=0;
							b_part_two_flag=1;
							b_part_two_step=0;					
//////////////////�ڽ���ڶ��ε�һ���ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////
							u8_tm_200ms=0;
//////////////////�ڽ���ڶ��ε�һ���ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////
					}
				}
				if(b_part_two_flag==1)
				{
					if(u8_tm_part_two_1s>=60&&b_part_two_step==0)
					{
						u8_tm_part_two_1s=0;
						b_part_two_step=1;
//////////////////�ڽ���ڶ��εڶ����ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////////////		
						u8_tm_1ms=0;
//////////////////�ڽ���ڶ��εڶ����ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////////////
					}
					else if(b_part_two_step==0)
					{
						LED_OUT=1;	
					}
					if(u8_tm_part_two_25ms>=20&&b_part_two_step==1)
					{
						u8_tm_part_two_25ms=0;
						LED_OUT=~LED_OUT;
						u8_flash_cnt++;
					}
					if(u8_flash_cnt>=6)
					{
						u8_flash_cnt=0;
						b_part_two_step=0;
						
//////////////////�ڷ��صڶ��ε�һ���ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////
						u8_tm_200ms=0;
//////////////////�ڷ��صڶ��ε�һ���ּ�ʱ֮ǰ�ȰѼ�ʱ������////////////////////////////////////
					}
				}

			}
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	
///////////////////////////FLASH//////////////////////////////////////////////////////////////////////////	

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
			if(KEY1!=b_key1||KEY2!=b_key2)
			{
				if(u8_tm_key_1ms>35)
				{
					b_key1=KEY1;
					b_key2=KEY2;
					u8_tm_key_1ms=0;
				} 
			}
			else
			{
				u8_tm_key_1ms=0;
				b_key_date1=b_key1^0x01;
				b_key_trg1=b_key_date1&(b_key_date1^b_key_cont1);
				b_key_cont1=b_key_date1;
				
				b_key_date2=b_key2^0x01;
				b_key_trg2=b_key_date2&(b_key_date2^b_key_cont2);
				b_key_cont2=b_key_date2;	
				
				if(b_key_cont1==1&&b_key_cont2==1)
				{
					b_key_errors=1;
				}
				else
				{
					b_key_errors=0;
				}
			}
			if(b_key_errors==0)							//û�з�������
			{
				if(b_key_cont1==1&&b_key_trg1==1)
				{
					LED_OUT=1;
					_pac6=1;
					u8_led_mode=1;	
				}
				else if(b_key_cont2==1&&b_key_trg2==1)
				{
					LED_OUT=1;
					_pac6=0;
					LED_WS=0;
					u8_led_mode=2;
				}
				else if(b_key_cont1==0&&b_key_cont2==0)
				{
					LED_OUT=0;
					u8_led_mode=0;
				}
			}
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////

///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
			if(u8_led_mode!=0)
			{
				u8_tm_halt_25ms=0;
			}
			if(u8_tm_halt_25ms>=HALT_START_TM)
			{				
//////////////////////////////////////////////////////////////////////
				_intc0	=0b00000000; 	
				_intc1	=0b00000000;
				_smod	=0b00000001;	// ϵͳʱ��ѡ��  FSYS=FH
				_smod1	=0b00000000;
				_sadc0	=0B00000000;
				_sadc1	=0B00000000;
				_sadc2	=0B00000000;
				_integ	=0b00000000;	
				_mfi0	=0b00000000;	
				_tbc	=0B00000000;  
				_wdtc	=0b10101111;
				
				_pa		=0b00000110;	//LED_WS(6) KEY2(2) KEY1(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ
				_pac	=0b00000110;	//����������� 0��� 1���� KEY2(2)��KEY1(1)����Ϊ���룬�������	
				_papu	=0b00000110;	//��������  0����  1ʹ��   KEY2(2)��KEY1(1)��������������������
				_pawu	=0b00000110;	//���ѹ���λ  0���� 1ʹ��  KEY2(2)��KEY1(1)���û��ѣ������޻���
				_pasr	=0b00000000;
				_pa		=0b00000110;	//LCD_BG(7) KEY(1)��ʼ�ߵ�ƽ�������ʼ�͵�ƽ	
//////////////////////////////////////////////////////////////////////
				b_halt_flag=1;
				u8_tm_halt_25ms=0;
			}
			if(b_halt_flag==1)
			{
				_nop();
				_nop();
				_halt();
				_nop();
				_nop();
//////////////////////////////////////////////////////////////////////
				_smod	=0b00000001;	// ϵͳʱ��ѡ��  FSYS=FH
				_smod1	=0b00000000;
				_integ	=0b00000000;	//INTN�жϽ��жϱ��ؿ��ƣ�˫�ش��� PB0��Ϊ����Ӧ�ţ������ʱ����INT0�����ѵ�Ƭ��
				_intc0	=0b00000101;	//�ж�����
				_intc1	=0b00000000;	//�ж�����
				_mfi0	=0b00000000;	//�๦���ж�����
				_tbc	=0B11000000;	//ʱ������	tb0   7812.5HZ	 128us	
				_wdtc	=0b01010111; 
//////////////////////////////////////////////////////////////////////
				_pa 	=0b01000110;	
				_pac	=0b00000110;	
				_papu	=0b00000110;	
				_pawu	=0b00000110;	
				_pasr	=0b00000000;
				_pa 	=0b01000110;
				b_halt_flag=0;	
			}
				
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////	
	///////////////////////////HALT//////////////////////////////////////////////////////////////////////////
		}
	}


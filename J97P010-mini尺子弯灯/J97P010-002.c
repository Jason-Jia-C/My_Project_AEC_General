	/*
	��Ŀ����: ���۵�MINI�������
	оƬ�ͺ�: HT66F002 �ڲ�8M ָ������4��Ƶ
	����:
	�������߹��� 0.2uA
	
	2·�����2+��1+����
	
	
	��λ����
				 -------
				|VDD VSS|
		   		|PA6 PA0| KEY
		  LD_SN |PA5 PA1| LD_2 
		   	    |PA7 PA2| LD_1
				 -------
	
	*/
	#include "HT66F002.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int 
	
	#define LD_SN	_pa5
	#define	LD_1	_pa2
	#define LD_1_C	_pac2
	#define LD_2	_pa1
	#define LD_2_C	_pac1
	
	#define KEY1	_pa0
	
	#define HALT_START_TM			200			//���߼�ʱ��25msΪ��λ			5S
	
	#pragma	vector int0_int @ 0x04
	#pragma vector tb0_int	@ 0x08 		//tb0�ж�
	#pragma vector tb1_int	@ 0x0C
	#pragma vector mf0_int	@ 0x10
	#pragma vector eep_int	@ 0x14
	#pragma vector adc_int	@ 0x18

	void int0_int(void)	{_inte=0;_intf=0;}
	void tb1_int(void)	{_tb1e=0;_tb1f=0;}
	void mf0_int(void)	{_mf0e=0;_mf0f=0;}
	void eep_int(void)	{_dee=0;_def=0;}
	void adc_int(void)	{_ade=0;_adf=0;}
	
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	//Clock
	bool b_tm_1ms_flag;
	bool b_tm_25ms_flag;
	bool b_tm_200ms_flag;
	bool b_tm_1s_flag;
	
	uint8 u8_tm_128us;
	uint8 u8_tm_1ms;
	uint8 u8_tm_25ms;
	uint8 u8_tm_200ms;	
	
	//KEY
	bool b_key1;
	bool b_key_trg1;
	bool b_key_date1;
	bool b_key_cont1;
	uint8 u8_tm_key_1ms;

	//LED
	uint8 u8_led_mode;

		
	#pragma norambank
	
	void 	systemset();
	void 	systemhalt();
	void 	ioset();
//	uint8 	eeprom_read();
//	void 	eeprom_wirte(uint8	u8_mode);
	void 	value_csh();
	
	void systemset()	
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
		_tbc	=0B11000011;  	//ʱ������  tb0  976hz   1ms  
		_wdtc	=0b01010111; 
	}

	void systemhalt()
	{
		_smod	=0b00000001; 	// ϵͳʱ��ѡ��  FSYS=FH
		_smod1	=0b00000000;	
		_sadc0	=0B00000000;
		_sadc1	=0B00000000;
		_sadc2	=0B00000000;
		_integ	=0b00000000;	//INTN�жϽ��жϱ��ؿ��ƣ�˫�ش��� PB0��Ϊ����Ӧ�ţ������ʱ����INT0�����ѵ�Ƭ��
		_intc0	=0b00000000; 	//�ж�����
		_intc1	=0b00000000;	//�ж�����
		_mfi0	=0b00000000;	//�๦���ж�����
		_tbc	=0B00000000;  	//ʱ������
		_wdtc	=0b10101111; 	
	}

	void ioset()
	{
		//7() 6(LED_WS) 5(LED_OUT) 4( ) 3( ) 2() 1() 0(  )
		_pa		=0b00000111;	//
		_pac	=0b00000001;	//����������� 	0��� 1����
		_papu	=0b00000001;	//��������  	0���� 1ʹ�� 
		_pawu	=0b00000001;	//���ѹ���λ  	0���� 1ʹ�� 
		_pasr	=0b00000000;
		_pa		=0b00000111;	//
		
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
	}*/
	
	void tb0_int()
	{
		b_tm_1ms_flag=1;
		if(u8_led_mode==1)
		{
			LD_SN=~LD_SN;
		}
	}
	
	void value_csh()
	{
	//HALT
	b_halt_flag=0;
	u8_tm_halt_25ms=0;
	
	//���ʱ��
	b_tm_1ms_flag=0;
	b_tm_25ms_flag=0;
	u8_tm_1ms=0;

	//KEY
	b_key1=0;
	b_key_trg1=0;
	b_key_date1=0;
	b_key_cont1=0;
	u8_tm_key_1ms=0;

	//LED
	u8_led_mode=0;	
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
					b_tm_25ms_flag=1;
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
				u8_tm_halt_25ms++;		//���ߵ���ʱ
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////	


///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
			if(KEY1!=b_key1)
			{
				if(u8_tm_key_1ms>35)
				{
					b_key1=KEY1;
					u8_tm_key_1ms=0;
				} 
			}
			else
			{
				u8_tm_key_1ms=0;
				b_key_date1=b_key1^0x01;
				b_key_trg1=b_key_date1&(b_key_date1^b_key_cont1);
				b_key_cont1=b_key_date1;
			}
			if(b_key_cont1==1&&b_key_trg1==1)
			{
				u8_led_mode++;
				if(u8_led_mode>2)
					u8_led_mode=0;
			}
			if(u8_led_mode==1)
			{
//				LD_SN=1;
				LD_2_C=0;
				LD_2=0;
				LD_1=1;
				LD_1_C=1;
			}
			else if(u8_led_mode==2)
			{
				LD_SN=1;
				LD_1_C=0;
				LD_1=0;
				LD_2=1;
				LD_2_C=1;
			}
			else if(u8_led_mode==0)
			{
				LD_SN=0;
				LD_1=1;
				LD_2=1;
				LD_1_C=1;
				LD_2_C=1;	
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
				b_halt_flag=1;
				systemhalt();
				_pa		=0b00000111;	//
				_pac	=0b00000001;	//����������� 	0��� 1����
				_papu	=0b00000001;	//��������  	0���� 1ʹ�� 
				_pawu	=0b00000001;	//���ѹ���λ  	0���� 1ʹ�� 
				_pasr	=0b00000000;
				_pa		=0b00000111;	//
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


/*
��Ŀ����: CRAֱ���
оƬ�ͺ�: HT66F002 �ڲ�8M ָ������4��Ƶ
����:
�������߹��� 17.9uA

2·���������pin4������pin5���ء�


��λ����
             -------
            |VDD VSS|
   			|PA6 PA0| 
            |PA5 PA1| KEY1 
       LD_1 |PA7 PA2| LD_2
             -------

*/

#include "HT66F002.h"

typedef bit 			bool;
typedef unsigned char	uint8;
typedef unsigned int	uint16;

#define		ROM_Rd_Wt_EN	0
#define		FSK_SPY_EN		0

/////////////////////////////////////////////////////////////

#define		pin4	_14_7
#define		pin5	_14_2
#define		pin6	_14_1

//����
#define		KEY_IN			pin6			//����

//���
#define		LD_1			pin4
#define		LD_2			pin5

#define		LED_ON			0
#define		LED_OFF			1

#define		OUT_ON			1
#define		OUT_OFF			0

#define		KEY_DOWN		0
#define		KEY_UP			1
#define		KEY_SHORT_PRESS_DOWN	20		//20ms
#define		KEY_SHORT_PRESS_UP		50		//50ms
#define		KEY_LONG_PRESS_DOWN		250		//2000ms

#define		IDLE_CNT			25

/////////////////////////////////////////////////////////////
#pragma	rambank0

uint8	u8_tm_1ms_cnt;
bool	b_tm_1ms_flag;

bool	b_tm_1ms_flag2;


bool	b_ep_flag;

uint8	u8_tm_200ms_cnt;
uint16	u16_tm_500ms_cnt;
uint8	u8_tm_5s_timeout;

uint8	u8_tm_idle_timeout;

uint16	u16_tm_1s_cnt;
uint8	u8_tm_4min_cnt;
bool	b_tm_4min_trig;
bool	b_tm_frist_5h_flag;

uint8	u8_sw_high_cnt;
uint8	u8_sw_low_cnt;
uint8	u8_sw_status;

uint8	u8_key_value;
uint8	u8_key_value_old;
uint8	u8_key_down_cnt;
uint8	u8_key_up_cnt;
bool	b_key_up_flag;
uint8	u8_key_down_long_cnt;

uint8	u8_loader_mode;

uint8	u8_ld_out1;
uint8	u8_ld_out2;

bool	b_ee_rd_en;
bool	b_ee_wt_en;

//adc param
uint16	u16_adc_vdd,u16_adc_vdd_avg;
uint16	u16_adc_vdd_min, u16_adc_vdd_max, u16_adc_vdd_sum;
uint8	u8_adc_vdd_cnt;
//adc param end


#pragma	norambank
/////////////////////////////////////////////////////////////
#pragma 	vector		ext_int		@ 0x04		//�ⲿ�ж�
#pragma 	vector		tb0_int		@ 0x08		//Time Base 0
#pragma 	vector		tb1_int		@ 0x0C		//Time Base 1
#pragma 	vector		mf0_int		@ 0x10 		//�๦��0 stm�ж�
#pragma		vector		eeprom_int	@ 0x14		//EEPROM
#pragma 	vector		ad_int		@ 0x18		//AD

/////////////////////////////////////////////////////////////

void	ext_int			(void){}
void	eeprom_int		(void){}
void    ad_int		    (void){}

/*
 * ����8M, fSYS=8M
 * fTB = fSYS/4 = 2M = 0.5us
 * tb0 = 256*0.5us = 128us
 */
void tb0_int(void)		//128us
{
	u8_tm_1ms_cnt++;
	if(u8_tm_1ms_cnt > 7)				//1ms
	{
		u8_tm_1ms_cnt = 0;
		b_tm_1ms_flag = 1;
		b_tm_1ms_flag2 = 1;
	}
}

void tb1_int()			//16.384ms
{
}

void mf0_int(void)		//125us
{
	//���PWM��������
	/////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////
void main()
{
	/////////////////////////////////////////////////////////////
//	ramclr();
//	_rstc	= 0B01010101;		//��λѡ��Ĵ���
	_bp=0;
	for(_mp0=0x40;_mp0<=0x7F;_mp0++)
	{
		_iar0=0;
	}
	/////////////////////////////////////////////////////////////
//	init();
	_rstc	= 0B01010101;		//��λѡ��Ĵ���
	
	_pasr	= 0B00000000;		//PA5,PA2,PA1,PA0����ѡ�� IO
	_pa		= 0B00000010;		//��ƽ		1:��;   0:��
	_pac	= 0B00000010;		//�������	1:����; 0:���
	_pawu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
	_papu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
	_pa		= 0B00000010;		//��ƽ		1:��;   0:��
	_ifs0	= 0B00110000;		//<5>STCK0PS; <4>STP0IPS; <1:0>INT��λ
	
	_sadc0	= 0B00000000;    	//<7>START; <6>ADBZ; <5>ENADC; <4>ADRFS; <1:0>CH
	_sadc1	= 0B00000000;		//<7-5>�����ź�ѡ��; <2-0>ʱ��Դ
	_sadc2	= 0B00000000;		//<7>ENOPA; <6>VBGEN; <3-0>�ο���ѹ

	_smod	= 0B00000001;		//<7-5>��Ƶѡ��; <3>LTO; <2>HTO; <1>IDLEN; <0>HLCLK
	_smod1	= 0B00000000;		//<7>FSYSON; <3>RSTF; <2>LVRF; <0>WRF
	_tbc	= 0B11000000;		//<7>TBON; <6>TBCK=fsys/4; <5:4>TB1=32768/fTB; <2-0>TB0=256/fTB 
	
	/*
	PTnCK=fSYS:		001 7.8K
	PTnCK=fSYS/4:	000 2K
	PTnCK=fH/16:	010 500Hz
	PTnCK=fH/64:	011 128Hz
	PTnCK=fTBC:		100 26.6Hz;
	*/
	_stm0c0	= 0B10000100;		//<7>ST0PAU; <6-4>fSYS; <3>ST0ON; <2-0>CCRP STM[9-7]�Ƚ�ƥ������256
	_stm0c1	= 0B00000000;		//<7:6>����ģʽ; <5:4>�������; <3>�������λ; <2>�������; <1>����/ռ�ձ�; <0>P/Aƥ��
	
	_mfi0	= 0B00000000;		//<1>STMA0E; <0>STMP0E
	_integ	= 0B00000000;		//<1:0>�ⲿ�жϱ���ѡ��λ
	_intc0	= 0B00000101;		//<3>TB1E; <2>TB0E; <1>INTE; <0>EMI
	_intc1	= 0B00000000;		//<2>ADE; <1>DEE; <0>MF0E
	
	_wdtc	= 0B01010111;		//<7-3>WE; <2-0>WS
	/////////////////////////////////////////////////////////////
	b_ep_flag=0;
	u8_loader_mode = 0;
	u8_ld_out1 = 0;
	u8_ld_out2 = 0;
	u8_tm_idle_timeout = IDLE_CNT;
	u8_key_value = 0;
	u8_key_value_old = 0;
//	b_ee_rd_en = 1;
	u8_sw_pwm_tel = 0;

	u8_adc_bat_level = 1;
	u8_adc_bat_level_new = 1;
	b_ld_flash_flag = 0;
	u16_ld_flash_cnt = 0;
	u16_adc_vdd_avg = 0xFFFF;
	u16_adc_vdd_max = 0;
	u16_adc_vdd_min = 0xFFFF;
	u16_adc_vdd_sum = 0;
	u8_adc_vdd_cnt = 0;
	while(1)
	{
		_clrwdt();				//���1������һ��
		/////////////////////////////////////////////////////////////
		if(b_tm_1ms_flag == 1)
		{
			b_tm_1ms_flag = 0;
			u8_key_down_cnt++;
			u8_key_up_cnt++;
			
			u8_tm_200ms_cnt++;
			if(u8_tm_200ms_cnt > 199)		//200ms
			{
				u8_tm_200ms_cnt = 0;
				
				if(u8_tm_idle_timeout)
					u8_tm_idle_timeout--;
				
			}
			if(u8_ld_out1 == 1)
			{
				LD_1 = 1;
				LD_2 = 0;
			}
			else if(u8_ld_out2 == 1)
			{
				LD_1 = 0;
				LD_2 = 1;
			}
			else
			{
				LD_1 = 0;
				LD_2 = 0;
			}
			
		}
		//��������
		if(KEY_IN == KEY_DOWN)
		{
			if(u8_key_down_cnt >= KEY_LONG_PRESS_DOWN)
			{
				u8_key_down_cnt = KEY_SHORT_PRESS_UP+1;
			}
			else if((u8_key_down_cnt >= KEY_SHORT_PRESS_DOWN) && (u8_key_down_cnt < KEY_SHORT_PRESS_UP))
			{
				u8_key_down_cnt = KEY_SHORT_PRESS_UP+1;
				if(b_key_up_flag)
				{
					b_key_up_flag = 0;
					u8_key_up_cnt = 0;
					u8_key_value = 0x01;
				}
			}
		}
		else
		{
			u8_key_down_cnt = 0;
			u8_key_down_long_cnt = 0;
			u8_key_value = 0x00;
			if(u8_key_up_cnt >= KEY_SHORT_PRESS_UP)
			{
				b_key_up_flag = 1;
				u8_key_up_cnt = KEY_SHORT_PRESS_UP;
			}
		}
		
		if(u8_key_value_old != u8_key_value)
		{
			u8_key_value_old = u8_key_value;
			if(u8_key_value == 0x01)
			{
				u8_key_value = 0;
				//��������һ��
				if(u8_loader_mode == 1)
				{
					u8_loader_mode = 2;	
				}
				else if(u8_loader_mode == 2)
				{
					u8_loader_mode = 0;	
				}
				else
				{
					u8_loader_mode = 1;	
				}
			}
		}

		
		////////////////////////////////////////////////////////////*/
		//���⹦��
		if(u8_loader_mode == 0)
		{
			u8_ld_out1 = 0;
			u8_ld_out2 = 0;
		}
		else if(u8_loader_mode == 2)		//����
		{
			u8_ld_out1 = 0;
			u8_ld_out2 = 1;
			
			u8_tm_idle_timeout = IDLE_CNT;
		}
		else if(u8_loader_mode == 1)		//��
		{
			u8_ld_out1 = 1;
			u8_ld_out2 = 0;
			
			u8_tm_idle_timeout = IDLE_CNT;
		}

		/////////////////////////////////////////////////////////////
		//���߻���
		if(u8_tm_idle_timeout == 0)
		{
			//*
			//׼������
			_pasr	= 0B00000000;		//PA5,PA2,PA1,PA0����ѡ�� IO
			_pa		= 0B00000010;		//��ƽ		1:��;   0:��
			_pac	= 0B00000010;		//�������	1:����; 0:���
			_pawu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
			_papu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
			_pa		= 0B00000010;		//��ƽ		1:��;   0:��
			_ifs0	= 0B00110000;		//<5>STCK0PS; <4>STP0IPS; <1:0>INT��λ
	
			_sadc0	= 0B00000000;    	//<7>START; <6>ADBZ; <5>ENADC; <4>ADRFS; <1:0>CH
			_sadc1	= 0B00000000;		//<7-5>�����ź�ѡ��; <2-0>ʱ��Դ
			_sadc2	= 0B00000000;		//<7>ENOPA; <6>VBGEN; <3-0>�ο���ѹ

			
			_clrwdt();
			_wdtc	= 0B10101111;		//<7-3>WE; <2-0>WS
			
			_intc0	= 0B00000000;
			_intc1	= 0B00000000;
			_nop();
			_nop();
			_halt();
			_nop();
			_nop();
			
			_wdtc	= 0B01010111;
			_clrwdt();
			
			_mfi0	= 0B00000000;		//<1>STMA0E; <0>STMP0E
			_integ	= 0B00000000;		//<1:0>�ⲿ�жϱ���ѡ��λ
			_intc0	= 0B00000101;		//<3>TB1E; <2>TB0E; <1>INTE; <0>EMI
			_intc1	= 0B00000000;		//<2>ADE; <1>DEE; <0>MF0E
			
			_pasr	= 0B00000000;		//PA5,PA2,PA1,PA0����ѡ�� IO
			_pa		= 0B00000010;		//��ƽ		1:��;   0:��
			_pac	= 0B00000010;		//�������	1:����; 0:���
			_pawu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
			_papu	= 0B00000010;		//����		1:ʹ��; 0:��ֹ
			_pa		= 0B00000010;		//��ƽ		1:��;   0:��
			
			u8_tm_idle_timeout = IDLE_CNT;
			b_key_up_flag = 1;
			//*/
		}
		/////////////////////////////////////////////////////////////
#if (ROM_Rd_Wt_EN == 1)
		//��EEPROM����
		if(b_ee_rd_en)
		{
			b_ee_rd_en = 0;

			_eea = 0;
			_mp1=0x40;
			_bp=1;
			_iar1 |= 0x02;
			_iar1 |= 0x01;
			while((_iar1&0x01) == 0x01)
			{
				_clrwdt();
			}
			_iar1=0;
			_bp=0;
			u8_loader_mode = _eed;
			
			u8_tm_idle_timeout = IDLE_CNT;
		}
		/////////////////////////////////////////////////////////////
		//дEEPROM����
		if(b_ee_wt_en)
		{
			b_ee_wt_en = 0;
			
			_eea = 0;
			_eed = u8_loader_mode;
			u8_loader_mode=0;
			_mp1 = 0x40;
			_bp=1;
			_emi = 0;			//д����ִ��ǰ���ж�λ����
			_iar1 |= 0x08;
			_iar1 |= 0x04;
			_emi = 1;			//д����ִ�к����ж�λʹ��
			while((_iar1&0x04) == 0x04)
			{
				_clrwdt();
			}
			_iar1=0;
			_bp=0;
			
			u8_tm_idle_timeout = IDLE_CNT;
		}
#endif
		/////////////////////////////////////////////////////////////
	}
}



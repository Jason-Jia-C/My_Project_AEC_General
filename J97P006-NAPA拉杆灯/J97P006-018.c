	/*
		������ʾ��������
		��1�� ��δ���ʱ����ص����ȼ�ֻ�����½�����������
		��2�� �ڳ��ʱ����ص����ȼ�ֻ���������������½�
		��3�� �ڵ����ȼ������ı�ʱ������ȴ�����5�������ϲ��ܽ�����һ�θı�
*/
	#include "HT66F018.h"
	
	#define	bool	bit
	#define	uint8	unsigned char 
	#define uint16	unsigned int
	
	#define	KEY1		_pa0
	#define LED_SN		_pb4
	#define LED_OUT		_pa7
	
	#define	CHG_F		_pb0		//����־λ		����Ч	
	#define	CHG_V		_pa4		//���������λ	
	#define BAT_V		_pb1		//��ص�ѹ���λ
	#define	CHG_SN		_pb3		//���ʹ��			����Ч
	#define	SN_385		_pb5		//385ʹ��			����Ч
	
	#define L_1			_pa5
	#define	L_2			_pa6
	#define L_3			_pa1
	#define	L_4			_pa2
	#define	L_5			_pa3
	
	#define ADC_CHG_V	0  
	#define ADC_CHG_A	3
	#define ADC_BAT_V	1
	
	#define	CHG_V_LEVEL_0 428			//0�񣬵�һ������˸
	#define CHG_V_LEVEL_1 352			//1�񣬵ڶ�������˸
	#define CHG_V_LEVEL_2 238			//2�񣬵���������˸	
	#define CHG_V_LEVEL_3 145			//3�񣬵��ĸ�����˸
	#define CHG_V_LEVEL_4 86			//4�񣬵��������˸
		
	#define	BAT_V_LEVEL_1 2406
	#define	BAT_V_LEVEL_2 2560
	#define	BAT_V_LEVEL_3 2635
	#define	BAT_V_LEVEL_4 2721
	#define	BAT_V_LEVEL_5 2843

	#define	OUT_CHG_VALUE 25			//�˳����ĵ���ֵ
	
	
	/////////////////////////////////////////////////////////////
	#pragma		vector		int0_int	@ 0x04		//�����ж�
	#pragma		vector		cpe_int		@ 0x08		//��ѹ�ж�
	#pragma 	vector		mf0f_int	@ 0x0C		//�ⲿ�ж�0
	#pragma 	vector		mf1f_int	@ 0x10 		//�๦��0 stm�ж�
	#pragma 	vector		mf2f_int	@ 0x14		//ADC
	#pragma		vector		AD_int		@ 0x18		//EEPROM
	#pragma 	vector		tb0_int		@ 0x1C		//Time Base 0
	#pragma 	vector		tb1_int		@ 0x20		//Time Base 1
	#pragma 	vector		int1_int	@ 0x24		//�ⲿ�ж�1
	/////////////////////////////////////////////////////////////
	void	cpe_int			(void){_cpe=0;_cpf=0;}
	void	mf0f_int		(void){_mf0e=0;_mf0f=0;}
	void	mf1f_int		(void){_mf1e=0;_mf1f=0;}
	void	mf2f_int		(void){_mf2e=0;_mf2f=0;}
	void	AD_int			(void){_ade=0;_adf=0;}
	void	tb1_int			(void){_tb1e=0;_tb1f=0;}
	void	int1_int		(void){_int1e=0;_int1f=0;}
	void	int0_int		(void){_int0e = 0; _int0f = 0;}
	//////////////////////////////////////////////////////////////
	
	 
	#pragma rambank0
	//tb0_int
	bool 	b_tm_1ms_flag;
	bool 	b_tm_25ms_flag;
	bool 	b_tm_500ms_flag;
	

	uint8 	u8_tm_1ms;
	uint8 	u8_tm_25ms;
		
	//ADC
	bool	b_ADC_finish_flag;
	
	uint8	u8_ADC_cnt;
	uint8	u8_ADC_mode;

	
	
	//CHG
	bool	b_chg_flag;
	bool	b_chg_start_flag;
	bool	b_chg_end_flag;
	bool	b_chg_A_low_flag;
	
	uint8	u8_chg_start_cnt;
	uint8	u8_chg_end_cnt;
	uint8	u8_chg_V_low_cnt;


	//KEY&LED
	bool 	b_key1;
	bool 	b_key_trg1;
	bool 	b_key_date1;
	bool 	b_key_cont1;
	bool 	b_key1_flag;
	
	uint8 	u8_tm_key_check_1ms1;
	uint8 	u8_tm_key_25ms;
	uint8	u8_led_step;
	uint8	u8_led_mode;
	
	//ld_out
	bool	b_ld_flag;	
	bool	b_ld_display_flag;
	bool	b_ld_flash_flag;
	bool 	b_tm_ld_500ms_flag;
	
	uint8	u8_ld_level;
	uint8	u8_tm_ld_close_25ms;
	
	//LEVLE
	bool	b_level_change_flag;
	uint8	u8_bat_level_csh_cnt;
	uint8	u8_bat_level;
	uint8	u8_chg_level;
	uint8	u8_level_add_cnt;
	uint8	u8_level_cut_cnt;
	uint8	u8_tm_level_uc_500ms;
	uint16	u16_level_refer_value;
	
	//SP
	bool 	b_sp_flag;
	uint8 	u8_tm_sp_25ms;
	uint8 	u8_sp_cnt;
	uint16	u16_sp_buf_value;
	
	#pragma	norambank
	
	#pragma	rambank 1 ds1


	uint16	u16_adc_chg_V_sum;
	uint16	u16_adc_chg_A_sum;
	uint16	u16_adc_bat_V_sum;

	uint16	u16_adc_chg_V_max;
	uint16	u16_adc_chg_A_max;
	uint16	u16_adc_bat_V_max;

	uint16	u16_adc_chg_V_min;
	uint16	u16_adc_chg_A_min;
	uint16	u16_adc_bat_V_min;

	uint16	u16_adc_chg_V_final;
	uint16	u16_adc_chg_A_final;
	uint16	u16_adc_bat_V_final;
	uint16  u16_adc_value;
//sp	
	uint8	u8_sp_value[16];
	
	
	#pragma	norambank	



	void systemset()	
	{
		_smod	=0b11100001;	// ϵͳʱ��ѡ��  FSYS=FH
		_wdtc	=0b01010111;	// ���Ź���ʱ��ʹ��

		_tmpc	=0b00000000;
		
		_tm0c0	=0b00000000;
		_tm1c0	=0b00000000;
		_tm2c0	=0b00000000;
		_tm0c1	=0b00000000;
		_tm1c1	=0b00000000;
		_tm2c1	=0b00000000;
		
		_adcr0	=0b00100000;	//�ر�AD
		_adcr1	=0b00000000;
		_acerl	=0b00000000;
		
		_cpc	=0b00001000;	//�Ƚ���
		
		_integ	=0b00000000;	//�ⲿ�ж�
		_intc0	=0b00000001;	//���ܿ���
		_intc1	=0b00001000;	//��ʱ���ж�0
		_intc2	=0b00000000;
		_mfi0	=0b00000000;
		_mfi1	=0b00000000;
		_mfi2	=0b00000000;
		
		_tbc	=0b11000011;	//ftb=fsys/4	2048	1ms
	}
	void systemhalt()
	{
		_smod	=0b11100001;	// ϵͳʱ��ѡ��  FSYS=FH
		_wdtc	=0b10101111;	// ���Ź���ʱ������

		_tmpc	=0b00000000;
		
		_tm0c0	=0b00000000;
		_tm1c0	=0b00000000;
		_tm2c0	=0b00000000;
		_tm0c1	=0b00000000;
		_tm1c1	=0b00000000;
		_tm2c1	=0b00000000;
		
		_adcr0	=0b00100000;	//�ر�AD
		_adcr1	=0b00000000;
		_acerl	=0b00000000;
		
		_cpc	=0b00001000;	//�Ƚ���
		
		_integ	=0b00000011;	//�ⲿ�ж�
		_intc0	=0b00000011;	//���ܿ��� ���ⲿ�ж�0
		_intc1	=0b00000000;
		_intc2	=0b00000000;
		_mfi0	=0b00000000;
		_mfi1	=0b00000000;
		_mfi2	=0b00000000;
		
		_tbc	=0b00000000;	//ftb=fsys/4	2048	1ms
	}
	void ioset()
	{
		_pa		=0b01101111;	// 7 pwm 	6 L2 	5 L1	4 CHG_V	3 L5	2 L4	1 L3	0 KEY
		_pac	=0b00010001;
		_papu	=0b00000001;
		_pawu	=0b00000001;
		_pa		=0b01101110;
		
		_pb		=0b00100000;	// 7 N0		6 N0	5 385_SN	4 LED_SN	3 CHG_SN	2 NO	1 BAT_V		0	CHG	
		_pbc	=0b00000011;
		_pbpu	=0b00000000;
		_pb		=0B00100000;
		
		_pc		=0b00000000;
		_pcc	=0b00000000;
		_pcpu	=0b00000000;
		_pc		=0b00000000;
	}
	void tb0_int()
	{
		b_tm_1ms_flag=1;
		if(u16_sp_buf_value > 0)				//������ 970
		{
			_pc2 = (u16_sp_buf_value & 0x01);
			u16_sp_buf_value >>= 1;
		}
		else
		{
			b_sp_flag = 0;
		}
	}
	void stm_out()
	{
		if(u8_led_mode==0)
		{
			LED_SN	=0;
			_tmpc	=0b10000010;	//���ſ���λʹ�ܣ�TP1ʹ��
			_tm1rpl	=0b00000000;	//����
			_tm1rph	=0b00000000;	
			_tm1al	=0b00000000;	//ռ�ձ�
			_tm1ah	=0b00000000;
			_tm1c0	=0b00000000;	//7  0���� 1��ͣ	6-4 ʱ��ѡ��fs/4	3 0 off	1 on	
			_tm1c1	=0b10101000;	//ѡ��PWM���	����Ч
			
		}
		else if(u8_led_mode==1)
		{
			LED_SN	=1;
			_tmpc	=0b10000010;	//���ſ���λʹ�ܣ�TP1ʹ��
			_tm1rpl	=0b11111111;	//����255
			_tm1rph	=0b00000000;	
			_tm1al	=0b10000000;	//ռ�ձ�128
			_tm1ah	=0b00000000;
			_tm1c0	=0b00001000;	
			_tm1c1	=0b10101000;
		}
		else if(u8_led_mode==2)
		{
			LED_SN	=1;
			_tmpc	=0b10000010;	//���ſ���λʹ�ܣ�TP1ʹ��
			_tm1rpl	=0b11111111;	//����255
			_tm1rph	=0b00000000;	
			_tm1al	=0b11111111;	//ռ�ձ�128
			_tm1ah	=0b00000000;
			_tm1c0	=0b00001000;	
			_tm1c1	=0b10101000;	
		}
	}
	void Get_ADC_Value(uint8 MODE)
	{
		_acerl	=0b00001011;
		_adcr0	=(0b01010111&(0xF8|MODE));	//111&(0��1��3) 
		_adcr1	=0b00000110;	//�ο���ѹѡ��V5V ʱ��ѡ��fsys/64 110
		_start	=0;
		_start	=1;
		_start	=0;
		while(_eocb);
		u16_adc_value=((uint16)_adrh<<8)+(_adrl);
	}
	void ADC_Handle()
	{
		if(u8_ADC_mode==ADC_CHG_V)
		{
			Get_ADC_Value(u8_ADC_mode);
			if(u16_adc_value>u16_adc_chg_V_max)
				u16_adc_chg_V_max=u16_adc_value;
			if(u16_adc_value<u16_adc_chg_V_min)
				u16_adc_chg_V_min=u16_adc_value;
			u16_adc_chg_V_sum+=u16_adc_value;
			u8_ADC_mode=ADC_CHG_A;
		}
		else if(u8_ADC_mode==ADC_CHG_A)
		{	
			Get_ADC_Value(u8_ADC_mode);
			if(u16_adc_value>u16_adc_chg_A_max)
				u16_adc_chg_A_max=u16_adc_value;
			if(u16_adc_value<u16_adc_chg_A_min)
				u16_adc_chg_A_min=u16_adc_value;
			u16_adc_chg_A_sum+=u16_adc_value;
			u8_ADC_mode=ADC_BAT_V;
		}
		else if(u8_ADC_mode==ADC_BAT_V)
		{
			Get_ADC_Value(u8_ADC_mode);
			if(u16_adc_value>u16_adc_bat_V_max)
				u16_adc_bat_V_max=u16_adc_value;
			if(u16_adc_value<u16_adc_bat_V_min)
				u16_adc_bat_V_min=u16_adc_value;
			u16_adc_bat_V_sum+=u16_adc_value;
			u8_ADC_mode=ADC_CHG_V;
			u8_ADC_cnt++;
		}
		if(u8_ADC_cnt>=10)
		{
			u8_ADC_cnt=0;
			//CHG_V
			u16_adc_chg_V_sum-=u16_adc_chg_V_max;
			u16_adc_chg_V_sum-=u16_adc_chg_V_min;
			u16_adc_chg_V_sum>>=3;
			u16_adc_chg_V_final=u16_adc_chg_V_sum;
			u16_adc_chg_V_sum=0;
			u16_adc_chg_V_max=0;
			u16_adc_chg_V_min=0xFFFF;
			//CHG_A
			u16_adc_chg_A_sum-=u16_adc_chg_A_max;
			u16_adc_chg_A_sum-=u16_adc_chg_A_min;
			u16_adc_chg_A_sum>>=3;
			u16_adc_chg_A_final=u16_adc_chg_A_sum;
			u16_adc_chg_A_sum=0;
			u16_adc_chg_A_max=0;
			u16_adc_chg_A_min=0xFFFF;
			
			//BAT_V
			u16_adc_bat_V_sum-=u16_adc_bat_V_max;
			u16_adc_bat_V_sum-=u16_adc_bat_V_min;
			u16_adc_bat_V_sum>>=3;
			u16_adc_bat_V_final=u16_adc_bat_V_sum;
			u16_adc_bat_V_sum=0;
			u16_adc_bat_V_max=0;
			u16_adc_bat_V_min=0xFFFF;
			b_ADC_finish_flag=1;
		}
	}
	void status_judge()
	{
		if(b_ADC_finish_flag==1)			//���һ��AD�������
		{
			if(b_chg_flag==0)				//�������־Ϊ0
			{
				if(u16_adc_chg_V_final>3316&&u16_adc_chg_V_final<3930)	//��������ڵĵ�ѹ����4.05V ������С��4.8V
					u8_chg_start_cnt++;
				else
					u8_chg_start_cnt=0;
				if(u8_chg_start_cnt>5)		//ȷ���Ѿ�������״̬
				{
					b_chg_flag=1;			//����־
					b_chg_start_flag=1;		//��翪ʼ��־
					b_chg_end_flag=0;
					b_ld_display_flag=1;	//������ʾ��־
					u8_chg_start_cnt=0;
				}
			}
			if(b_chg_flag==1)				//�������־Ϊ1		
			{
				if(u16_adc_chg_A_final<50)	//���������С��50
					b_chg_A_low_flag=1;		//����������־��1
				else
					b_chg_A_low_flag=0;
			}
			if(b_chg_A_low_flag==1)			//������͵�����־Ϊ1
			{
				if(u16_adc_chg_V_final<3316)//�����ѹС��3316V
					u8_chg_V_low_cnt++;
				if(u8_chg_V_low_cnt>3)			
				{
					b_chg_flag=0;
					b_chg_start_flag=0;
					b_chg_end_flag=1;
					b_chg_A_low_flag=0;
					b_ld_display_flag=0;		//������ʾ��־
					u8_chg_V_low_cnt=0;
				}
			}
			
			if(b_chg_flag==0)			//�������־λΪ1
			{
				SN_385=1;					//385����
				CHG_SN=0;					//���оƬ����
			}
			else							//�������־λΪ0
			{
				
				SN_385=0;						//385����
				if(b_chg_A_low_flag==0)			//���͵�����־
					CHG_SN=1;					//���оƬʹ��
		//		else
		//			���ж�����ÿ1MSȡ��һ��
			}
//			b_ADC_finish_flag=0;
		}
	}	
	void V_level()
	{
		if(b_chg_start_flag==1||b_chg_end_flag==1)
		{
			if(b_chg_start_flag==1)
			{
				u8_chg_level=(u8_bat_level-1);
			}
			else if(b_chg_end_flag==1)
			{
				if(u8_chg_level<5)
					u8_bat_level=(u8_chg_level+1);
				else
					u8_bat_level=u8_chg_level;
			}
			if(u8_tm_level_uc_500ms>120)
			{
				b_chg_start_flag=0;
				b_chg_end_flag=0;
				u8_level_add_cnt=0;
				u8_level_cut_cnt=0;
				u8_tm_level_uc_500ms=0;
			}
			b_ADC_finish_flag=0;
		}
		else
		{
			if(b_ADC_finish_flag==1)
			{
				if(b_chg_flag==0)
				{
					if(u8_bat_level==1)
						u16_level_refer_value=BAT_V_LEVEL_1;
					else if(u8_bat_level==2)
						u16_level_refer_value=BAT_V_LEVEL_2;
					else if(u8_bat_level==3)
						u16_level_refer_value=BAT_V_LEVEL_3;
					else if(u8_bat_level==4)
						u16_level_refer_value=BAT_V_LEVEL_4;
					else if(u8_bat_level==5)
						u16_level_refer_value=BAT_V_LEVEL_5;
					if(u16_adc_bat_V_final<u16_level_refer_value)
						u8_level_cut_cnt++;
					else
						u8_level_cut_cnt=0;
				}
				else
				{
					if(u8_chg_level==0)
						u16_level_refer_value=CHG_V_LEVEL_0;
					else if(u8_chg_level==1)
						u16_level_refer_value=CHG_V_LEVEL_1;
					else if(u8_chg_level==2)
						u16_level_refer_value=CHG_V_LEVEL_2;
					else if(u8_chg_level==3)
						u16_level_refer_value=CHG_V_LEVEL_3;
					else if(u8_chg_level==4)
						u16_level_refer_value=CHG_V_LEVEL_4;
					if(u16_adc_chg_A_final<u16_level_refer_value)
						u8_level_add_cnt++;
					else
						u8_level_add_cnt=0;
				}
				if(u8_level_add_cnt>250)
				{
					if(u8_chg_level<5)
					u8_chg_level++;
					u8_level_add_cnt=0;
				}
				else if(u8_level_cut_cnt>250)
				{
					if(u8_bat_level>0)
					u8_bat_level--;
					u8_level_cut_cnt=0;
				}
				b_ADC_finish_flag=0;
			}
			
		}
	}	
	void ld_display()
	{
		if(b_ld_display_flag==1)
		{
			if(b_chg_flag==0)
				u8_ld_level=u8_bat_level;
			else
				u8_ld_level=u8_chg_level;
			if(u8_ld_level==1)
			{
				LED_SN=1;L_1=0;L_3=1;L_4=1;L_5=1;	
				if(b_chg_flag==0)
					L_2=1;
			}	
			else if(u8_ld_level==2)
			{
				LED_SN=1;L_1=0;L_2=0;L_4=1;L_5=1;	
				if(b_chg_flag==0)
					L_3=1;			
			}
			else if(u8_ld_level==3)
			{
				LED_SN=1;L_1=0;L_2=0;L_3=0;L_5=1;		
				if(b_chg_flag==0)
					L_4=1;		
			}
			else if(u8_ld_level==4)
			{
				LED_SN=1;L_1=0;L_2=0;L_3=0;L_4=0;
				if(b_chg_flag==0)
					L_5=1;			
			}
			else if(u8_ld_level==5)
			{
				LED_SN=1;L_1=0;L_2=0;L_3=0;L_4=0;L_5=0;				
			}
			if(b_tm_ld_500ms_flag==1)
			{
				if(u8_ld_level==0)
						L_1=~L_1;
				if(b_chg_flag==1)
				{	
					if(u8_ld_level==1)
						L_2=~L_2;
					else if(u8_ld_level==2)
						L_3=~L_3;
					else if(u8_ld_level==3)
						L_4=~L_4;
					else if(u8_ld_level==4)
						L_5=~L_5;
				}
				b_tm_ld_500ms_flag=0;
			}
			
		}
		else
		{
			
			LED_SN=0;L_1=1;L_2=1;L_3=1;L_4=1;L_5=1;	
		}
	}
	void value_csh()
	{
		//tb0_int
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		b_tm_500ms_flag=0;
		u8_tm_1ms=0;
		u8_tm_25ms=0;
			
		//ADC
		b_ADC_finish_flag=0;
		u8_ADC_cnt=0;
		u8_ADC_mode=0;
		u16_adc_chg_V_sum=0;
		u16_adc_chg_A_sum=0;
		u16_adc_bat_V_sum=0;
	
		u16_adc_chg_V_max=0;
		u16_adc_chg_A_max=0;
		u16_adc_bat_V_max=0;
	
		u16_adc_chg_V_min=0xFFFF;
		u16_adc_chg_A_min=0xFFFF;
		u16_adc_bat_V_min=0xFFFF;
	
		u16_adc_chg_V_final=0;
		u16_adc_chg_A_final=0;
		u16_adc_bat_V_final=0;
		u16_adc_value;
		
		
		//CHG
		b_chg_flag=0;
		b_chg_flag=0;
		b_chg_start_flag=0;
		b_chg_end_flag=0;
		b_chg_A_low_flag=0;
		
		u8_chg_start_cnt=0;
		u8_chg_end_cnt=0;
		u8_chg_V_low_cnt=0;
	
	
		//KEY&LED
		b_key1=0;
		b_key_trg1=0;
		b_key_date1=0;
		b_key_cont1=0;
		b_key1_flag=0;
		
		u8_tm_key_check_1ms1=0;
		u8_tm_key_25ms=0;
		u8_led_step=0;
		u8_led_mode=0;
		
		//ld_out
		b_ld_flag=0;	
		b_ld_display_flag=0;
		b_ld_flash_flag=0;
		b_tm_ld_500ms_flag=0;
		
		u8_ld_level=0;
		u8_tm_ld_close_25ms=0;
		
		//LEVLE
		b_level_change_flag=0;
		u8_bat_level=0;
		u8_chg_level=0;
		u8_level_add_cnt=0;
		u8_level_cut_cnt=0;
		u8_bat_level_csh_cnt=0;
		u8_tm_level_uc_500ms=0;
		u16_level_refer_value=0;
		
		
		//SP
		b_sp_flag=0;
		u8_tm_sp_25ms=0;
		u8_sp_cnt=0;
		u16_sp_buf_value=0;
	}
	void bat_level_csh()
	{
		while(b_ADC_finish_flag==0)
		{
			_clrwdt();
			ADC_Handle();
			if(b_ADC_finish_flag==1)
			{
				u8_bat_level_csh_cnt++;
				if(u8_bat_level_csh_cnt<30)
					b_ADC_finish_flag=0;	
			}
			if(b_ADC_finish_flag==1)
			{
				if(u16_adc_bat_V_final>BAT_V_LEVEL_5)
					u8_bat_level=5;
				else if(u16_adc_bat_V_final>BAT_V_LEVEL_4)
					u8_bat_level=4;
				else if(u16_adc_bat_V_final>BAT_V_LEVEL_3)
					u8_bat_level=3;
				else if(u16_adc_bat_V_final>BAT_V_LEVEL_2)
					u8_bat_level=2;
				else
					u8_bat_level=1;
			}
			
		}	
	}
	void main()
	{
		systemset();
		ioset();
		value_csh();
		bat_level_csh();
		b_ADC_finish_flag=0;
		while(1)
		{
			_clrwdt();
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////	
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////			
			if(b_tm_1ms_flag==1)
			{
				u8_tm_1ms++;
				if(u8_tm_1ms>=25)
				{
					u8_tm_1ms=0;
					u8_tm_25ms++;
					b_tm_25ms_flag=1;
					if(u8_tm_25ms>=20)
					{
						u8_tm_25ms=0;
						b_tm_500ms_flag=1;
					}
				}
			}
			if(b_tm_1ms_flag==1)
			{
				b_tm_1ms_flag=0;
				if(KEY1!=b_key1)
					u8_tm_key_check_1ms1++;
				if(b_chg_A_low_flag==1)
					CHG_SN=~CHG_SN;
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				if(b_key_cont1==1&&u8_tm_key_25ms<250)
					u8_tm_key_25ms++;
				if(b_key_cont1==0&&b_ld_display_flag==1&&b_chg_flag==0)
					u8_tm_ld_close_25ms++;
				u8_tm_sp_25ms++;
				
			}
			if(b_tm_500ms_flag==1)
			{
				b_tm_500ms_flag=0;
				b_tm_ld_500ms_flag=1;
				if(b_chg_start_flag==1||b_chg_end_flag==1)
					u8_tm_level_uc_500ms++;
			}
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////	
////////////////////////////tb0//////////////////////////////////////////////////////////////////////////

////////////////////////////_status//////////////////////////////////////////////////////////////////////////	
////////////////////////////_status//////////////////////////////////////////////////////////////////////////
			ADC_Handle();
			status_judge();
			V_level();
////////////////////////////_status//////////////////////////////////////////////////////////////////////////	
////////////////////////////_status//////////////////////////////////////////////////////////////////////////
	
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////

////////////////////////////ADC//////////////////////////////////////////////////////////////////////////	
////////////////////////////ADC//////////////////////////////////////////////////////////////////////////				
				
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
////////////////////////////KEY//////////////////////////////////////////////////////////////////////////
			if(KEY1!=b_key1)
			{
				if(u8_tm_key_check_1ms1>25)
				{
					b_key1=KEY1;
					u8_tm_key_check_1ms1=0;
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
				b_key1_flag=1;
				b_ld_flag=0;
				
			}
			if(b_key_cont1==1&&u8_tm_key_25ms>=60)	
			{
				b_key1_flag=0;					// ��������COB��־��0 
				u8_tm_key_25ms=0;
				if(b_chg_flag==0&&b_ld_display_flag==0)	//���LD��־λ0��δ�ڳ�磬��ô׼����LD
				{
					b_ld_flag=1;				//��������LD��־��1
					if(u8_tm_ld_close_25ms>0)	//����LD֮ǰ���Ȱ�LD�رռ�ʱ����
						u8_tm_ld_close_25ms=0;
				}
			}
			if(b_key_cont1==0&&b_key1_flag==1)	//���LED��־λ1 ��ô��LED
			{
				u8_led_step=1;
				b_key1_flag=0;
				u8_tm_key_25ms=0;
			}
			if(b_ld_flag==1)	//���LD��־Ϊ1  ��ô��LD
			{
				b_ld_flag=0;
				b_ld_display_flag=1;
			}
			if(u8_tm_ld_close_25ms>=80)			//����ɿ����� 2S֮��Ϩ��LD
			{
				b_ld_display_flag=0;
				u8_tm_ld_close_25ms=0;
			}
	////////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
	////////////////////////////KEY//////////////////////////////////////////////////////////////////////////			
	
	////////////////////////////LED//////////////////////////////////////////////////////////////////////////	
	////////////////////////////LED//////////////////////////////////////////////////////////////////////////
			if(u8_led_step==1)
			{

					u8_led_mode++;
					if(u8_led_mode>2)
					{
						u8_led_mode=0;
					}
					stm_out();
					u8_led_step=0;
			}
	////////////////////////////LED//////////////////////////////////////////////////////////////////////////	
	////////////////////////////LED//////////////////////////////////////////////////////////////////////////
	
	////////////////////////////LD//////////////////////////////////////////////////////////////////////////	
	////////////////////////////LD//////////////////////////////////////////////////////////////////////////
			ld_display();
	////////////////////////////LD//////////////////////////////////////////////////////////////////////////	
	////////////////////////////LD//////////////////////////////////////////////////////////////////////////
							//��������API����
			if(u8_tm_sp_25ms>20)
			{

				u8_sp_value[0] = ((u16_adc_chg_V_final>>8)&0xFF);
				u8_sp_value[1] = (u16_adc_chg_V_final&0xFF);
				u8_sp_value[2] = 0xFF;

				u8_sp_value[3] = ((u16_adc_chg_A_final>>8)&0xFF);
				u8_sp_value[4] = (u16_adc_chg_A_final&0xFF);
				u8_sp_value[5] = 0xFF;

				u8_sp_value[6] = ((u16_adc_bat_V_final>>8)&0xFF);
				u8_sp_value[7] = (u16_adc_bat_V_final&0xFF);
				u8_sp_value[8] = 0xFF;
				
				u8_sp_value[9] = ((u16_level_refer_value>>8)&0xFF);
				u8_sp_value[10]= (u16_level_refer_value&0xFF);
				u8_sp_value[11]= 0xFF;
				
				u8_sp_value[12]= u8_bat_level;
				u8_sp_value[13]= u8_chg_level;
				u8_sp_value[14]= 0xFF;



		/*		u8_sp_value[0] = ((u16_adc_chg_V_final>>8)&0xFF);
				u8_sp_value[1] = (u16_adc_chg_V_final&0xFF);
				
				u8_sp_value[2] = ((u16_adc_chg_A_final>>8)&0xFF);
				u8_sp_value[3] = (u16_adc_chg_A_final&0xFF);
				
				u8_sp_value[4] = ((u16_adc_bat_V_final>>8)&0xFF);
				u8_sp_value[5] = (u16_adc_bat_V_final&0xFF);
				
				u8_sp_value[6] = u8_bat_level;
				u8_sp_value[7] = u8_chg_level;
				
				u8_sp_value[8] = b_chg_flag;
				u8_sp_value[9] = L_2;
				
				u8_sp_value[10] = ((u16_level_refer_value>>8)&0xFF);
				u8_sp_value[11] = (u16_level_refer_value&0xFF);
				
				u8_sp_value[12] = b_chg_start_flag;
				u8_sp_value[13] = b_chg_end_flag;
				
				u8_sp_value[14] = u8_level_add_cnt;
				u8_sp_value[15] = u8_level_cut_cnt;*/
				
				u8_tm_sp_25ms=0;
				u8_sp_cnt=0;
			}
			if(u8_sp_cnt<16&&b_sp_flag==0)
			{
				u16_sp_buf_value=(((uint16)u8_sp_value[u8_sp_cnt]<<1)|0x0200);
				u8_sp_cnt++;
				b_sp_flag=1;
			}		
		}
	}

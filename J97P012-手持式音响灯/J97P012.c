/*
项目名称：手持式音响灯
芯片型号: HT66F004 内部8M 指令周期4分频2M
功能：



脚位定义
             ________________________
            |VSS                  VDD|
    LED_BLE |PC0         PB0/INT0/AN0| 
       		|PC1         PB1/INT1/AN1| 
     		|PC2        PB2/PTCK0/AN2| 
    KEY_BLE	|PA0/PTP0   PA4/PTCK1/AN3| CE2
        	|PA1/PTP0I   PA5/AN4/VREF| CE1
    KEY_LED |PA2        PA6/AN5/VREFO| BLE_B
         	|PA3/PTP1I   PA7/PTP1/AN6| BLE_S
             ------------------------
#define LED_BLE		_pc0
#define KEY_BLE 	_pa0
#define KEY_COB		_pa2
#define BLE_S		_pa7
#define BLE_B		_pa6
#define COB_CE1		_pa5
#define COB_CE2		_pa4

*/	
	#include "HT66F004.h"
	#define bool	bit
	#define uint8 	unsigned char 
	#define uint16	unsigned int 
	
	#define SP_FLAG 0
	
	#define LED_BLE		_pc0
	#define KEY_BLE 	_pa0
	#define KEY_COB		_pa2
	#define COB_CE2		_pa4
	#define COB_CE1		_pa5
	#define BLE_B		_pa6
	#define BLE_S		_pa7

	#define HALT_START_TM			200			//休眠计时，25ms为单位			5S
	
	#pragma rambank0
	
	//HALT
	bool b_halt_flag;
	uint8 u8_tm_halt_25ms;
	
	//Clock
	bool b_tm_1ms_flag;
	bool b_tm_25ms_flag;
	bool b_tm_500ms_flag;

	uint8 u8_tm_1ms;	
	uint8 u8_tm_25ms;
	
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
	
	//BLE
	bool b_ble_flag;
	bool b_ble_status_flag;
	uint8 u8_tm_ble_status_1ms;
	uint8 u8_tm_ble_status_con_1ms;

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
		_smod	= 0b00000000; 	// 系统时钟选择  FSYS=FH
		_smod1	= 0b00000000;	
		_sadc0	= 0B00000000;
		_sadc1	= 0B00000000;
		_sadc2	= 0B00000000;
		_ptm0c0	= 0b00000000;	
		_ptm0c1 = 0b00000000;		
		_ptm1c0	= 0b00000000;	
		_ptm1c1 = 0b00000000;	
		_integ	= 0b00000000;	
		_intc0	= 0b00000000; 	//中断设置
		_intc1	= 0b00000000;	//中断设置
		_mfi0	= 0b00000000;	//多功能中断设置
		_tbc	= 0B00000000;  	//时基设置
		_pa		=0b00000101;	
		_pac	=0b00000101;	
		_papu	=0b00000101;	
		_pawu	=0b00000101;
		_pasr	=0b00000000;	
		_pa		=0b00000101;	

		_pb		=0b00000000;	
		_pbc	=0b00000000;			
		_pbpu	=0b00000000;	
		_pbsr	=0b00000000;	
		_pb		=0b00000000;	

		_pc		=0b00000000;
		_pcc	=0b00000000;
		_pcpu	=0b00000000;
		_pc		=0b00000000;
		
		_wdtc	= 0b10101111; 	
	}
/*	
	#define LED_BLE		_pc0
	#define KEY_BLE 	_pa0
	#define KEY_COB		_pa2
	#define COB_CE2		_pa4
	#define COB_CE1		_pa5
	#define BLE_B		_pa6
	#define BLE_S		_pa7
*/
	void ioset()
	{
				// 76543210
		_pa		=0b00000101;	
		_pac	=0b10000101;	
		_papu	=0b00000101;	
		_pawu	=0b00000101;
		_pasr	=0b00000000;	
		_pa		=0b00000101;	

		_pb		=0b00000000;	
		_pbc	=0b00000000;			
		_pbpu	=0b00000000;	
		_pbsr	=0b00000000;	
		_pb		=0b00000000;	

		_pc		=0b00000000;
		_pcc	=0b00000000;
		_pcpu	=0b00000000;
		_pc		=0b00000000;
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

	
	void value_csh()
	{
		//HALT
		b_halt_flag=0;
		u8_tm_halt_25ms=0;
		
		//软件时钟
		b_tm_1ms_flag=0;
		b_tm_25ms_flag=0;
		b_tm_500ms_flag=0;

		u8_tm_1ms=0;	
		u8_tm_25ms=0;
	
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
		b_ble_flag=0;
		b_ble_status_flag=0;
		u8_tm_ble_status_1ms=0;
		u8_tm_ble_status_con_1ms=0;
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
					if(u8_tm_25ms>19)
					{
						u8_tm_25ms=0;
						b_tm_500ms_flag=1;
					}
				}
			}
			if(b_tm_1ms_flag==1)
			{
				b_tm_1ms_flag=0;
				u8_tm_key1_1ms++;	
				u8_tm_key2_1ms++;	
				if(b_ble_flag==1)
				{
					if(BLE_S==0)
					{
						if(b_ble_status_flag==0)
						{
							u8_tm_ble_status_1ms++;
							u8_tm_ble_status_con_1ms=0;
						}
					}
					else
					{
						u8_tm_ble_status_1ms=0;
						u8_tm_ble_status_con_1ms++;
					}

					
				}
				else
				{
					u8_tm_ble_status_1ms=0;
					u8_tm_ble_status_con_1ms=0;
				}
			}
			if(b_tm_25ms_flag==1)
			{
				b_tm_25ms_flag=0;
				u8_tm_halt_25ms++;		//休眠倒计时
#if (SP_FLAG==1)
				u8_tm_sp_25ms++;
#endif
			}
			if(b_tm_500ms_flag==1)
			{
				b_tm_500ms_flag=0;
				if(b_ble_flag==1&&b_ble_status_flag==0)
				{
					LED_BLE=~LED_BLE;
				}
			}
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////
////////////////////////////TIME//////////////////////////////////////////////////////////////////////////

///////////////////////////KEY//////////////////////////////////////////////////////////////////////////	
///////////////////////////KEY//////////////////////////////////////////////////////////////////////////		
			if(KEY_COB!=b_key1)
			{
				if(u8_tm_key1_1ms>35)
				{
					b_key1=KEY_COB;
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
			if(KEY_BLE!=b_key2)
			{
				if(u8_tm_key2_1ms>35)
				{
					b_key2=KEY_BLE;
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
			if(b_key_cont1==1&&b_key_trg1==1)
			{
				u8_led_mode++;
				if(u8_led_mode>2)
					u8_led_mode=0;
			}
			if(b_key_cont2==1&&b_key_trg2==1)
			{
				b_ble_flag=~b_ble_flag;
			}
			if(u8_led_mode==1)
			{
				COB_CE1=1;
				COB_CE2=0;
			}
			else if(u8_led_mode==2)
			{
				COB_CE1=1;
				COB_CE2=1;
			}
			else{COB_CE1=0;COB_CE2=0;}

			if(b_ble_flag==1)
			{
				BLE_B=1;
				if(b_ble_status_flag==1)
					LED_BLE=1;
			}
			else
			{
				BLE_B=0;
				LED_BLE=0;
				b_ble_status_flag=0;
			}
			if(u8_tm_ble_status_1ms>150)
			{
				b_ble_status_flag=1;
				u8_tm_ble_status_1ms=0;
			}
			if(u8_tm_ble_status_con_1ms>250)
			{
				b_ble_status_flag=0;
				u8_tm_ble_status_con_1ms=0;
			}




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
			if(u8_led_mode!=0||b_ble_flag==1)
			{
				u8_tm_halt_25ms=0;
			}
			if(u8_tm_halt_25ms>=HALT_START_TM)
			{
				b_halt_flag=1;
				systemhalt();
				
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


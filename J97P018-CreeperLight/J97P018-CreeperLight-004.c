/*
项目名称: CreeperLight
芯片型号: HT66F004 	内部8M 指令周期4分频
待机休眠功耗 ？uA
功能说明:


脚位定义:			HT66F002
					 -------
					|VDD VSS|
		       		|PA6 PA0| 
		     		|PA5 PA1| 
			        |PA7 PA2| 
					 -------
					HT66F004
			 ------------------------
    		|VSS                  VDD|	
   	IFR_LED |PC0         PB0/INT0/AN0| CE_3535_3	 
       		|PC1         PB1/INT1/AN1| CE_2835_1
IFR_Receive |PC2        PB2/PTCK0/AN2| 
   IFR_Send |PA0/PTP0   PA4/PTCK1/AN3| 
        	|PA1/PTP0I   PA5/AN4/VREF| KEY1
    		|PA2        PA6/AN5/VREFO| 
         	|PA3/PTP1I   PA7/PTP1/AN6| CE_1616_2 
             ------------------------


*/

#include "HT66F004.h"
#define bool	bit
#define uint8 	unsigned char 
#define uint16	unsigned int 

#define HT66F_002	2					//选择MCU,2为002
#define HT66F_004	4					//选择MCU,4为004						
#define MCU_S		4					//MCU选择位
#define HALT_F		1					//休眠选择位
#define SHUT_F		1					//5S关灯及记忆功能选择位
#define FLASH_F		1					//低电压闪烁选择位
#define ADC_F		1					//AD转换选择位	
#define IFR_F		1					//红外感应选择位
#define SP_F		1					//串口通讯选择位	
#define CLOSE_F		0					//强制关断选择位
#define OPEN		1	
#define CLOSE 		0	

#define BAT_LOW		1427				//2.9V		
#define VIN_REC		1673				//3.4V


#define	IFR_LED			_pc0
#define	IFR_Receive		_pc2 
#define IFR_Send		_pa0
#define CE_2835_1		_pb1
#define CE_1616_2		_pa7
#define CE_3535_3		_pb0
#define KEY1			_pa5

#if (MCU_S==HT66F_002)
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	#pragma		vector 			int0_int 		@ 0x04
	#pragma 	vector 			tb0_int			@ 0x08 		
	#pragma 	vector 			tb1_int			@ 0x0C
	#pragma 	vector 			mf0_int			@ 0x10
	#pragma 	vector 			eep_int			@ 0x14
	#pragma 	vector 			adc_int			@ 0x18
	void 	int0_int(void)	{	_inte	= 0;	_intf	=0;		}
	void 	tb1_int (void)	{	_tb1e	= 0;	_tb1f	=0;		}
	void 	mf0_int (void)	{	_mf0e	= 0;	_mf0f	=0;		}
	void 	eep_int (void)	{	_dee	= 0;	_def	=0;		}
	void 	adc_int (void)	{	_ade	= 0;	_adf	=0;		}
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
#elif (MCU_S==HT66F_004)
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
	#pragma 	vector			int0_int		@ 0x04		
	#pragma 	vector			tb0_int			@ 0x08		
	#pragma 	vector			tb1_int			@ 0x0C		
	#pragma 	vector			mf0_int			@ 0x10 		
	#pragma		vector			eep_int			@ 0x14		
	#pragma 	vector			adc_int			@ 0x18		
	#pragma 	vector			int1_int		@ 0x1C		
	void	int0_int(void)	{	_int0e	= 0;	_int0f	= 0;	}
	void	tb1_int	(void)	{	_tb1e	= 0; 	_tb1f	= 0; 	}
	void	mf0_int	(void)	{	_mf0e	= 0; 	_mf0f 	= 0;	}
	void	eep_int	(void)	{	_dee	= 0;	_def	= 0;	}
	void    adc_int	(void)	{	_ade 	= 0;	_adf	= 0;	}
	void 	int1_int(void)	{	_int1e	= 0;	_int1f 	= 0;	}
	/////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////
#endif

#pragma rambank0
bool	b_halt_flag;
uint8	u8_tm_halt_100ms;
//TM
bool	b_tm_1ms_flag;
bool	b_tm_10ms_flag;
bool	b_tm_100ms_flag;
bool	b_tm_1s_flag;

uint8	u8_tm_1ms_cnt;
uint8	u8_tm_10ms_cnt;
uint8	u8_tm_100ms_cnt;

//KEY
bool 	b_key1;		
bool 	b_key_trg1;
bool 	b_key_date1;
bool 	b_key_cont1;	
uint8 	u8_tm_key1_1ms;

bool	b_key_flag1;
uint8	u8_tm_key1_10ms;
uint8	u8_tm_key1_100ms;

//LED
bool	b_led_shutdown_flag;

uint8	u8_led_mode;
uint8	u8_led_mode1;
uint8	u8_led_step;


//IFR	
bool	b_ifr_sw_flag;
bool	b_tm_ifr_1ms_flag;
bool	b_tm_ifr_10ms_flag;

uint8	u8_ifr_step;
uint8	u8_ifr_cnt;
uint8	u8_tm_ifr_1ms;
uint8	u8_tm_ifr_10ms;



//ADC
bool	b_adc_bat_final_flag;
uint8	u8_adc_bat_cnt1;
uint16	u16_adc_bat_vdd;
uint16	u16_adc_bat_sum1;
uint16	u16_adc_bat_max;
uint16	u16_adc_bat_min;
uint16	u16_adc_bat_final;

//FLASH
bool 	b_led_flash_flag;
bool 	b_bat_low_flag;
bool 	b_tm_bat_low_1s_flag;
bool	 b_flash_once_flag;
uint8	u8_flash_cnt;
uint8	u8_flash_step;
uint8	u8_tm_flash_10ms;
uint8	u8_tm_flash_1s;
uint16	u16_bat_low_cnt;

//SP
bool 	b_sp_flag;
uint8	u8_sp_value[11];
uint8 	u8_tm_sp_10ms;
uint8 	u8_sp_cnt;
uint16	u16_sp_buf_value;



#pragma norambank

void Var_Init()
{
	b_halt_flag=0;
	u8_tm_halt_100ms=0;
//TM
	b_tm_1ms_flag=0;
	b_tm_10ms_flag=0;
	b_tm_100ms_flag=0;
	b_tm_1s_flag=0;

	u8_tm_1ms_cnt=0;
	u8_tm_10ms_cnt=0;
	u8_tm_100ms_cnt=0;

//KEY
 	b_key1=0;		
 	b_key_trg1=0;
 	b_key_date1=0;
 	b_key_cont1=0;	
 	u8_tm_key1_1ms=0;

	b_key_flag1=0;
	u8_tm_key1_10ms=0;
	u8_tm_key1_100ms=0;

//LED
	b_led_shutdown_flag=0;

	u8_led_mode=0;
	u8_led_mode1=0;
	u8_led_step=0;


//IFR	
	b_ifr_sw_flag=0;
	b_tm_ifr_1ms_flag=0;
	b_tm_ifr_10ms_flag=0;

	u8_ifr_step=0;
	u8_ifr_cnt=0;
	u8_tm_ifr_1ms=0;
	u8_tm_ifr_10ms=0;



//ADC
	b_adc_bat_final_flag=0;
	u8_adc_bat_cnt1=0;
	u16_adc_bat_vdd=0;
	u16_adc_bat_sum1=0;
	u16_adc_bat_max=0;
	u16_adc_bat_min=0xFFFF;
	u16_adc_bat_final==0;
	
//FLASH
 	b_led_flash_flag=0;
 	b_bat_low_flag=0;
 	b_tm_bat_low_1s_flag=0;
	b_flash_once_flag=0;
	u16_bat_low_cnt=0;
	u8_flash_cnt=0;
	u8_flash_step=0;
	u8_tm_flash_10ms=0;
	u8_tm_flash_1s=0;

//SP
 	b_sp_flag=0;
 	u8_tm_sp_10ms=0;
 	u8_sp_cnt=0;
	u16_sp_buf_value=0;
}
void Seg_Init()
{
	_smod		=	0b00000001;
	_smod1		=	0b00000000;
	_rstc		=	0b01010101;
	_sadc0		=	0b00000000;
	_sadc1		=	0b00000000;
	_sadc2		=	0b00000000;
	_integ		=	0b00000000;
	if(b_halt_flag==0)
	{
		_intc0	=	0b00000101;
		_tbc	=	0b11000011;
		_wdtc	=	0b01010111;	
		
	}
	else
	{
		_intc0	=	0b00000000;
		_tbc	=	0b00000000;
		_wdtc	=	0b10101111;	
	}
	_intc1		=	0b00000000;
	_mfi0		=	0b00000000;	
	_ptm0c0		=	0b00000000;
	_ptm0c1		=	0b00000000;
	_ptm1c0		=	0b00000000;
	_ptm1c1		=	0b00000000;
}
/*
			 ------------------------
    		|VSS                  VDD|	
   	IFR_LED |PC0         PB0/INT0/AN0| CE_3535_3	 
       		|PC1         PB1/INT1/AN1| CE_2835_1
IFR_Receive |PC2        PB2/PTCK0/AN2| 
   IFR_Send |PA0/PTP0   PA4/PTCK1/AN3| 
        	|PA1/PTP0I   PA5/AN4/VREF| KEY1
    		|PA2        PA6/AN5/VREFO| 
         	|PA3/PTP1I   PA7/PTP1/AN6| CE_1616_2 
             ------------------------
             #define	IFR_LED			_pc0
			#define	IFR_Receive		_pc2 
			#define CE_3535_3		_pb0			
			#define CE_2835_1		_pb1
			#define IFR_Send		_pa0
			#define KEY1			_pa5
			#define CE_1616_2		_pa7



*/
void IO_Init()
{
	_pa			=	0b00100000;
	_pac		=	0b00100000;
	_papu		=	0b00100000;
	_pawu		=	0b00100000;
	_pasr		=	0b00000001;
	_pa			=	0b00100000;
	
	_pb			=	0b00000000;
	_pbc		=	0b00000000;
	_pbpu		=	0b00000000;
	_pbsr		=	0b00000000;
	
	_pc			=	0b00000000;
	_pcc		=	0b00000100;
	_pcpu		=	0b00000000;
	_pc			=	0b00000000;	
}
void Halt_Con()
{
#if (HALT_F==1)
	if(u8_led_mode>0)
		u8_tm_halt_100ms=0;
	if(u8_tm_halt_100ms>49)
	{
		u8_tm_halt_100ms=0;
		b_halt_flag=1;
	}
	if(b_halt_flag==1)
	{
		Seg_Init();
		IO_Init();
		_nop();
		_nop();
		_nop();
		_nop();
		_halt();
		_nop();
		_nop();
		b_halt_flag=0;
		Seg_Init();
		IO_Init();
	}
#endif
}
void ADC_San()
{	
	_sadc1	=	0b01100100;		//[7:5]ADin=VDD/4	[2:0]Fad=Fsys/16
	_sadc0	=	0b00110000;		//[5]Open ENADC		[4]SADOL/H
	_sadc2	=	0b11001010;		//[7]ENOPA			[6]VBGEN			[3:0]Vf=vbg*2=2.08	
	_start	=0;
	_start	=1;
	_start	=0;
	while(_adbz)
		_clrwdt();
	u16_adc_bat_vdd=((uint16)_sadoh<<8)+(_sadol);
}
void ADC_Cal()
{
	u16_adc_bat_sum1+=u16_adc_bat_vdd;
	if(u16_adc_bat_vdd>u16_adc_bat_max)
		u16_adc_bat_max=u16_adc_bat_vdd;
	if(u16_adc_bat_vdd<u16_adc_bat_min)
		u16_adc_bat_min=u16_adc_bat_vdd;
	u8_adc_bat_cnt1++;
	if(u8_adc_bat_cnt1>=10)			
	{
		u16_adc_bat_sum1	-=	u16_adc_bat_max;
		u16_adc_bat_sum1	-=	u16_adc_bat_min;
		u16_adc_bat_sum1	>>=	3;
		u16_adc_bat_final	=	u16_adc_bat_sum1;
		b_adc_bat_final_flag=	1;
		u8_adc_bat_cnt1		=	0;
		u16_adc_bat_sum1	=	0;
		u16_adc_bat_max		=	0;
		u16_adc_bat_min		=	0xFFFF;
		u16_adc_bat_vdd		=	0;
	}	
}
void ADC_Con()
{
	ADC_San();
	ADC_Cal();		
}

void KEY_San()
{
	if(KEY1!=b_key1)
	{
		if(u8_tm_key1_1ms>20)
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
}
void KEY_Con()
{
	KEY_San();
	if(b_ifr_sw_flag==1)										
	{
		if(b_key_cont1==1&&b_key_trg1==1)
			b_key_flag1=1;										//Set Flag
		if(b_key_cont1==1&&b_key_flag1==1)
		{
			if(u8_tm_key1_10ms>200)
			{
				b_ifr_sw_flag=0;								//Close IFR								
				b_key_flag1=0;									//Clr Flag
				u8_led_mode=0;									//Close LED
				b_led_shutdown_flag=0;
				u8_led_mode1=0;
				u8_led_step=1;										
				u8_ifr_step=0;
				u8_tm_key1_10ms=0;
			}
		}
		else
			u8_tm_key1_10ms=0;		
	}	
	else
	{
		if(b_key_cont1==1&&b_key_trg1==1)						
		{
			b_key_flag1=1;		
		}
		if(b_key_cont1==1&&b_key_flag1==1)			
		{
			if(u8_tm_key1_10ms>200)
			{
				b_ifr_sw_flag=1;
				b_key_flag1=0;						
				u8_led_mode=1;
				u8_led_step=1;
				u8_ifr_step=1;
			}
		}
		else 
			u8_tm_key1_10ms=0;
		if(b_key_cont1==0)
		{
			if(b_key_flag1==1)
			{
				b_key_flag1=0;
#if (SHUT_F==1)
				if(b_led_shutdown_flag==1)
				{
					b_led_shutdown_flag=0;
					u8_led_mode1=u8_led_mode;
					u8_led_mode=0;
					u8_led_step=2;	
				}
				else
				{
					if(u8_led_mode1>0)
					{
						u8_led_mode=u8_led_mode1;
						u8_led_mode1=0;	
					}
					else
					{
						u8_led_mode++;
						if(u8_led_mode>3)
							u8_led_mode=0;	
					}
					u8_led_step=2;	
					b_bat_low_flag=0;	
				}
#else
				u8_led_mode++;
				if(u8_led_mode>3)
					u8_led_mode=0;
				u8_led_step=2;	
#endif

			}
		}
#if (SHUT_F==1)
		if(b_key_cont1==0&&u8_led_mode>0&&b_led_shutdown_flag==0)
		{
			if(u8_tm_key1_100ms>49)
			{
				b_led_shutdown_flag=1;		
				u8_tm_key1_100ms=0;	
			}
		}
		else
			u8_tm_key1_100ms=0;
#endif
	}
}
void IFR_Sw()				//生成38K方波（会略低于38K）
{
#if (IFR_F==1)
	_ptm0rpl	=	211;	//周期				
	_ptm0rph	=	0x00;
	_ptm0al		=	105;	//频率
	_ptm0ah		=	0x00;
	if(b_ifr_sw_flag==1)
	{
		_ptm0c0	=	0b00011000;
		IFR_LED	=	1;
	}
	else
	{
		_ptm0c0	=	0b00010000;
		IFR_LED	=	0;
	}
	_ptm0c1		=	0b10101000;
#endif
}
void IFR_Con()
{
#if (IFR_F==1)
	if(b_ifr_sw_flag==1)
	{
		if(u8_ifr_step==1)
		{
			if(IFR_Receive==1)
			{
				if(b_tm_ifr_1ms_flag==1&&u8_tm_ifr_1ms<255)
				{
					u8_tm_ifr_1ms++;
					b_tm_ifr_1ms_flag=0;	
				}
				if(u8_tm_ifr_1ms>20)
				{
					u8_ifr_step=2;
					u8_ifr_cnt=0;
					u8_tm_ifr_1ms=0;
					u8_tm_ifr_10ms=0;
				}
			}
			else
			{
				u8_tm_ifr_1ms=0;	
			}
		}
		else if(u8_ifr_step==2)
		{
			if(IFR_Receive==0)						
			{
				if(b_tm_ifr_1ms_flag==1&&u8_tm_ifr_1ms<255)
				{
					u8_tm_ifr_1ms++;
					b_tm_ifr_1ms_flag=0;
				}
				if(u8_tm_ifr_1ms>6)
				{
					u8_tm_ifr_1ms=0;
					u8_tm_ifr_10ms=0;
					u8_ifr_step=3;	
				}
			}	
			else
			{	
				u8_tm_ifr_1ms=0;
				if(u8_ifr_cnt==1)
				{
					if(b_tm_ifr_10ms_flag==1&&u8_tm_ifr_10ms<255)
					{
						u8_tm_ifr_10ms++;
						b_tm_ifr_10ms_flag=0;
					}
					if(u8_tm_ifr_10ms>50)
					{
						u8_ifr_cnt=0;
						u8_tm_ifr_10ms=0;
					}
				}
			}
		}
		else if(u8_ifr_step==3)
		{
			if(IFR_Receive==0)
			{
				if(b_tm_ifr_10ms_flag==1&&u8_tm_ifr_10ms<255)
				{
					u8_tm_ifr_10ms++;
					b_tm_ifr_10ms_flag=0;
				}
				if(u8_tm_ifr_10ms>49)
				{
					u8_ifr_cnt=0;
					u8_ifr_step=1;	
					u8_tm_ifr_10ms=0;
				}
				u8_tm_ifr_1ms=0;
			}	
			else
			{
				if(b_tm_ifr_1ms_flag==1&&u8_tm_ifr_1ms<255)
				{
					u8_tm_ifr_1ms++;
					b_tm_ifr_1ms_flag=0;
				}
				if(u8_tm_ifr_1ms>19)
				{
					if(u8_ifr_cnt==0)
					{
						u8_ifr_cnt=1;
						u8_ifr_step=2;	
					}
					else
					{
						u8_ifr_cnt=0;
						u8_ifr_step=4;	
					}
					u8_tm_ifr_1ms=0;
					u8_tm_ifr_10ms=0;
				}
			}	
		}
		else if(u8_ifr_step==4)
		{
			u8_led_mode++;
			if(u8_led_mode>3)
				u8_led_mode=1;
			u8_led_step=2;										
			u8_ifr_step=1;
			b_bat_low_flag=0;
		}
	}
#endif	
}
void LED_Con()
{
	if(u8_led_step==1)
	{
#if (IFR_F==1)
		IFR_Sw();
#endif
		u8_led_step=2;
	}
	else if(u8_led_step==2)
	{
		if(u8_led_mode==1)
		{
			CE_2835_1=1;	
			CE_1616_2=0;
			CE_3535_3=0;
		}	
		else if(u8_led_mode==2)
		{
			CE_2835_1=0;	
			CE_1616_2=1;
			CE_3535_3=0;
		}		
		else if(u8_led_mode==3)
		{
			CE_2835_1=0;	
			CE_1616_2=0;
			CE_3535_3=1;
		}	
		else 
		{
			CE_2835_1=0;
			CE_1616_2=0;
			CE_3535_3=0;
		}
		u8_led_step=3;
	}
}
void Flash_San()
{
#if	(FLASH_F==1)
	if(b_led_flash_flag==0)
	{
		if(u8_led_mode>0)
		{
			if(u16_adc_bat_final<BAT_LOW)
			{
				if(b_tm_bat_low_1s_flag==1)
				{
					u16_bat_low_cnt++;	
					b_tm_bat_low_1s_flag=0;
				}
			}
			else
			{
				if(u16_bat_low_cnt>0)
					u16_bat_low_cnt--;	
			}
			if(u16_bat_low_cnt>60000)
			{
				u16_bat_low_cnt=0;
				b_bat_low_flag=1;
				b_led_flash_flag=1;	
				u8_flash_step=1;
			}
		}
		else
			u16_bat_low_cnt=0;
	}
	else
	{
		if(u16_adc_bat_final>VIN_REC)
		{
			if(b_tm_bat_low_1s_flag==1)
			{
				u16_bat_low_cnt++;
				b_tm_bat_low_1s_flag=0;
			}
			if(u16_bat_low_cnt>60000)
			{
				b_led_flash_flag=0;
				b_bat_low_flag=0;
				u8_led_step=2;
				u16_bat_low_cnt=0;	
			}
		}
		else
		{
			if(u16_bat_low_cnt>0)
				u16_bat_low_cnt--;	
		}	
	}

#endif
}
void Flash_Con()
{
#if (FLASH_F==1)
	Flash_San();
	if(b_bat_low_flag==1)
	{
		if(u8_flash_step==1)
		{
			if(u8_tm_flash_10ms>49)
			{
				b_flash_once_flag=1;
				u8_tm_flash_10ms=0;
			}	
			if(u8_flash_cnt>59)
			{
				u8_flash_cnt=0;	
				u8_flash_step=2;
				u8_led_step=2;
			}
		}	
		else if(u8_flash_step==2)
		{
			if(u8_tm_flash_1s<57)
				u8_tm_flash_10ms=0;			
			if(u8_tm_flash_10ms>49)
			{
				b_flash_once_flag=1;
				u8_tm_flash_10ms=0;
			}
		 	if(u8_flash_cnt>5)
			{
				u8_flash_cnt=0;	
				u8_led_step=2;
				u8_tm_flash_1s=0;
			}
		}
		if(b_flash_once_flag==1)
		{
			if(u8_led_step<10)
			{
				CE_2835_1=0;
				CE_1616_2=0;
				CE_3535_3=0;
				u8_led_step=10;
			}
			else
			{
				u8_led_step=2;	
			}
			b_flash_once_flag=0;
			u8_flash_cnt++;
		}
	}
#endif
}
void tb0_int()
{
	b_tm_1ms_flag=1;
	b_tm_ifr_1ms_flag=1;
	b_tm_bat_low_1s_flag=1;
#if (SP_F==1)
	if(u16_sp_buf_value > 0)	
    {
        _pc1 = (u16_sp_buf_value & 0x01);
        u16_sp_buf_value >>= 1;
    }
    else
    {
        b_sp_flag = 0;
        _pc1=1;
    }	
#endif
}
void TM_Con()
{
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
	if(b_tm_1ms_flag==1)
	{
		b_tm_1ms_flag=0;
		if(u8_tm_key1_1ms<255)
			u8_tm_key1_1ms++;
	}
	if(b_tm_10ms_flag==1)
	{
		b_tm_10ms_flag=0;
		b_tm_ifr_10ms_flag=1;
		if(u8_tm_sp_10ms<255)
			u8_tm_sp_10ms++;
		if(u8_tm_key1_10ms<255)	
			u8_tm_key1_10ms++;
		if(b_bat_low_flag==1)
		{
			if(u8_tm_flash_10ms<255)
				u8_tm_flash_10ms++;
		}
		else
			u8_tm_flash_10ms=0;
	}
	if(b_tm_100ms_flag==1)
	{
		b_tm_100ms_flag=0;
		if(u8_tm_key1_100ms<255)
			u8_tm_key1_100ms++;
		if(u8_tm_halt_100ms<255)
		u8_tm_halt_100ms++;
	}
	if(b_tm_1s_flag==1)
	{
		b_tm_1s_flag=0;
		if(u8_flash_step==2)
		{
			if(u8_tm_flash_1s<61)
				u8_tm_flash_1s++;
		}
		else
			u8_tm_flash_1s=0;	
	}
}
void Sp_Con()
{
#if (SP_F==1)	
	if(u8_tm_sp_10ms>=50)
	{
		u8_sp_value[0]	= ((u16_adc_bat_final>>8)&0xFF);
		u8_sp_value[1]	= (u16_adc_bat_final&0xFF);
		u8_sp_value[2]	= ((u16_bat_low_cnt>>8)&0xFF);
		u8_sp_value[3]	= (u16_bat_low_cnt&0xFF);
		u8_sp_value[4]	= b_led_flash_flag;
		u8_sp_value[5]	= b_bat_low_flag;
		u8_sp_value[6]	= u8_flash_step;
		u8_sp_value[7]	= u8_flash_cnt;
		u8_sp_value[8]	= b_led_shutdown_flag;
		u8_sp_value[9]	= b_ifr_sw_flag;
		u8_sp_value[10]	= 0xFF;
		u8_tm_sp_10ms=0;
		u8_sp_cnt=0;
	}

	if(u8_sp_cnt<11&b_sp_flag==0)
	{
		u16_sp_buf_value=(((uint16)u8_sp_value[u8_sp_cnt]<<1)|0x0200);
		u8_sp_cnt++;
		b_sp_flag=1;
	}	
#endif
}

void main()
{
	Var_Init();
	Seg_Init();
	IO_Init();
	while(1)
	{
		_clrwdt();
		TM_Con();
		ADC_Con();
		KEY_Con();
		LED_Con();
		IFR_Con();
		Flash_Con();
		Sp_Con();	
		Halt_Con();
			
	}
	
}












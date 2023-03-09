/* ********************************************************************
FILE                   : main.c

PROGRAM DESCRIPTION    :  Implementation of function operation of scanf, sscanf, printf, sprintf, gets, puts, putchar and getchar, with more accurate floating point precision. 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 1: Output of conversion between String and Real Number is not exact as input of real num conversion.   

NOTE                  : FOR reusable firmware development and data can be inputed by multi input source and outputed to multi output source.                        
                       						
						
CAUTION               :  

               LCD    :  Only display least numeric digits and from left to right display is implemented.
			   UART   :  Only display least numeric digits and from left to right display is implemented.
			          :  Only 9600 baud rate is supported.
	     LED MATRIX   :  Does not support data type in str. 
                      :  Disp map ie led_dot_disp_map[] is coded for only seq write type = SEQ_WRITE_COL.
           KEYBOARD   :  If Keyboard is used, then u must hold CPU by using While(read_sw_pressed_state) by defining macro SW_PRESS_HOLD_CPU.
			          :  If ENTER_SW or BACKSPACE_SW is not a part of keyboard, then set IO channnel of that ENTER_SW or BACKSPACE_SW must set to IO_CH_INVALID.
					  :  Keypad's each row are activated in a sequential (serial) order and the columns of that row are sequentially checked for the pressed state. 
		        SW    :  If only independent SW's are used, then no need to hold CPU by using sw read level transition by not defining macro SW_PRESS_HOLD_CPU. 
				      :  NUM_INPUT_DEV_ID_SW_CHS must be appropiately set to accommodate independent SW's(if any) and (if any) Keyboard's required SW's. 
					     Keyboard's required SW's are KEYPAD_NUM_COLS, and ENTER_SW or/and BACKSPACE_SW(if ENTER_SW or/and BACKSPACE_SW are part of keyboard).
                      :  Long Key/SW press and no Key/SW press timeout is not yet implemented.    						 
  SW & KEYBOARD INIT  :  First initialise independent SW if any, then only initialize Keyboard, if any.
                TIMER :  TIMER 0 is used for system tick and timer 1 is used for application as well as system. 
	
DATA & DEV CTRL CONF  :  Configuration of data id and device control must be correct and appropriate. 
    	ERROR PROC    :  Process to do when a error or warning has occured is not implemented, but communicates with error status in LCD(ch: 0) or/and UART(ch: 0).
		  CTRL CHARS  :  Only Backspace ctrl char is supported and enter ctrl char can be used as terminator char. 
		  DATA STREAM :  Data Stream is not supported.
		STDOUT OPER   :  For Put_Char(), Put_Str() and Print() operations, macro UART_MOD_ENABLE must be defined 
		                 and TRACE_UART_CH_ID is used for STDOUT operations.
					  :  In real value, %g, %e or %E specifiers are not supported and only float data type is supported and IEEE 754 single precision is used for float data type. 						 
        STDIN OPER    :  For Get_Char(), Get_Str() and Scan() operations, macros UART_MOD_ENABLE and KEYBOARD_MOD_ENABLE must be defined. 		
		                 Keyboard dev must be one of the input data source in cur_data_id and IO Channel of ENTER_SW must not be IO_CH_INVALID(ie must be valid IO channel).
						 and TRACE_UART_CH_ID is used for STDOUT operation for display (for UART, to display read char(s)).	
					  :  In real value, %g, %e or %E specifiers are not supported and only float data type is supported and IEEE 754 single precision is used for float data type.  
		FLOW CONTROL  :  Only Stop and Wait ARQ and XON/XOFF are yet to be supported. 
	     	CHAR CODE :  Only ASCII 7 bit character code is supported. 
		    DEV INIT  :  All data ids dev source must be initialized and make sure that same dev source(dev id and dev ch id) must not initialized more than once.
          DEV DEINIT  :  All data ids dev source can be deinitialized and make sure that same dev source(dev id and dev ch id) must not deinitialized more than once.
   COMM INIT & DEINIT :	 First transmit and/or receive communication oper must be performed for init and deinit oper and then at last only init or deinit comm oper must be performed. 
  GENERAL INTERRUPT   :  FIQ interrupt type is not supported and interrupt handlers(ISRs) are non Reentrant interrupt handlers due to irq keyword.
                
				
CHANGE LOGS           : 

FILE ID               : 01

*****************************************************************************/
#include "main.h"
#include "conf_port.h"
#include "conf_dev_chs.h"
#include "conf_misc_ids.h"

/* ------------------------------ macro defination ------------------------------ */


/* ----------------------------- global variable defination --------------------- */
num_value_t disp_trace_num;
disp_num_fmt_t disp_trace_num_fmt;
uint32_t system_status_flag = NO_ERROR, uint32_temp_disp_data;
data_id_status_para_t data_id_status_para[NUM_DATA_IDS];
uint8_t cur_data_id = DATA_ID_INVALID, error_or_warning_proc_flag = 0;
const uint8_t internal_error_led_io_ch = INTERNAL_ERROR_LED_IO_CH;

#ifdef LCD_MOD_ENABLE
  char lcd_disp_error_code[MAX_AVAIL_NUM_COLS] = {'F','P', 'O', 'G','Q','V'};
  /* LCD_ERROR_CODE position from COL_NUM_01 in ERROR_LINE_NUM of TRACE_LCD_CH_ID are as below : 
    UART0_FRAMING, UART0_PARITY, UART0_OVERFLOW, UART1_FRAMING, UART1_PARITY, UART1_OVERFLOW */  
#endif

/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */
static uint16_t System_Init(void); 
static uint16_t HW_Init(const void *const get_init_ptr);
static uint16_t PLL_Init(void);
extern uint16_t Appl_Proc(void);
extern uint16_t Appl_Init(const void *const data_ptr);
extern uint16_t Appl_HW_Init(void);
extern uint16_t Appl_Reset(const uint8_t reset_type);

/*------------------------------------------------------------*
FUNCTION NAME  : main

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.01  

BUGS           :              
-*------------------------------------------------------------*/
int main(void)
{
	if((System_Init()) != SUCCESS)
	{
       return FAILURE;
	}	
    if((Appl_Init(NULL_DATA_PTR)) != SUCCESS)
	{
		return FAILURE;  
    }
	while(1)
	{
		if((Appl_Proc()) != SUCCESS)
	    {
	   	    return FAILURE;
	    }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_Process

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.02

BUGS           :    
-*------------------------------------------------------------*/
uint16_t Reset_Process(const uint8_t reset_type)
{
	switch(reset_type)
	{
		case RESET_WHOLE: 
          Reset_All_Interrupts(reset_type);
          #ifdef TIMER_MOD_ENABLE
		     Reset_All_Timer_Chs(reset_type);
          #endif		  
		  #ifdef KEYBOARD_MOD_ENABLE	
             Reset_All_Keyboard_Chs(reset_type);
	      #endif
		  #ifdef SW_MOD_ENABLE
	         Reset_All_SW_Chs(reset_type);
	      #endif          
	      #ifdef SEG7_MOD_ENABLE
	        Reset_All_Seg7_Chs(reset_type);
	      #endif
	      #ifdef LED_MATRIX_MOD_ENABLE
	         Reset_All_LED_Matrix_Chs(reset_type);
	      #endif
	      #ifdef LCD_MOD_ENABLE
	         Reset_All_LCD_Chs(reset_type);
	      #endif 
		  #ifdef UART_MOD_ENABLE
		    Reset_All_UART_Chs(reset_type);
		  #endif
		case RESET_DATA_IDS_AND_APPL: 
		    system_status_flag = NO_ERROR;
			if(internal_error_led_io_ch < IO_CH_INVALID)
			{
		       if((IO_Channel_Write(internal_error_led_io_ch, STATE_LOW))!= SUCCESS)
	           {
		          system_status_flag = ERR_IO_CH_WRITE;	
				  Error_or_Warning_Proc("01.02.01", ERROR_OCCURED, system_status_flag);
		          return system_status_flag;
	           }
			}			
		    Reset_Data_IDs_Status();			
		case RESET_APPL:		    
            if((Appl_Reset(RESET_APPL)) != SUCCESS)
		    {
			   system_status_flag = ERR_RESET_OPER;
			   Error_or_Warning_Proc("01.02.02", ERROR_OCCURED, system_status_flag);
		       return system_status_flag;
		    }
        break;
        default:
           system_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_Proc("01.02.03", ERROR_OCCURED, system_status_flag);
		   return system_status_flag;		
	}
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : System_Init

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.03  

BUGS           :              
-*------------------------------------------------------------*/
static uint16_t System_Init(void)
{
	 if((HW_Init(NULL_DATA_PTR)) != SUCCESS)
	 {
		 system_status_flag = ERR_HW_INIT;
		 Error_or_Warning_Proc("01.03.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }
	 if((Reset_Process(RESET_WHOLE)) != SUCCESS)
	 {
		 system_status_flag = ERR_RESET_OPER;
		 Error_or_Warning_Proc("01.03.02", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	 }	 
	 if((Data_IDs_Set_Para()) != SUCCESS)
	 {
         system_status_flag = ERR_DATA_ID_CONF;
		 Error_or_Warning_Proc("01.03.03", ERROR_OCCURED, system_status_flag);
		 return system_status_flag; 
	 }
     return SUCCESS;	 
}

/*------------------------------------------------------------*
FUNCTION NAME  : HW_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : First initialise independent SW if any, then initialize Keyboard, if any.

Func ID        : 01.04  

BUGS           :              
-*------------------------------------------------------------*/
static uint16_t HW_Init(const void *const get_init_ptr)
{
	PLL_Init();
	Reset_IO_Chs();	
    if((Appl_HW_Init()) != SUCCESS)
	{
		 system_status_flag = ERR_HW_INIT;
		 Error_or_Warning_Proc("01.04.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}    
    return SUCCESS;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : PLL_Init

DESCRIPTION    : PLL IS CONFIGURED. 
								
INPUT          : 

OUTPUT         : 

NOTE           : calculated for crystal oscillitor of 12MHz 
                 and PLL IS CONFIGURED for CCLK(processor clock) = 60MHz
				 and PCLK(APB clock) = 15MHz.

Func ID        : 01.05  

BUGS           :              
-*------------------------------------------------------------*/
static uint16_t PLL_Init(void)
{
	PLLCFG = 0x24; // SET (divider)PSEL = 2 AND (Multiplier) MSEL=5 
    PLLCON = 0x01; //Enable PLL, PLL IS ACTIVE BUT NOT YET CONNECT   
    PLLFEED = 0xAA; //Feed sequence
    PLLFEED = 0x55; //Feed sequence 
    while(!(PLLSTAT & 0x00000400)); //WAIT FOR FEED SEQUENCE TO BE INSERTED AND PLL to be locked 
    PLLCON = 0x03; //PLL HAS BEEN ACTIVE, LOCKED and Connect PLL
    PLLFEED = 0xAA; //Feed sequence
    PLLFEED = 0x55; //Feed sequence
  	VPBDIV=0X00;   // or APBDIV, SET APBDIV, PCLK(APB clock) = CCLK(processor clock) / 4
   	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : ISR_PLL

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.06  

BUGS           :              
-*------------------------------------------------------------*/
void ISR_PLL(void) __irq
{
	VICVectAddr = 0x0;
}

/*------------------------------------------------------------*
FUNCTION NAME  : ISR_Brown_Out_Detect

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 01.07  

BUGS           :              
-*------------------------------------------------------------*/
void ISR_Brown_Out_Detect(void) __irq
{
	VICVectAddr = 0x0;
}

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

/* ********************************************************************
FILE                  : uart.c

PURPOSE               :  
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : only display least numeric digits and from left to right display is implemented.
         
CHANGE LOGS           :  

FILE ID               : 10

*****************************************************************************/
#include "main.h"

#ifdef UART_MOD_ENABLE
#include <stdarg.h>	

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */
static uint8_t volatile *const uart_lcr_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0LCR, (uint8_t *) &U1LCR };
static uint8_t volatile *const uart_dll_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0DLL, (uint8_t *) &U1DLL };
static uint8_t volatile *const uart_dlm_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0DLM, (uint8_t *) &U1DLM };
static uint8_t volatile *const uart_fcr_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0FCR, (uint8_t *) &U1FCR };
static uint8_t volatile *const uart_lsr_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0LSR, (uint8_t *) &U1LSR };
static uint8_t volatile *const uart_rbr_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0RBR, (uint8_t *) &U1RBR };
static uint8_t volatile *const uart_thr_ptr[NUM_COMM_DEV_ID_UART_CHS] = {(uint8_t *) &U0THR, (uint8_t *) &U1THR };

uart_status_t uart_status[NUM_COMM_DEV_ID_UART_CHS];
file_t stdout_vt;

/* ----------------------------- global variable declaration -------------------- */
 

/* ----------------------------- global function declaration -------------------- */
int16_t VFile_Print(file_t *const fp, const char *const fmt, va_list *const print_va_list_ptr);
static uint16_t Error_or_Warning_UART_Proc(const uint8_t uart_ch_id, const char *const error_trace_str, const uint8_t warn_or_error_format);

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : only 9600 baud rate and no flow control are supported.

Func ID        : 10.01  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t UART_Init(const uint8_t uart_ch_id, const uint8_t init_mode)
{
  	uart_ctrl_t *cur_uart_ctrl_ptr;
	uart_status_t *cur_uart_status_ptr;
	uint8_t uart_io_ch, temp_data;
	
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.01.01", ERROR_OCCURED);
		return system_status_flag;
	}
	switch(uart_ch_id)
	{
		case CH_ID_00:
	       uart_io_ch = IO_CH_00;
		break;
        case CH_ID_01:
		   uart_io_ch = IO_CH_08;
        break;		
	}
	cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id;
	cur_uart_status_ptr = uart_status + uart_ch_id;
	switch(init_mode)
	{
      case COMM_TRANSMIT_FUNC:	
	    cur_uart_status_ptr->uart_transmit_io_ch = uart_io_ch; 
    	if((Non_GPIO_Func_Set(cur_uart_status_ptr->uart_transmit_io_ch, IO_FUNC_MODE_01 )) != SUCCESS)
	    {
	    	system_status_flag = ERR_UART_INIT;
	        Error_or_Warning_UART_Proc(uart_ch_id, "10.01.02", ERROR_OCCURED);
		    return system_status_flag;
	    }
      return SUCCESS;
	  //break;
	  case COMM_RECEIVE_FUNC:
         cur_uart_status_ptr->uart_receive_io_ch = uart_io_ch + 1;	  
	     if((Non_GPIO_Func_Set(cur_uart_status_ptr->uart_receive_io_ch, IO_FUNC_MODE_01 )) != SUCCESS)
	     {
	    	system_status_flag = ERR_UART_INIT;
	    	Error_or_Warning_UART_Proc(uart_ch_id, "10.01.03", ERROR_OCCURED);
	     	return system_status_flag;
	     }
       return SUCCESS;
	  //break;
	  case DEV_INIT_OPER:
	      if(cur_uart_ctrl_ptr->data_bit_len + 1 < 5)
	     {
	      	system_status_flag = ERR_UART_INIT;
	        Error_or_Warning_UART_Proc(uart_ch_id, "10.01.04", ERROR_OCCURED);
	        return system_status_flag;
	     }
		 temp_data = cur_uart_ctrl_ptr->data_bit_len + 1 - 5;	    
		 temp_data |= (cur_uart_ctrl_ptr->stop_select << 2);
		 switch(cur_uart_ctrl_ptr->error_check)
		 {
			 case NO_ERROR_CHECK:			   
       break;
			 case ODD_PARITY:
			   temp_data |= 0x08;
			 break;
       case EVEN_PARITY:
			   temp_data |= 0x18;
       break;	
			 default:
			    system_status_flag = ERR_UART_INIT;
	        Error_or_Warning_UART_Proc(uart_ch_id, "10.01.05", ERROR_OCCURED);
	        return system_status_flag; 
		 }
		 *uart_lcr_ptr[uart_ch_id] = 0x80 | temp_data;		 
     *uart_dll_ptr[uart_ch_id] = 98;   //Baud Rate 9600	
	   *uart_dlm_ptr[uart_ch_id] = 0;     //Baud Rate 9600   
     *uart_lcr_ptr[uart_ch_id] = 0x03;  //Set Length and clear DLE
     *uart_fcr_ptr[uart_ch_id] = 0x03;  //FIFO Enable 
     if((Init_File(&stdout_vt, STDOUT_FILE_DESP, CHAR_DEVICE, STREAM_TYPE_IO, STREAM_IO_OUTPUT )) != SUCCESS)
	  {
		  system_status_flag = ERR_INIT_IO_FILE_PROC;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.01.06", ERROR_OCCURED);
		  return system_status_flag;
	  }	
	  break;  
    default:
        system_status_flag = ERR_UART_INIT;
	   	  Error_or_Warning_UART_Proc(uart_ch_id, "10.01.07", ERROR_OCCURED);
	   	  return system_status_flag;	  
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.02  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t UART_Transmit_Char(const uint8_t uart_ch_id, const unsigned char uart_tx_char)
{
	uart_ctrl_t *cur_uart_ctrl_ptr;
	uart_status_t *cur_uart_status_ptr;
	
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.02.01", ERROR_OCCURED);		
		return system_status_flag;
	}
	cur_uart_status_ptr = uart_status + uart_ch_id;
	if(cur_uart_status_ptr->uart_transmit_till_end_point == STATE_YES)
	{
		if(cur_uart_status_ptr->uart_num_chars_transmitted >= cur_uart_status_ptr->uart_max_num_chars_transmitted)
		{
		    system_status_flag = WARN_TRANSMIT_END_POINT_REACHED;
			Error_or_Warning_UART_Proc(uart_ch_id, "10.02.02", WARNING_OCCURED);
		    return SUCCESS;
		}
	} 
	*uart_thr_ptr[uart_ch_id] = uart_tx_char;   //Transmitting Data
    while(!(*uart_lsr_ptr[uart_ch_id] & (1 << 6))); //Waiting for Transmission End
    if(cur_uart_ctrl_ptr->echo_receive_char == STATE_NO && cur_uart_status_ptr->uart_transmit_till_end_point == STATE_YES)
	{		
	    ++cur_uart_status_ptr->uart_num_chars_transmitted;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit_Str

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.03  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t UART_Transmit_Str(const uint8_t uart_ch_id, const char *const transmit_str)
{
	uint8_t tx_num_chars = 0;
	
	if(transmit_str == NULL_DATA_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.03.01", ERROR_OCCURED);
		return system_status_flag;
	}
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.03.02", ERROR_OCCURED);
		return system_status_flag;
	}	
	while(*(transmit_str + tx_num_chars))
	{
		 if((UART_Transmit_Char(uart_ch_id, *(transmit_str + tx_num_chars))) != SUCCESS)
		 {
			 system_status_flag = ERR_UART_TX_PROC;
			 Error_or_Warning_UART_Proc(uart_ch_id, "10.03.03", ERROR_OCCURED);
			 return system_status_flag; 
		 }			 
		 ++tx_num_chars;
	}
    return SUCCESS;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit_Num

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : only display least numeric digits and from left to right display is implemented

Func ID        : 10.04  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t UART_Transmit_Num(const uint8_t uart_ch_id, const uint8_t num_data_type, const disp_num_fmt_t disp_num_fmt, const int32_t uart_num)
{
    char hex_data[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; 
    uint32_t base_power_val, uart_disp_num;
    int16_t place_val;
    uint8_t base, place_digit[MAX_UART_DIGITS];    	
	 
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.04.01", ERROR_OCCURED);
		   return system_status_flag;
	}
	if(disp_num_fmt.num_digits <= 0 || disp_num_fmt.num_digits > MAX_UART_DIGITS )
	{
		system_status_flag = ERR_UART_DISP_FORMAT;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.04.02", ERROR_OCCURED);
		return system_status_flag;
	}
     switch(num_data_type)
	 {
		case DATA_TYPE_IN_BINARY:
           base = BASE_02;
	       if(uart_num < 0)
		   {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	if((UART_Transmit_Char(uart_ch_id, '-')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                Error_or_Warning_UART_Proc(uart_ch_id, "10.04.03", ERROR_OCCURED);
						return system_status_flag;
					}
				 }			    
			}
			else
			{
			   if(disp_num_fmt.sign_flag == STATE_YES)
		       {
			    	if((UART_Transmit_Char(uart_ch_id, '+')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                Error_or_Warning_UART_Proc(uart_ch_id, "10.04.04", ERROR_OCCURED);
						return system_status_flag;
					}
			   }
		    }		
           uart_disp_num = uart_num;   		   
		   if((UART_Transmit_Str(uart_ch_id, "0b")) != SUCCESS)
		   {
			   system_status_flag = ERR_UART_TX_PROC;
		        Error_or_Warning_UART_Proc(uart_ch_id, "10.04.05", ERROR_OCCURED);
			   return system_status_flag;
		   }
       break;			
	   case DATA_TYPE_IN_HEXA:
		   base = BASE_16;
	       if(uart_num < 0)
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	if((UART_Transmit_Char(uart_ch_id, '-')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                Error_or_Warning_UART_Proc(uart_ch_id, "10.04.06", ERROR_OCCURED);
						return system_status_flag;
					}			
				 }
            	uart_disp_num = -uart_num; 
			 }
			 else
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		        {
			    	if((UART_Transmit_Char(uart_ch_id, '+')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                Error_or_Warning_UART_Proc(uart_ch_id, "10.04.07", ERROR_OCCURED);
						return system_status_flag;
					}			
				 }
                 uart_disp_num = uart_num; 
			 }           			
			if((UART_Transmit_Str(uart_ch_id, "0x")) != SUCCESS)
			{
				system_status_flag = ERR_UART_TX_PROC;
		         Error_or_Warning_UART_Proc(uart_ch_id, "10.04.08", ERROR_OCCURED);
				return system_status_flag;
			}
        break;			
		case DATA_TYPE_IN_DECIMAL:
		    base = BASE_10;
		    if(uart_num < 0)
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	if((UART_Transmit_Char(uart_ch_id, '-')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                 Error_or_Warning_UART_Proc(uart_ch_id, "10.04.09", ERROR_OCCURED);
						return system_status_flag;
					}
				 }
			    uart_disp_num = -uart_num; 
			 }
			 else
			 {
				 if(disp_num_fmt.sign_flag == STATE_YES)
		         {
			    	if((UART_Transmit_Char(uart_ch_id, '+')) != SUCCESS)
					{
						system_status_flag = ERR_UART_TX_PROC;
		                 Error_or_Warning_UART_Proc(uart_ch_id, "10.04.10", ERROR_OCCURED);
						return system_status_flag;
					}
				 }
			  	 uart_disp_num = uart_num;  
			 }	
		break;
		default:
		  system_status_flag = ERR_UART_DISP_FORMAT;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.04.11", ERROR_OCCURED);
		  return system_status_flag;
	}
	if(disp_num_fmt.least_digits_flag == STATE_YES && disp_num_fmt.left_format_flag == STATE_YES)
	{			
	       base_power_val =  (uint32_t)Power_Of(base, disp_num_fmt.num_digits);
		   for(place_val = disp_num_fmt.num_digits - 1; place_val >= 0; --place_val)
		   {
		      uart_disp_num %= base_power_val;
		      base_power_val /= base; 
		      place_digit[place_val] = uart_disp_num /base_power_val;
		      if((UART_Transmit_Char(uart_ch_id, hex_data[place_digit[place_val]])) != SUCCESS)
			  {
				  system_status_flag = ERR_UART_TX_PROC;
		          Error_or_Warning_UART_Proc(uart_ch_id, "10.04.12", ERROR_OCCURED);
				  return system_status_flag;
			  }				  
		  } 
	}
    else
	{
		//only display least numeric digits and from left to right display is implemented
		system_status_flag = ERR_UART_DISP_FORMAT;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.04.13", ERROR_OCCURED);
		return system_status_flag;
	}		
    return SUCCESS;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Receive_Char

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.05  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t UART_Receive_Char(const uint8_t uart_ch_id, void *const dev_input_ctrl_para_ptr )
{
	uart_ctrl_t *cur_uart_ctrl_ptr;
	uart_status_t *cur_uart_status_ptr;
	dev_read_ctrl_para_t *rcvd_dev_input_ctrl_para_ptr = (dev_read_ctrl_para_t *)dev_input_ctrl_para_ptr;
	const char framing_error_msg_disp[] = "FERR", overrun_error_msg_disp[] = "OERR";
	uint8_t uart_rcvd_status;
	
	if(rcvd_dev_input_ctrl_para_ptr == NULL_DATA_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.05.01", ERROR_OCCURED);
		return system_status_flag;
	}
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.05.02", ERROR_OCCURED);
		return system_status_flag;
	}
	cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id;
	cur_uart_status_ptr = uart_status + uart_ch_id;
	if(cur_uart_ctrl_ptr->access_flag != STATE_YES) 
	{  
        system_status_flag = WARN_CUR_DATA_ID_DEV_NO_ACCESS;			 
        return  system_status_flag;
	}
	if( cur_uart_ctrl_ptr->enable_flag != STATE_YES) 
	{  
       system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;		 
       return system_status_flag;
	}
	if(cur_uart_ctrl_ptr->receive_enable_flag != STATE_YES)
	{
		system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;		 
        return system_status_flag;
	}
	if(cur_uart_status_ptr->uart_receive_till_end_point == STATE_YES)
	{
		if(cur_uart_status_ptr->uart_num_chars_received >= cur_uart_status_ptr->uart_max_num_chars_received)
		{
		    system_status_flag = WARN_RECEIVE_END_POINT_REACHED;
			Error_or_Warning_UART_Proc(uart_ch_id, "10.05.03", WARNING_OCCURED);
		    return SUCCESS;
		}
	}
	if(rcvd_dev_input_ctrl_para_ptr->cur_data_read_status.max_try_reached_flag == STATE_YES || rcvd_dev_input_ctrl_para_ptr->cur_data_read_status.read_and_ready_to_process_flag == STATE_YES)
	{
		  UART_Receive_Enable_Ctrl(uart_ch_id, STATE_NO); 
		  return WARN_CUR_DATA_ID_DEV_DISABLED;
	}
	uart_rcvd_status = *uart_lsr_ptr[uart_ch_id];
	if(!(uart_rcvd_status & (1 << 0)))
	{
		return COMM_NO_RCV_CHAR;
	}
	do
	{
		if(uart_rcvd_status & (1 << 7))
		{
			system_status_flag = ERR_COMM_RCV_PROC;
		   if(uart_rcvd_status & (1 << 1))
		   {
			   //overflow run error has occured
			  #ifdef LCD_MOD_ENABLE
		           LCD_Disp_Goto_XY(TRACE_LCD_CH_ID, ERROR_LINE_NUM, COL_NUM_01);
	               LCD_Disp_Str(TRACE_LCD_CH_ID, overrun_error_msg_disp);
			  #endif 
			   system_status_flag  = ERR_UART_OVERFLOW_RUN; 			  		              		
		   }
		   if(uart_rcvd_status & (1 << 2))
		   {
			 // parity error has occured
			   #ifdef LCD_MOD_ENABLE
	             LCD_Disp_Goto_XY(TRACE_LCD_CH_ID, ERROR_LINE_NUM, COL_NUM_01);
	             LCD_Disp_Str(TRACE_LCD_CH_ID, framing_error_msg_disp);
			   #endif 
			    system_status_flag = ERR_UART_PARITY; 
		   }
		   if(uart_rcvd_status & (1 << 3))
		   {
			 // framing error has occured
			   #ifdef LCD_MOD_ENABLE
	             LCD_Disp_Goto_XY(TRACE_LCD_CH_ID, ERROR_LINE_NUM, COL_NUM_01);
	             LCD_Disp_Str(TRACE_LCD_CH_ID, framing_error_msg_disp);
			   #endif 
			    system_status_flag = ERR_UART_FRAMING; 		   				
		   }
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.05.04", WARNING_OCCURED);
		   return system_status_flag;	
		}
		uart_rcvd_status = *uart_lsr_ptr[uart_ch_id];		
	} while(!(uart_rcvd_status & (1 << 0)));      //Waiting for Data Receiving
    rcvd_dev_input_ctrl_para_ptr->read_data_char = *uart_rbr_ptr[uart_ch_id];  //Receive the Data 	 
	if(cur_uart_status_ptr->uart_receive_till_end_point == STATE_YES && rcvd_dev_input_ctrl_para_ptr->read_data_char != BACKSPACE_CHAR && rcvd_dev_input_ctrl_para_ptr->read_data_char != ENTER_CHAR)
	{
		++cur_uart_status_ptr->uart_num_chars_received;
	}			   
	if(cur_uart_ctrl_ptr->echo_receive_char == STATE_YES)
	{
	   if((UART_Transmit_Char(uart_ch_id, rcvd_dev_input_ctrl_para_ptr->read_data_char)) != SUCCESS)
	   {
		   system_status_flag = ERR_UART_TX_PROC;
		    Error_or_Warning_UART_Proc(uart_ch_id, "10.05.05", ERROR_OCCURED);
		   return system_status_flag;
	   }
	}
  return COMM_RCVD_CHAR;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Disable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.06  

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Disable(const uint8_t uart_ch_id)
{
	 uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	
	 if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.06.01", ERROR_OCCURED);
		  return system_status_flag;
	 }
	 cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id ;  
     cur_uart_ctrl_ptr->enable_flag = STATE_NO;	 
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Enable

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.07  

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Enable(const uint8_t uart_ch_id)
{
      uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	
	 if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.07.01", ERROR_OCCURED);
		  return system_status_flag;
	 }
	 cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id ;  
     cur_uart_ctrl_ptr->enable_flag = STATE_YES;
   return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_All_UART_Chs

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.08 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t Reset_All_UART_Chs(const uint8_t uart_reset_type)
{
	uint8_t uart_ch_id;
	
	File_Flush(&stdout_vt);
	for(uart_ch_id = 0; uart_ch_id < NUM_COMM_DEV_ID_UART_CHS; ++uart_ch_id)
	{
		Reset_UART_Ch(uart_ch_id, uart_reset_type);
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Allow_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.09

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Allow_Access(const uint8_t uart_ch_id)
{
	 uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	
	 if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;	
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.09.01", ERROR_OCCURED);
		  return system_status_flag;
	 }
	 cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id ;   
     cur_uart_ctrl_ptr->access_flag = STATE_YES;
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_No_Access

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.10 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_No_Access(const uint8_t uart_ch_id)
{
	 uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	
	 if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	 {
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.10.01", ERROR_OCCURED);
		  return system_status_flag;
	 }
	 cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id ;   
     cur_uart_ctrl_ptr->access_flag = STATE_NO;
     UART_Disable(uart_ch_id);
	 return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_DeInit

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.11 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_DeInit(const uint8_t uart_ch_id, const uint8_t deinit_mode)
{
	uart_status_t *cur_uart_status_ptr;
 	io_config_t uart_unconfig;	
		
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.11.01", ERROR_OCCURED);
		return system_status_flag;
	}
	cur_uart_status_ptr = uart_status + uart_ch_id;
	switch(deinit_mode)
	{
      case COMM_TRANSMIT_FUNC:
        uart_unconfig.io_ch = cur_uart_status_ptr->uart_transmit_io_ch; 
		uart_unconfig.port_pin_len = 1;
	    uart_unconfig.func = IO_FUNC_UART ;
	    uart_unconfig.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
		if((IO_Ch_Func_Reset(&uart_unconfig)) != SUCCESS)
	    {
	    	system_status_flag = ERR_IO_CH_FUNC_RESET;
		    Error_or_Warning_UART_Proc(uart_ch_id, "10.11.02", ERROR_OCCURED);
            return system_status_flag;		
	    }
		cur_uart_status_ptr->uart_transmit_io_ch = IO_CH_INVALID;
	  break;
	  case COMM_RECEIVE_FUNC: 
	    uart_unconfig.io_ch = cur_uart_status_ptr->uart_receive_io_ch;		 
		uart_unconfig.port_pin_len = 1;
	    uart_unconfig.func = IO_FUNC_UART ;
	    uart_unconfig.func_type = IO_FUNC_TYPE_GPIO_NON_SW;
		if((IO_Ch_Func_Reset(&uart_unconfig)) != SUCCESS)
	    {
	    	system_status_flag = ERR_IO_CH_FUNC_RESET;
		    Error_or_Warning_UART_Proc(uart_ch_id, "10.11.03", ERROR_OCCURED);
            return system_status_flag;		
	    }
		cur_uart_status_ptr->uart_receive_io_ch = IO_CH_INVALID;
	  break;
      case DEV_DEINIT_OPER:        
      break;  
	  default:
	    system_status_flag = ERR_UART_DEINIT;
	    Error_or_Warning_UART_Proc(uart_ch_id, "10.11.04", ERROR_OCCURED);
		return system_status_flag;
	}
    return SUCCESS;	
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit_Enable_Ctrl

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.12 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Transmit_Enable_Ctrl(const uint8_t uart_ch_id, const uint8_t transmit_enable_flag)
{
	 uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	 
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.12.01", ERROR_OCCURED);
		return system_status_flag;
	}
	if(transmit_enable_flag > 1)
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.12.02", ERROR_OCCURED);
		return system_status_flag;
	}
	cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id;	
	cur_uart_ctrl_ptr->transmit_enable_flag = transmit_enable_flag;	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Receive_Enable_Ctrl

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.13 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Receive_Enable_Ctrl(const uint8_t uart_ch_id, const uint8_t receive_enable_flag)
{
	uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	 
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS)
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.13.01", ERROR_OCCURED);
		return system_status_flag;
	}
	if(receive_enable_flag > 1)
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.13.02", ERROR_OCCURED);
		return system_status_flag;
	}
	cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id;	
	cur_uart_ctrl_ptr->receive_enable_flag = receive_enable_flag;	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.14

BUGS           :   
-*------------------------------------------------------------*/
uint16_t UART_Transmit(const uint8_t uart_ch_id, const void *const uart_transmit_ctrl_para_ptr)
{
	data_write_ctrl_ptr_t *uart_data_transmit_ptr;	
    uart_ctrl_t *cur_uart_ctrl_ptr = NULL_DATA_PTR;
	uart_status_t *cur_uart_status_ptr = NULL_DATA_PTR;
	uart_transmit_ctrl_t *uart_transmit_ctrl_ptr; 
	char retrieved_uart_data_str[DATA_MAX_NUM_ALLOCATED_CHARS], *cur_uart_disp_str;
	int32_t uart_transmit_num_int32;
	uint8_t output_data_type ;    
	
    if(uart_transmit_ctrl_para_ptr == NULL_DATA_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.14.01", ERROR_OCCURED);
		return system_status_flag;
	}
    if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS )
	{
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.14.02", ERROR_OCCURED);
		   return system_status_flag;
	}		
	cur_uart_ctrl_ptr = uart_ctrl + uart_ch_id; 
	cur_uart_status_ptr = uart_status + uart_ch_id;
	if( cur_uart_ctrl_ptr->access_flag != STATE_YES) 
	{  
        system_status_flag = WARN_CUR_DATA_ID_DEV_NO_ACCESS;		
        return  system_status_flag;
	}		 
	   if(cur_uart_ctrl_ptr->enable_flag != STATE_YES) 
	   {  
          system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;
		  return  system_status_flag;
	   } 
	   if(cur_uart_ctrl_ptr->transmit_enable_flag != STATE_YES) 
	   {  
          system_status_flag = WARN_CUR_DATA_ID_DEV_DISABLED;
		  return  system_status_flag;
	   }
     uart_data_transmit_ptr  = (data_write_ctrl_ptr_t *)uart_transmit_ctrl_para_ptr;
	 uart_transmit_ctrl_ptr = &(((dev_write_ctrl_para_t *)uart_data_transmit_ptr->dev_transmit_ctrl_para_ptr)->uart_transmit_ctrl_para);
     if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_CTRL_OUTPUT_DATA_TYPE, &output_data_type)) != SUCCESS)
	 {
		  system_status_flag = ERR_APPL_DATA_RETRIEVE;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.14.03", ERROR_OCCURED);
		  return system_status_flag; 
	 }
     cur_uart_status_ptr->uart_transmit_till_end_point = uart_transmit_ctrl_ptr->uart_data_till_end_point_flag;
	 switch(output_data_type)
	 {
		case DATA_TYPE_IN_STR:
		   switch(uart_transmit_ctrl_ptr->uart_use_data_status_flag)
		   {
		      case STATE_YES:
		         if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_STR, retrieved_uart_data_str )) != SUCCESS)
		         {
			    	  system_status_flag = ERR_APPL_DATA_RETRIEVE;
				      Error_or_Warning_UART_Proc(uart_ch_id, "10.14.04", ERROR_OCCURED);
				      return system_status_flag; 
		         }
				 cur_uart_disp_str = retrieved_uart_data_str;
              break;
              case STATE_NO:
                 cur_uart_disp_str = uart_transmit_ctrl_ptr->uart_disp_str;
              break;
		   }			  
		   if((UART_Transmit_Str(uart_ch_id, cur_uart_disp_str )) != SUCCESS)
		   {
			   system_status_flag = ERR_UART_TX_PROC;
			   Error_or_Warning_UART_Proc(uart_ch_id, "10.14.05", ERROR_OCCURED);
			   return system_status_flag; 
		   }
		break;
		case DATA_TYPE_IN_CHAR:
		   switch(uart_transmit_ctrl_ptr->uart_use_data_status_flag)
		   {
		      case STATE_YES:
		         if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_CHAR, retrieved_uart_data_str )) != SUCCESS)
		         {
			    	 system_status_flag = ERR_APPL_DATA_RETRIEVE;
				     Error_or_Warning_UART_Proc(uart_ch_id, "10.14.06", ERROR_OCCURED);
				     return system_status_flag; 
		         }
				        cur_uart_disp_str = retrieved_uart_data_str;
              break;
              case STATE_NO:
                cur_uart_disp_str  = uart_transmit_ctrl_ptr->uart_disp_str;
              break;
		   }	
		   if((UART_Transmit_Char(uart_ch_id, *cur_uart_disp_str)) != SUCCESS)
		   {
			    system_status_flag = ERR_UART_TX_PROC;
				Error_or_Warning_UART_Proc(uart_ch_id, "10.14.07", ERROR_OCCURED);
				return system_status_flag;
		   }			   
		break;
		case DATA_TYPE_IN_BINARY:
		case DATA_TYPE_IN_HEXA:
		case DATA_TYPE_IN_DECIMAL:
		   if(uart_transmit_ctrl_ptr->disp_num_fmt.num_digits <= 0 || uart_transmit_ctrl_ptr->disp_num_fmt.num_digits > MAX_UART_DIGITS )
		   {
			  system_status_flag = ERR_UART_DISP_FORMAT;
			  Error_or_Warning_UART_Proc(uart_ch_id, "10.14.08", ERROR_OCCURED);
			  return system_status_flag;
		   }
		   switch(uart_transmit_ctrl_ptr->uart_use_data_status_flag)
		   {
		      case STATE_YES:
		         if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_INT32, &uart_transmit_num_int32 )) != SUCCESS)
		         {
			    	  system_status_flag = ERR_APPL_DATA_RETRIEVE;
				      Error_or_Warning_UART_Proc(uart_ch_id, "10.14.09", ERROR_OCCURED);
				      return system_status_flag; 
		         }				
              break;
              case STATE_NO:
                 uart_transmit_num_int32 = uart_transmit_ctrl_ptr->uart_disp_num;
              break;
		   }	
		   if((UART_Transmit_Num(uart_ch_id, output_data_type, uart_transmit_ctrl_ptr->disp_num_fmt, uart_transmit_num_int32)) != SUCCESS)
		   {
			  system_status_flag = ERR_UART_TX_PROC;
			  Error_or_Warning_UART_Proc(uart_ch_id, "10.14.10", ERROR_OCCURED);
			  return system_status_flag;
		   }        			
         break;
		 default:
		   system_status_flag = ERR_DATA_TYPE_INVALID;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.14.11", ERROR_OCCURED);
		   return system_status_flag;
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Transmit_Ctrl_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.15 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Transmit_Ctrl_Proc(const uint8_t uart_ch_id, const uint8_t uart_ctrl_format, const char uart_transmit_char )
{	
    if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS )
	{
		   system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.15.01", ERROR_OCCURED);
		   return system_status_flag;
	}
	switch(uart_transmit_char)
	{	
         case BACKSPACE_CHAR:
		 case ENTER_CHAR:		    		    
		 break;		 
		 default:
		    switch(uart_ctrl_format & 0x03)
            {
               case RCVD_CHAR_PLAIN_DISP_FORMAT:
                  if((UART_Transmit_Char(uart_ch_id, uart_transmit_char)) != SUCCESS)
				  {
					  system_status_flag = ERR_UART_TX_PROC;
			          Error_or_Warning_UART_Proc(uart_ch_id, "10.15.02", ERROR_OCCURED);
			          return system_status_flag;
				  }
               break;
               case RCVD_CHAR_HIDDEN_DISP_FORMAT:
                  if((UART_Transmit_Char(uart_ch_id, uart_transmit_char)) != SUCCESS)
				  {
					  system_status_flag = ERR_UART_TX_PROC;
			          Error_or_Warning_UART_Proc(uart_ch_id, "10.15.03", ERROR_OCCURED);
			          return system_status_flag;
				  }
                  Delay_MS(REQ_TIME_DELAY_UART_HIDDEN_DISP_IN_MS);
				  if((UART_Transmit_Char(uart_ch_id, BACKSPACE_CHAR)) != SUCCESS)
				  {
					  system_status_flag = ERR_UART_TX_PROC;
			          Error_or_Warning_UART_Proc(uart_ch_id, "10.15.04", ERROR_OCCURED);
			          return system_status_flag; 
				  }
                  if((UART_Transmit_Char(uart_ch_id, HIDDEN_KEY_DISP_CHAR)) != SUCCESS)
				  {
					  system_status_flag = ERR_UART_TX_PROC;
			          Error_or_Warning_UART_Proc(uart_ch_id, "10.14.10", ERROR_OCCURED);
			          return system_status_flag; 
				  }
               break;
	           case RCVD_CHAR_DONT_DISP_FORMAT:
	           break;
	           default:
                  /* error: invalid lcd enter char disp format */
                 system_status_flag = ERR_FORMAT_INVALID;
				Error_or_Warning_UART_Proc(uart_ch_id, "10.15.05", ERROR_OCCURED);
                return system_status_flag;		 
            }
	}
	return SUCCESS;
}	

/*------------------------------------------------------------*
FUNCTION NAME  : Reset_UART_Ch

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.16 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t Reset_UART_Ch(const uint8_t uart_ch_id, const uint8_t uart_reset_type)
{	
    uart_status_t *cur_uart_status_ptr;
		
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS )
	{
		  system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		  Error_or_Warning_UART_Proc(uart_ch_id, "10.16.01", ERROR_OCCURED);
		  return system_status_flag;
	}
    switch(uart_reset_type)
    {
        case RESET_WHOLE:	    	
	    case RESET_RECEIVE_DATAS:
	    case RESET_STATUS_RECEIVE_CHARS:
        case RESET_TRANSMIT_DATAS:
	    case RESET_STATUS_TRANSMIT_CHARS:	
        break;
        default:
           system_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_UART_Proc(uart_ch_id, "10.16.02", ERROR_OCCURED);
           return system_status_flag;  	  
    }	
	UART_No_Access(uart_ch_id);
	UART_Transmit_Enable_Ctrl(uart_ch_id, STATE_NO);
	UART_Receive_Enable_Ctrl(uart_ch_id, STATE_NO);
	cur_uart_status_ptr = uart_status + uart_ch_id;	
	if(uart_reset_type == RESET_WHOLE || uart_reset_type == RESET_RECEIVE_DATAS)
	{
		cur_uart_status_ptr->uart_receive_till_end_point = STATE_NO;
		cur_uart_status_ptr->uart_max_num_chars_received = 0;		
	}
	if(uart_reset_type == RESET_WHOLE || uart_reset_type == RESET_RECEIVE_DATAS || uart_reset_type == RESET_STATUS_RECEIVE_CHARS )
	{
		cur_uart_status_ptr->uart_num_chars_received = 0;
	}
	if(uart_reset_type == RESET_WHOLE || uart_reset_type == RESET_TRANSMIT_DATAS)
	{
		 cur_uart_status_ptr->uart_max_num_chars_transmitted = 0;		
		 cur_uart_status_ptr->uart_transmit_till_end_point = STATE_NO;
	}
	if(uart_reset_type == RESET_WHOLE || uart_reset_type == RESET_TRANSMIT_DATAS || uart_reset_type == RESET_STATUS_TRANSMIT_CHARS )
	{
		cur_uart_status_ptr->uart_num_chars_transmitted = 0;
	}
	return SUCCESS;
}	

/*------------------------------------------------------------*
FUNCTION NAME  : UART_Config_Tx_and_Rcv_Ctrl_Datas

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.17 

Bugs           :  
-*------------------------------------------------------------*/
uint16_t UART_Config_Tx_and_Rcv_Ctrl_Datas(const uint8_t uart_ch_id, const uint8_t uart_config_tx_and_rcv_ctrl_datas_type, const uint8_t set_data )
{	
    uart_status_t *cur_uart_status_ptr;	
	
	if(uart_ch_id >= NUM_COMM_DEV_ID_UART_CHS )
	{
		system_status_flag = ERR_DEV_CH_ID_EXCEEDS;
		Error_or_Warning_UART_Proc(uart_ch_id, "10.17.01", ERROR_OCCURED);
		return system_status_flag;
	}
	cur_uart_status_ptr = uart_status +  uart_ch_id;
    switch(uart_config_tx_and_rcv_ctrl_datas_type)
    {
	   case CONFIG_RECEIVE_END_POINT_FLAG:
	     if(set_data > 1)
		 {
			system_status_flag = ERR_DATA_OUT_OF_RANGE;
		    Error_or_Warning_UART_Proc(uart_ch_id, "10.17.02", ERROR_OCCURED);
            return system_status_flag;
		 }	
		 cur_uart_status_ptr->uart_receive_till_end_point = set_data;
      break;		 
	  case CONFIG_MAX_RECEIVE_CHARS:	 
        cur_uart_status_ptr->uart_max_num_chars_received = set_data;
      break;
	  case CONFIG_TRANSMIT_END_POINT_FLAG:
	    if(set_data > 1)
		{
			system_status_flag = ERR_DATA_OUT_OF_RANGE;
		    Error_or_Warning_UART_Proc(uart_ch_id, "10.17.03", ERROR_OCCURED);
            return system_status_flag;
		}	    
		cur_uart_status_ptr->uart_transmit_till_end_point = set_data;
	  break; 
      case CONFIG_MAX_TRANSMIT_CHARS:
	    cur_uart_status_ptr->uart_max_num_chars_transmitted = set_data;
      break;
      default:
         system_status_flag = ERR_FORMAT_INVALID;
		 Error_or_Warning_UART_Proc(uart_ch_id, "10.17.04", ERROR_OCCURED);
         return system_status_flag;  	  
    }
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Print

DESCRIPTION    : Print() operation is similar to printf()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.18 

Bugs           :  
-*------------------------------------------------------------*/
int16_t Print(const char* const format_ptr,...) 
{ 
   va_list arg;
   int16_t num_chars_printed = 0;	
	
	if(format_ptr == NULL_DATA_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("10.18.01", ERROR_OCCURED, system_status_flag);
		return EOF;
	}
	if(*format_ptr == NULL_CHAR)
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
		Error_or_Warning_Proc("10.18.02", ERROR_OCCURED, system_status_flag);
		return EOF;
	}
	va_start(arg, format_ptr); 
	num_chars_printed = VFile_Print(&stdout_vt, format_ptr, &arg);
	va_end(arg);
	if(num_chars_printed == EOF)
	{
		system_status_flag = ERR_EOF;
		Error_or_Warning_Proc("10.18.03", ERROR_OCCURED, system_status_flag);
	}
	return num_chars_printed;
  
} 

/*------------------------------------------------------------*
FUNCTION NAME  : Put_Char

DESCRIPTION    : Put_Char() operation is similar to putchar()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.19 

Bugs           :  
-*------------------------------------------------------------*/
char Put_Char(const char to_disp_char)
{
	file_t *stdout_ptr = &stdout_vt;
	dev_id_t uart_dev_id;
	char displayed_char = to_disp_char;
		
	uart_dev_id.dev_id = COMM_DEV_ID_UART;
	uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;
	/*if((ret_status = Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	{
	   uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;
	} */
	if(displayed_char == NULL_CHAR)
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
	    Error_or_Warning_Proc("10.19.01", ERROR_OCCURED, system_status_flag );
	    return NULL_CHAR;
	}
	if(stdout_ptr->num_chars_stream_buf + stdout_ptr->stream_buf_cur_pos + 1 >= MAX_STREAM_BUFFER_SIZE )
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
	    Error_or_Warning_Proc("10.19.02", ERROR_OCCURED, system_status_flag);
	    return NULL_CHAR;
	}
	if((UART_Transmit_Char(uart_dev_id.dev_ch_id, displayed_char)) != SUCCESS)
	{
		system_status_flag = ERR_UART_TX_PROC;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.19.03", ERROR_OCCURED);
	    return NULL_CHAR;
	}	
	return displayed_char;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Put_Str

DESCRIPTION    : Put_Str() operation is similar to puts()
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.20 

Bugs           :  
-*------------------------------------------------------------*/
int16_t Put_Str(const char *const to_disp_str)
{
	file_t *stdout_ptr = &stdout_vt;
    dev_id_t uart_dev_id;
	uint8_t num_chars_displayed = 0;
	uint8_t to_disp_str_len;
	
	if(to_disp_str == NULL_DATA_PTR)
	{
		system_status_flag = ERR_NULL_PTR;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.20.01", ERROR_OCCURED);
	    return EOF;
	}
	if((to_disp_str_len = Str_Len(to_disp_str)) == 0)
	{
		system_status_flag = ERR_STR_LEN_OPER;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.20.02", ERROR_OCCURED);
	    return EOF;
	}
	if(stdout_ptr->num_chars_stream_buf + stdout_ptr->stream_buf_cur_pos + to_disp_str_len + 1 >=  MAX_STREAM_BUFFER_SIZE )
	{
		system_status_flag = ERR_DATA_OUT_OF_RANGE;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.20.03", ERROR_OCCURED);
	    return EOF;	     
	}
	uart_dev_id.dev_id = COMM_DEV_ID_UART;
	uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;
	/* if((Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	{
	    uart_dev_id.dev_ch_id = TRACE_UART_CH_ID;
	}*/	
	if((UART_Transmit_Str(uart_dev_id.dev_ch_id, to_disp_str)) != SUCCESS)
	{
		system_status_flag = ERR_UART_TX_PROC;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.20.04", ERROR_OCCURED);
	    return EOF;
	}
	num_chars_displayed = to_disp_str_len;	
	if((UART_Transmit_Char(uart_dev_id.dev_ch_id, NEW_LINE_CHAR)) != SUCCESS)
	{
		system_status_flag = ERR_UART_TX_PROC;
	    Error_or_Warning_UART_Proc(TRACE_UART_CH_ID, "10.20.05", ERROR_OCCURED);
	    return EOF;
	}	
	return num_chars_displayed;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Error_or_Warning_UART_Proc

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.21 

Bugs           :  
-*------------------------------------------------------------*/
static uint16_t Error_or_Warning_UART_Proc(const uint8_t uart_ch_id, const char *const error_trace_str, const uint8_t warn_or_error_format)
{
 	if(uart_ch_id < NUM_COMM_DEV_ID_UART_CHS && uart_ch_id != TRACE_UART_CH_ID)
	{
	     Error_or_Warning_Proc(error_trace_str, warn_or_error_format, system_status_flag);
	}
	else
	{
		Error_or_Warning_Occured_Proc(warn_or_error_format, system_status_flag);
		if((Comm_Error_Proc(error_trace_str, warn_or_error_format, COMM_ERROR_BY_LED_AND_OR_LCD, system_status_flag ))!= SUCCESS)
	    {
		   system_status_flag = ERR_PROC_ERROR;		
		   return system_status_flag;
	    }
	}
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : ISR_UART_0

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.22 

Bugs           :  
-*------------------------------------------------------------*/
void ISR_UART_0(void) __irq
{
	VICVectAddr = 0x0;
}

/*------------------------------------------------------------*
FUNCTION NAME  : ISR_UART_0

DESCRIPTION    : 
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 10.23 

Bugs           :  
-*------------------------------------------------------------*/
void ISR_UART_1(void) __irq
{
	VICVectAddr = 0x0;
}

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

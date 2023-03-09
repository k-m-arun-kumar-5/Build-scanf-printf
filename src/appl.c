/* ********************************************************************
FILE                   : appl.c

PURPOSE                :  Application 
	 
AUTHOR                : K.M. Arun Kumar alias Arunkumar Murugeswaran
	 
KNOWN BUGS            : 

NOTE                  : 
						
CAUTION               :  
                                    
CHANGE LOGS           :  

FILE ID               : 11

*****************************************************************************/
#include "main.h"
#include "appl.h"

/* ------------------------------ macro defination ------------------------------ */

/* ----------------------------- global variable defination --------------------- */ 
uint32_t appl_status_flag = NO_ERROR;
			
/* ----------------------------- global variable declaration -------------------- */


/* ----------------------------- global function declaration -------------------- */

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.01

BUGS           :    
-*------------------------------------------------------------*/
uint16_t Appl_Proc(void)
{	
     dev_id_t uart_dev_id;
     char read_str[MAX_STREAM_BUFFER_SIZE + 1], input_str[MAX_STREAM_BUFFER_SIZE + 1];
     uint32_t uint32_num = 30;
	 int32_t int32_num = -40;
	 int16_t int16_num = -20;	
	 float float_num = -20.56;
	 uint16_t ret_status;
     unsigned char last_read_char;
     uint8_t uart_transmit_char_disp_format, uint8_num = 97;
	 static uint8_t loop_flag = 0, input_fsm_state = INPUT_FLOAT_AND_NUM;
	 
	   switch(cur_data_id) 
	   {	
	    case DATA_ID_STDIN_STDOUT: 
		   if((loop_flag & (1 << 0)) == 0)
		   {
			    LCD_Disp_Goto_XY(CH_ID_00, LINE_NUM_01, COL_NUM_01);
				LCD_Disp_Str(CH_ID_00, "Hello");
			    strcpy(read_str, "-38.476334567 35");
			    Print("Read_Str: %s \r", read_str);							
                Str_Scan(read_str, "%f %d", &float_num , &uint8_num);
                Print("Str_Scan: float = %.9f, num = %d \r",float_num , uint8_num); 
			    Print("uint32_num : 0x%-4X, %% hello\r", uint32_num);   
		        Print("Print: float_num = %09.9f, ", float_num); 
	            Print("Print: uint32_num : 0x%-4X, %% hello \rEnter a float & num : ", uint32_num);    				
				loop_flag |= (1 << 0);
		   }			
         	ret_status = Cur_Data_ID_Read_Oper_Func(DEV_RECEIVE_TYPE, &last_read_char);				
		      switch(ret_status)
	        {
		        case CUR_DATA_ID_UNTIL_LAST_TRY_COMPLETE_READ_AND_READY_PROCESS:
            case CUR_DATA_ID_LAST_TRY_COMPLETE_READ_AND_READY_PROCESS:			 
			      case CUR_DATA_ID_READ_MAX_TRIED:
               if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_STATUS_DATA_STR, input_str)) != SUCCESS)
               {
                     appl_status_flag = ERR_APPL_DATA_RETRIEVE;
					 Error_or_Warning_Proc("11.01.01", ERROR_OCCURED, appl_status_flag);
					 return appl_status_flag; 
               }
              switch(input_fsm_state)
              {
				  case INPUT_FLOAT_AND_NUM:
                     int16_num = Str_Scan(input_str, "%f %d", &float_num, &int32_num);	
                     Print("\rEntered float = %f, float * 3.5 = %10.9f, ", float_num, 3.5 * float_num);
					 Print("num = %d\rEnter a str : ", int32_num);
                     input_fsm_state = INPUT_STR;					 
                  break;
				  case INPUT_STR:
				     int16_num = Str_Scan(input_str, "%10s", read_str);	
                     Print("\rEntered str: %s \rEnter a 2 numbers : ", read_str);
                     input_fsm_state = INPUT_TWO_NUMS;
				  break;
                  case INPUT_TWO_NUMS:
                     Str_Scan(input_str, "%d %d",&int32_num, &uint32_num);	
                     Print("\rScan: Entered num1 = %d, num2 = %d \r", int32_num , uint32_num); 
                     Print("Enter a float & num : ");  
                     input_fsm_state = INPUT_FLOAT_AND_NUM;                                            
                  break; 				  
                  default:
                     appl_status_flag = ERR_FORMAT_INVALID;
					 Error_or_Warning_Proc("11.01.02", ERROR_OCCURED, appl_status_flag);
					 return appl_status_flag; 
                    
              }
			  if((Reset_Data_ID_Status(cur_data_id, DATA_ID_RESET_WHOLE_STATUS)) != SUCCESS)
		      {
			      system_status_flag = ERR_RESET_DATA_ID_STATUS;
		          Error_or_Warning_Proc("11.01.03", ERROR_OCCURED, system_status_flag);
		          return system_status_flag;
		      }
           break;
           case CUR_DATA_ID_VALID_CHAR_READ: 
  			      uart_dev_id.dev_id = COMM_DEV_ID_UART;
	              if((Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	              {
	                 appl_status_flag = ERR_UART_NOT_DATA_SRC;
	                 Error_or_Warning_Proc("11.01.04", ERROR_OCCURED, appl_status_flag);
	                 return appl_status_flag;
	              }
			      if((Appl_Data_Retrieve_Para(cur_data_id, DATA_RETRIEVE_CTRL_DATA_DISP_FORMAT, &uart_transmit_char_disp_format)) != SUCCESS)
			      {
					   appl_status_flag = ERR_APPL_DATA_RETRIEVE;
					   Error_or_Warning_Proc("11.01.05", ERROR_OCCURED, appl_status_flag);
					   return appl_status_flag; 
			      }
			      if((UART_Transmit_Ctrl_Proc(uart_dev_id.dev_ch_id, uart_transmit_char_disp_format, last_read_char)) != SUCCESS)
			      {
				     appl_status_flag  = ERR_UART_TRANSMIT;
					  Error_or_Warning_Proc("11.01.06", ERROR_OCCURED, appl_status_flag);
					  return appl_status_flag;
			      }
				  break;
			 case CUR_DATA_ID_REACH_MAX_CHARS_READ_WAIT_TERMINATOR_CHAR:			   
			 break;
             case CUR_DATA_ID_NO_INPUT_DEV:	
			 case CUR_DATA_ID_NO_RECEIVER_DEV:
		     case WARN_CUR_DATA_ID_DEV_DISABLED:
		     case WARN_CUR_DATA_ID_DEV_NO_ACCESS:		         
		     case SW_OR_KEY_NOT_PRESSED:
			 case COMM_NO_RCV_CHAR:
		     case WARN_CUR_DATA_ID_SPECIAL_CHAR_READ_BUT_DISABLED:
		     case WARN_CUR_DATA_ID_NUM_CHAR_READ_BUT_DISABLED:
		     case WARN_CUR_DATA_ID_SMALL_ENGLISH_ALPHA_CHAR_READ_BUT_DISABLED:
		     case WARN_CUR_DATA_ID_BIG_ENGLISH_ALPHA_CHAR_READ_BUT_DISABLED:
		     case WARN_CUR_DATA_ID_CTRL_CHAR_READ_BUT_DISABLED:
             break;		      			 
             case VALID_CTRL_CHAR_READ:			  
			    switch(last_read_char)
				{
					case BACKSPACE_CHAR:
					case ENTER_CHAR:
					   uart_dev_id.dev_id = COMM_DEV_ID_UART;
	                   if((Data_ID_Src_Spec_Dev_Find_Dev_Ch_ID(cur_data_id, &uart_dev_id, DEV_TRANSMIT_TYPE))!= SUCCESS)
	                   {
	                       appl_status_flag = ERR_UART_NOT_DATA_SRC;
	                       Error_or_Warning_Proc("11.01.07", ERROR_OCCURED, appl_status_flag);
	                       return appl_status_flag;
	                   }
					   if((UART_Transmit_Char(uart_dev_id.dev_ch_id, last_read_char)) != SUCCESS)
			           {
			             appl_status_flag  = ERR_UART_TRANSMIT;
			              Error_or_Warning_Proc("11.01.08", ERROR_OCCURED, appl_status_flag);
			              return appl_status_flag; 
			           }
					break;					
                    default:
                       appl_status_flag = ERR_DEV_OPER_READ_FUNC;
			                 Error_or_Warning_Proc("11.01.09", ERROR_OCCURED, appl_status_flag);
			                return appl_status_flag;  		  
	            }
			 break;
             default:
                appl_status_flag = ERR_DEV_OPER_READ_FUNC;
			    Error_or_Warning_Proc("11.01.10", ERROR_OCCURED, appl_status_flag);
			    return appl_status_flag;  	
	        }
		 break;	
		 default:
		   appl_status_flag = ERR_CUR_DATA_ID_INVALID;
		   Error_or_Warning_Proc("11.01.11", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
	 } 
     return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.02  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Appl_Init(const void *const data_ptr)
{	
  if((Next_Data_Conf_Parameter(DATA_ID_STDIN_STDOUT)) != SUCCESS)
	{
		 appl_status_flag = ERR_NEXT_DATA_CONF;
		 Error_or_Warning_Proc("11.02.01", ERROR_OCCURED, system_status_flag);
		 return system_status_flag;
	}
  if((LCD_Enable(CH_ID_00)) != SUCCESS)
	{
		appl_status_flag = ERR_DEV_ENABLE_PROC;
		Error_or_Warning_Proc("11.02.02", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	if((UART_Transmit_Enable_Ctrl(CH_ID_00, STATE_YES)) != SUCCESS)
	{
		appl_status_flag = ERR_DEV_ENABLE_PROC;
		Error_or_Warning_Proc("11.02.03", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	if((UART_Receive_Enable_Ctrl(CH_ID_00, STATE_YES)) != SUCCESS)
	{
		appl_status_flag = ERR_DEV_ENABLE_PROC;
		Error_or_Warning_Proc("11.02.04", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}
	if((UART_Enable(CH_ID_00)) != SUCCESS)
	{
		appl_status_flag = ERR_DEV_ENABLE_PROC;
		Error_or_Warning_Proc("11.02.05", ERROR_OCCURED, system_status_flag);
		return system_status_flag;
	}	
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_HW_Init

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : First initialise independent SW in order from SW_CH_ID from 0 if any, then initialize Keyboard, if any.

Func ID        : 11.03  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Appl_HW_Init(void)
{
    if((UART_Init(TRACE_UART_CH_ID, COMM_TRANSMIT_FUNC)) != SUCCESS)
	{
		return FAILURE;
	}
	if((UART_Init(TRACE_UART_CH_ID, COMM_RECEIVE_FUNC)) != SUCCESS)
	{
		return FAILURE;
	}
	if((UART_Init(TRACE_UART_CH_ID, DEV_INIT_OPER)) != SUCCESS)
	{
		return FAILURE;
	}
	if(internal_error_led_io_ch < IO_CH_INVALID)
	{
	   if((Output_Dev_Init(internal_error_led_io_ch, 1)) != SUCCESS)
	   {
	      return FAILURE;
	   }
	}
	if((LCD_Init(CH_ID_00, DATA_NA)) != SUCCESS)
	{
		return FAILURE;
	} 
	#ifdef TRACE_FLOW
	   Print("TRA: HW Init over\r");
	#endif
	return SUCCESS; 
}

/*------------------------------------------------------------*
FUNCTION NAME  : Error_or_Warning_Occured_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.04

BUGS           :    
-*------------------------------------------------------------*/
uint16_t Error_or_Warning_Occured_Proc(const uint8_t warn_or_error_format, const uint32_t warning_or_error_code)
{
	switch(warn_or_error_format)
	{
		case WARNING_OCCURED:			
		  if((error_or_warning_proc_flag & (1 << 0)) == 0)
		  {
			  /* warning occured, process to do  */ 
			  error_or_warning_proc_flag |= (1 << 0);
		  }		 
		break;
		case ERROR_OCCURED:
		   if((error_or_warning_proc_flag & (1 << 1)) == 0)
		   {
		       /*error occured, process to do  */
			   error_or_warning_proc_flag |= (1 << 1);
			   error_or_warning_proc_flag |= (1 << 0); 
		   }		   
		break;
		case FATAL_OCCURED:
		  if((error_or_warning_proc_flag & (1 << 2)) == 0)
		  {
		      /* fatal error occured, process to do */
			  error_or_warning_proc_flag |= (1 << 2);
			  error_or_warning_proc_flag |= (1 << 1);
			  error_or_warning_proc_flag |= (1 << 0); 
		  }		  
        break;
		default:
		   error_or_warning_proc_flag |= (1 << 3);
           system_status_flag = ERR_FORMAT_INVALID;		
		   return system_status_flag;
	}	
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Reset

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.05

BUGS           :    
-*------------------------------------------------------------*/
 uint16_t Appl_Reset(const uint8_t reset_type)
{
	switch(reset_type)
	{
		case RESET_APPL:	
           appl_status_flag = NO_ERROR;	
		   if((Appl_Reset_Proc(DATA_NA)) != SUCCESS)
		   {
			    appl_status_flag = ERR_APPL_RESET_PROC;
		        Error_or_Warning_Proc("11.05.01", ERROR_OCCURED, appl_status_flag);
		        return appl_status_flag;
		   }
		break;
		default:
		   appl_status_flag = ERR_FORMAT_INVALID;
		   Error_or_Warning_Proc("11.05.02", ERROR_OCCURED, appl_status_flag);
		   return appl_status_flag;
	}
	return SUCCESS;
} 

/*------------------------------------------------------------*
FUNCTION NAME  : Appl_Reset_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.06
BUGS           :    
-*------------------------------------------------------------*/ 
uint16_t Appl_Reset_Proc(const uint8_t reset_data)
{
	return SUCCESS;
}	

#ifdef TIMER_MOD_ENABLE 
/*------------------------------------------------------------*
FUNCTION NAME  : Timer_0_Timeout_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.07  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Timer_0_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_0_or_counter_data_ptr)
{
	uint16_t ret_status;
	
	if(timer_0_or_counter_data_ptr == NULL_DATA_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("11.07.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	if(timer_0_or_counter_data_ptr->timer_or_counter_run_id != TMR_OR_COUNTER_SYSTEM_TICK_STATE)
	{
		appl_status_flag = ERR_TIMER_ID_EXCEEDS;
		Error_or_Warning_Proc("11.07.02", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
    /* --------------------------------------- User Code: Begin ------------------------------------------ */ 
	
	/* --------------------------------------- User Code: End   ------------------------------------------ */ 
	switch(timeout_type)
	{
		 case TMR_NO_MAX_NUM_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_NO_MAX_NUM_TIMEOUT_PROC;
	     break;		
		 case TMR_AT_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_AT_LAST_TIMEOUT_PROC;
		 break;  
		 case TMR_BEFORE_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_BEFORE_LAST_TIMEOUT_PROC;
		 break;
         case TMR_MAX_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_MAX_NUM_TIMEOUT_PROC; 
         break;		
         default:
             appl_status_flag = ERR_TMR_TIMEOUT_TYPE_INVALID;
		     Error_or_Warning_Proc("11.07.03", ERROR_OCCURED, appl_status_flag);
		     return appl_status_flag;
	}
    return ret_status;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Timer_1_Timeout_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.08 

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Timer_1_Timeout_Proc(const uint16_t timeout_type, volatile const timer_or_counter_data_t *const timer_1_or_counter_data_ptr)
{
	    uint16_t ret_status;
    
    if(timer_1_or_counter_data_ptr == NULL_DATA_PTR)
	{
		appl_status_flag = ERR_NULL_PTR;
		Error_or_Warning_Proc("11.08.01", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	if(timer_1_or_counter_data_ptr->timer_or_counter_run_id >= NUM_TIMER_AND_COUNTER_IDS)
	{
		appl_status_flag = ERR_TIMER_ID_EXCEEDS;
		Error_or_Warning_Proc("11.08.02", ERROR_OCCURED, appl_status_flag);
		return appl_status_flag;
	}
	/* --------------------------------------- User Code: Begin ------------------------------------------ */ 
	
		
	/* --------------------------------------- User Code: End   ------------------------------------------ */ 
	
	 switch(timeout_type)
	 {
		 case TMR_NO_MAX_NUM_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_NO_MAX_NUM_TIMEOUT_PROC;
	     break;		
		 case TMR_AT_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_AT_LAST_TIMEOUT_PROC;
		 break;  
		 case TMR_BEFORE_LAST_TIMEOUT_YET_PROCESS:
		   ret_status = TMR_BEFORE_LAST_TIMEOUT_PROC;
		 break;
         case TMR_MAX_TIMEOUT_YET_PROCESS:
		    ret_status = TMR_MAX_NUM_TIMEOUT_PROC; 
         break;		 
         default:
		     appl_status_flag = ERR_TMR_TIMEOUT_TYPE_INVALID;
		     Error_or_Warning_Proc("11.08.05", ERROR_OCCURED, appl_status_flag);
		     return appl_status_flag;
	 }
    return ret_status;	
}

#endif

#ifdef EXT_INTERRUPT_MOD_ENABLE
/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_0_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.09  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_0_Proc(void)
{
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_1_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.10  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_1_Proc(void)
{
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_2_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.11  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_2_Proc(void)
{
	return SUCCESS;
}

/*------------------------------------------------------------*
FUNCTION NAME  : Ext_Interrupt_3_Proc

DESCRIPTION    :
								
INPUT          : 

OUTPUT         : 

NOTE           : 

Func ID        : 11.12  

BUGS           :              
-*------------------------------------------------------------*/
uint16_t Ext_Interrupt_3_Proc(void)
{
	return SUCCESS;
}

#endif

		    
/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

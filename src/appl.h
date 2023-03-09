/**************************************************************************
   FILE          :    appl.h
 
   PURPOSE       :    application header.  
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _APPL_H
#define _APPL_H

 
/* ---------------------- macro defination ------------------------------------------------ */

/* ---------------------- data type defination -------------------------------------------- */
typedef enum 
{
    INPUT_STR, INPUT_FLOAT_AND_NUM, INPUT_TWO_NUMS 
} input_data_format_t;

typedef enum
{
	ERR_UART_NOT_DATA_SRC = NUM_SYS_STATUS, ERR_UART_TRANSMIT, NUM_APPL_STATUS
} appl_status_t; 
 
 /* -------------------- public variable declaration --------------------------------------- */
extern uint32_t appl_status_flag;

/* -------------------- public function declaration --------------------------------------- */
uint16_t Appl_Reset(const uint8_t reset_type);
uint16_t Appl_Reset_Proc(const uint8_t );

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

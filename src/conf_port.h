
/**************************************************************************
   FILE          :    conf_port.h
 
   PURPOSE       :    port header - define port and its pin assignment.
 
   AUTHOR        :     K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :    port header 
 To avoid this header file to be included more than once, conditional directive is used  
	
  CHANGE LOGS     :  
	   
 **************************************************************************/
#ifndef _CONF_PORT_H
#define _CONF_PORT_H

/* ---------------------- macro defination ------------------------------------------------ */
	

/* ---------------------- data type defination -------------------------------------------- */
typedef enum 
{
 	INTERNAL_ERROR_LED_IO_CH = (IO_CH_07) 
} io_ch_map_led_ch_t;
 

#ifdef LCD_MOD_ENABLE
//                                    IO_CH_CMD_RS,  IO_CH_CMD_RW, IO_CH_CMD_E, IO_CH_DATA_0, IO_CH_DATA_1,    IO_CH_DATA_2,                              
const uint8_t lcd_io_ch_ctrl[][3 + 8] = {
	                                 {IO_CH_48,     IO_CH_29,       IO_CH_23,    IO_CH_INVALID, IO_CH_INVALID, IO_CH_INVALID, 
									 
//		                               IO_CH_DATA_3,  IO_CH_DATA_4, IO_CH_DATA_5, IO_CH_DATA_6, IO_CH_DATA_7 							 
									  IO_CH_INVALID, IO_CH_49,       IO_CH_54,     IO_CH_57,       IO_CH_61 }
                               };
#endif 

/* -------------------- public variable declaration --------------------------------------- */
 
/* -------------------- public function declaration --------------------------------------- */

 
#endif 

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

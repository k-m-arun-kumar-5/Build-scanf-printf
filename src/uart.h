/**************************************************************************
   FILE          :    uart.h
 
   PURPOSE       :      
 
   AUTHOR        :   K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _UART_H
#define _UART_H

/* ---------------------- macro defination ------------------------------------------------ */

/* ---------------------- data type defination -------------------------------------------- */

typedef enum
{
	STOP_BIT_1, STOP_BIT_1_5_OR_2
} stop_select_t;

typedef struct
{
  uint16_t baud_rate;
  uint8_t access_flag               : 1;
  uint8_t enable_flag               : 1;
  uint8_t transmit_enable_flag      : 1;
  uint8_t receive_enable_flag       : 1; 
  uint8_t echo_receive_char         : 1;
  uint8_t stop_select               : 1; 
  uint8_t                           : 2; 
  uint8_t data_bit_len              : 3;  // FOR 8 bit data bit len, then, data_bit_len = 8 - 1.  
  uint8_t error_check               : 2;   
  uint8_t                           : 3;
} uart_ctrl_t; 

typedef struct
{
	uint8_t uart_transmit_io_ch;
	uint8_t uart_receive_io_ch;
	uint8_t uart_num_chars_transmitted;
    uint8_t	uart_num_chars_received;
	uint8_t uart_max_num_chars_transmitted;
	uint8_t uart_max_num_chars_received; 
	uint8_t uart_transmit_till_end_point   : 1;
    uint8_t uart_receive_till_end_point    : 1;
	uint8_t                                : 6;
} uart_status_t;

typedef struct
{
	disp_num_fmt_t  disp_num_fmt;
	char *uart_disp_str;
	int32_t uart_disp_num;
	uint8_t uart_use_data_status_flag       :  1;
	uint8_t uart_data_till_end_point_flag   :  1;
	uint8_t                                 :  6;
} uart_transmit_ctrl_t;

/* -------------------- public variable declaration --------------------------------------- */
extern uart_ctrl_t uart_ctrl[];
extern uart_status_t uart_status[];
extern file_t stdout_vt; 

/* -------------------- public function declaration --------------------------------------- */
uint16_t UART_Init(const uint8_t uart_ch_id, const uint8_t init_mode);
uint16_t UART_Transmit_Char(const uint8_t uart_ch_id, const unsigned char uart_tx_char);
uint16_t UART_Receive_Char(const uint8_t uart_ch_id, void *const dev_input_ctrl_para_ptr);
uint16_t UART_Transmit_Num(const uint8_t uart_ch_id, const uint8_t num_data_type, const disp_num_fmt_t disp_num_fmt, const int32_t uart_num);
uint16_t UART_Transmit_Str(const uint8_t uart_ch_id, const char *const transmit_str);
uint16_t UART_Disable(const uint8_t uart_ch_id);
uint16_t UART_Enable(const uint8_t uart_ch_id);
uint16_t Reset_All_UART_Chs(const uint8_t uart_reset_type);
uint16_t UART_Allow_Access(const uint8_t uart_ch_id);
uint16_t UART_No_Access(const uint8_t uart_ch_id);
uint16_t UART_DeInit(const uint8_t uart_ch_id, const uint8_t deinit_mode);
uint16_t UART_Transmit_Enable_Ctrl(const uint8_t uart_ch_id, const uint8_t transmit_enable_flag);
uint16_t UART_Receive_Enable_Ctrl(const uint8_t uart_ch_id, const uint8_t receive_enable_flag);
uint16_t UART_Transmit(const uint8_t uart_ch_id, const void *const uart_transmit_ctrl_para_ptr);
uint16_t UART_Transmit_Ctrl_Proc(const uint8_t uart_ch_id, const uint8_t uart_ctrl_format, const char uart_transmit_char );
uint16_t Reset_UART_Ch(const uint8_t uart_ch_id, const uint8_t uart_reset_type);
uint16_t UART_Config_Tx_and_Rcv_Ctrl_Datas(const uint8_t uart_ch_id, const uint8_t uart_config_tx_and_rcv_ctrl_datas_type, const uint8_t set_data );
int16_t Print(const char* const format_str,...);
char Put_Char(const char to_disp_char);
int16_t Put_Str(const char *const to_disp_str);

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

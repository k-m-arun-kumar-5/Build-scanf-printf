/**************************************************************************
   FILE          :    conf_appl.h
 
   PURPOSE       :   main application configuration Header
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   
  
  CHANGE LOGS    :
	   
 **************************************************************************/
 
 #ifndef _CONF_APPL_H
 #define _CONF_APPL_H 
 
/* -------------------------------- Debug Conf -----------------------------------------*/
//#define TRACE                                 (1U)
#define TRACE_ERROR                             (2U)
#define TRACE_REQ                               (3U)
#define TRACE_INFO                              (4U)
//#define TRACE_DATA                              (5U)
#define TRACE_FLOW                              (6U)

#define TRACE_LCD_CH_ID                             (CH_ID_00)
#define TRACE_UART_CH_ID                            (CH_ID_00) 

/* -------------------------------  Macro Conf ------------------------------------------*/
//#define SHOULD_REMOVE                 (7)
#define SHOULD_CALL                     (8)
//#define TEST                          (9)                                                  
//#define SW_PRESS_HOLD_CPU             (10)
//#define DMA_DATA_ID_STR               (11)
//#define STR_TO_NUM_FROM_RIGHT_TO_LEFT (12)
//#define DELAY_BY_CLOCK                (13) 

/* ------------------------------ Appl Data Types --------------------------------------- */

typedef enum 
{
   DATA_ID_STDIN_STDOUT, NUM_DATA_IDS    	
} appl_data_id_t;

/*------------------------------- LCD Disp Conf ------------------------------------------*/
#ifdef LCD_MOD_ENABLE

   #define NUM_CHARS_INPUTDATA        GIVEN_CHARS_MAX_CONFIG_LINES_AND_COLS 
   #define RESET_LCD_DISP_LOC         (BEGIN_LOC_LINE_NUM_01)
   #define RESET_LCD_INPUT_LOC        (BEGIN_LOC_LINE_NUM_02)
   
   #define ERROR_LINE_NUM             (LINE_NUM_04)   
   
#endif

/*----------------------------------- SW Conf ------------------------------------------- */
#ifdef SW_MOD_ENABLE

#define MAX_COUNT_LONG_PRESSKEY_TIMEOUT          (3U)
#define MAX_TIMEOUT_NO_KEY_PRESS_IN_SEC          (10)  
#define MAX_TIMEOUT_LONG_PRESS_KEY_IN_SEC         (5)

#endif

/* -------------------------------Timer State Conf ---------------------------------------*/
#ifdef TIMER_MOD_ENABLE

#endif

/*------------------------------- Ext Interrupt Conf ------------------------------------------*/
#ifdef EXT_INTERRUPT_MOD_ENABLE

#endif

/* ------------------------------ ADC input signal val Conf ------------------------------*/
#ifdef ADC_MOD_ENABLE

#define ADC_VREF_IN_MV                     (3300)

#endif


/* ------------------------------- Application Conf --------------------------------------*/

#define HIDDEN_KEY_DISP_CHAR                      ('X') 
#define DATA_MAX_NUM_ALLOCATED_CHARS              (20) 

#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

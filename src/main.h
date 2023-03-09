/**************************************************************************
   FILE          :    main.h
 
   PURPOSE       :    project header.  
 
   AUTHOR        :    K.M. Arun Kumar alias Arunkumar Murugeswaran
 
  KNOWN BUGS     :
	
  NOTE           :   PROJECT header- groups the key information about the mcrocontroller device you have used, along with other key
parameters – such as the oscillator frequency and commonly used information such as common data types in the project
	
  CHANGE LOGS     :
	   
 **************************************************************************/
 
 /* to avoid this header file to be included more than once, conditional directive is used  */
#ifndef _MAIN_H
#define _MAIN_H

/* Must include the appropriate microcontroller header file here eg for eg if we use LPC 2138 uC device.
 In most case, microcontroller header is also a device header.in our case, lpc213x.h
 device header will include the addresses of the special function registers (SFRs) used for port access, plus similar
 details for other on-chip components such as analog-to-digital converters*/
 /* lpc213x.h is a system header and for <> enclosed one, preprocessor will search the lpc213x.h in predetermined directory path to locate the header file. */
 // Must include the appropriate microcontroller header file here
#include <lpc213x.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "constants.h"
#include "sys_status.h"
#include "conf_io.h"
#include "conf_appl.h"
#include "isr.h"
#include "io_lib.h"
#include "gen_std_lib.h"
#include "dev_oper.h"
 
/* ---------------------- macro defination ------------------------------------------------ */

// for most instruction, number of oscillations required to execute per instruction
// 1 – ARM
#define OSC_PER_INST                            (1)

#define MAX_UINT_VAL                                  (0xFFFFFFFF)
#define INT_BIT_SIZE                                  (32)

/* ---------------------- data type defination -------------------------------------------- */

 
/* -------------------- public variable declaration --------------------------------------- */


/* -------------------- public function declaration --------------------------------------- */
#endif

/*------------------------------------------------------------------*-
  ------------------------ END OF FILE ------------------------------
-*------------------------------------------------------------------*/

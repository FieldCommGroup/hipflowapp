/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: This holds the configuration data for the initial configuration
 *	
 *		
 *	
 * #include "configuration_Default.h"
 */
#pragma once

#ifndef _CONFIGURE_DEFAULT_H
#define _CONFIGURE_DEFAULT_H
#ifdef INC_DEBUG
#pragma message("In configuration_Default.h") 
#pragma message("NO INCLUDES in configuration_Default.h") 
#endif


#ifdef INC_DEBUG
#pragma message("    Finished Includes::configuration_Default.h") 
#endif

#ifdef NDEBUG
// this needs to be defin3ed on the command line (ie property box) #define _IS_HART7
#endif

/* some of these may change from NV memory...eg INIT_CONFIG_CNT*/
#ifdef _PRESSUREDEVICE
#define INIT_DEVTYPE_HI		0xf9
#define INIT_DEVTYPE_LO		0xfc

#define INIT_DEV_ID0        0xbc 
#define INIT_DEV_ID1        0x61
#define INIT_DEV_ID2        0x4e

#define INIT_TAG            "PIPRS"
#define INIT_LONG_TAG       "NATIVE_PRESS"
#else
  #ifdef _FLOWDEVICE
    #define INIT_DEVTYPE_HI		0xf9
    #define INIT_DEVTYPE_LO		0xfd
#define INIT_DEV_ID0        0xbc 
#define INIT_DEV_ID1        0x68
#define INIT_DEV_ID2        0x4e
#define INIT_TAG            "NATIVEPI"
#define TAG_LEN             8
#define INIT_LONG_TAG       "NATIVE_PI_FLOW"
#define LONG_LEN            14          
  #else
    #define INIT_DEVTYPE_HI		0xf9
    #define INIT_DEVTYPE_LO		0xfd
#define INIT_DEV_ID0        0xbc 
#define INIT_DEV_ID1        0x61
#define INIT_DEV_ID2        0x4f
#define INIT_TAG            "PINON"
#define INIT_LONG_TAG       "NATIVE_PI"
  #endif
#endif
#define INIT_POLLADDR		0x00
#define INIT_REQ_PREAMBL	0x00
#define INIT_MASTER_PREAMBL	0x00
#ifdef _IS_HART7
#define INIT_UNIV_REV		0x07
#define INIT_BURST_ENABLE   0x04
#else
  #ifdef _IS_HART6
#define INIT_UNIV_REV		0x06
#define INIT_BURST_ENABLE   0x04
  #else  /* must be hart 5 */
#define INIT_UNIV_REV		0x05  /* hart opc server will crash on Univ Rev > 5 */
#define INIT_BURST_ENABLE   0x00  /* turn 'em off while debugging */
  #endif
#endif
#define INIT_DEV_REV		0x01
#define INIT_SW_REV			0x04

#define INIT_HW_REV			0x48  /* top 5 bits...ie 9 */
#define INIT_SIG_CODE       0x06  /* bottom 3 bits, and with above*/
#define INIT_BYTE_7         ( ((INIT_HW_REV & 0x1f)<<3) | (INIT_SIG_CODE & 0x07) )
#define INIT_FLAGS          0x00
#define INIT_MY_PREAMBL     0x00
#define INIT_DEV_VAR_CNT    0x03  /* without the 'standard's */
#define INIT_CONFIG_CNT    {0x00, 0x01}
#define INIT_EXT_DEV_STAT   0x00
#define INIT_MFG_ID		   {0x00, 0xf9}
#define INIT_PRIV_ID       {0x00, 0xf9}
#define INIT_DEV_PROFILE    0x41


#endif //_CONFIGURE_DEFAULT_H

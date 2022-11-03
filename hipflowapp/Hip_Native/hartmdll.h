/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
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
 *****************************************************************/

/**********************************************************
 *
 * File Name:
 *   hartmdll.h
 * File Description:
 *   Header file for the HART Master Data Link Layer 
 *   library.  This file contains prototypes for all user 
 *   accessible functions.  To add support for more HART 
 *   commands, or to remove support for some commands, 
 *   modify the DEVDATA structure here, and change the 
 *   two happ_ functions in happ.c.
 *
 **********************************************************/
#ifndef _HARTMDLL_H
#define _HARTMDLL_H

#include "common.h"
#include "lnkctrl.h"
#include "hartdefs.h"


/****************
 *  Definitions
 ****************/
#define HART_FRAME_EXT             0
#define HART_FRAME_SHORT           1

#define HART_MAX_POLL_ADDR         63
#define HART_QUEUE_DEPTH           1
#define HART_PREAMBLE              0xFFu

#define DELIM_POLLADDR             0x02
#define DELIM_UNIQADDR             0x82u
#define BURST_MODE                 0x40
#define PRIM_MASTER                0x80u
#define NO_BRST_NO_PRIM            (~(BURST_MODE | PRIM_MASTER))
#define POLL_ADDR_LEN              1

#define HART_TIME_1MS              32      // 1 bit = 1/32ms
#define HART_TIME_1SEC             ((HART_TIME_1MS) * (MSEC_PER_SEC))


/*
 * Lengths for standard HART text fields.  These do not include space for
 * the null string terminator.
 */
#define HART_DESCRIPT_LEN          16
#define HART_LONGTAG_LEN           32
#define HART_MESSAGE_LEN           32
#define HART_TAG_LEN               8


/*
 * Universal HART Commands
 */
#define CMD_READ_UID               0
#define CMD_READ_PV                1
#define CMD_READ_PVMA              2
#define CMD_READ_VARS              3
#define CMD_WRITE_POLLADDR         6
#define CMD_READ_UID_BYTAG         11
#define CMD_READ_MESSAGE           12
#define CMD_READ_TAG_DESC_DATE     13
#define CMD_READ_PV_SENSOR_INFO    14
#define CMD_READ_PV_OUTPUT_INFO    15
#define CMD_READ_FAN               16
#define CMD_WRITE_MESSAGE          17
#define CMD_WRITE_TAG_DES_DATE     18
#define CMD_WRITE_FAN              19

/*
 * Common Practice HART Commands
 */
#define CMD_WRITE_PV_DAMPING       34
#define CMD_WRITE_PV_RANGE         35
#define CMD_SET_PV_UPPER_RANGE     36
#define CMD_SET_PV_LOWER_RANGE     37
#define CMD_RESET_CFG_CHANGE       38
#define CMD_BURN_EEPROM            39
#define CMD_FIXED_PV_CURRENT       40
#define CMD_SELF_TEST              41
#define CMD_MASTER_RESET           42
#define CMD_SET_PV_ZERO            43
#define CMD_WRITE_PV_UNITS         44
#define CMD_TRIM_PV_DAC_ZERO       45
#define CMD_TRIM_PV_DAC_GAIN       46
#define CMD_WRITE_PV_TRANSFER_FUNC 47
#define CMD_READ_MORE_STATUS       48
#define CMD_WRITE_SENSOR_SN        49
#define CMD_WRITE_BURST_COMMAND    108
#define CMD_BURST_MODE_CONTROL     109

/*
 * Response code classifications.  These are not the actual response
 * code values, but the categories for these values.
 */
#define RSP_NO_ERROR               0    /* no command specific error  */
#define RSP_CMD_ERROR              1    /* command error              */
#define RSP_CMD_WARNING            24   /* warning                    */
#define RSP_COMM_ERROR             0x80 /* communication error        */
#define RSP_DEVICE_BUSY            32   /* the device is busy         */
#define RSP_CMD_NOTIMPLEMENTED     64   /* cmd not used by the device */
#define RSP_DR_INITIATE            33   /* delayed response initiated */
#define RSP_DR_RUNNING             34   /* delayed response running   */
#define RSP_DR_DEAD                35   /* delayed response failed    */

/*
 * Single Definition HART Error Response Codes for Byte 1 of the 2 response bytes.
 * Range is 0-7, 16, 17-23, 32-64
 */
#define RSP_SUCCESS                0
#define RSP_INVALID_SELECTION      2
#define RSP_PARAMETER_TOO_LARGE    3
#define RSP_PARAMETER_TOO_SMALL    4
#define RSP_TOO_FEW_DATA_BYTES     5
#define RSP_XMSPECIFIC             6
#define RSP_IN_WRITE_PROTECT_MODE  7
#define RSP_ACCESS_RESTRICTED      16
#define RSP_BUSY                   32
#define RSP_CMND_NOT_IMPLEMENTED   64

/*
 * Multiple Definition HART Error Response Codes.  Range 9-13, 15, 28, 29, 65-95
 */
#define RSP_PROCESS_TOO_HIGH       9
#define RSP_PROCESS_TOO_LOW        10

#define RSP_LOWER_RANGE_TOO_HIGH   9
#define RSP_LOWER_RANGE_TOO_LOW    10
#define RSP_UPPER_RANGE_TOO_HIGH   11
#define RSP_UPPER_RANGE_TOO_LOW    12
#define RSP_UPPER_LOWER_OOL        13

#define RSP_NOT_IN_PROPER_MODE     9
#define RSP_IN_MULTIDROP_MODE      11
#define RSP_INVALID_VAR_CODE       11
#define RSP_INVALID_UNITS          12

/*
 * Single Definition HART Warning Response Codes.  Range 24-27, 96-111
 */

/*
 * Multiple Definition HART Warning Response Codes.  Range 8, 14, 30-31, 112-127
 */
#define RSP_SET_TO_NEAREST         8
#define RSP_UPDATE_IN_PROGRESS     8
#define RSP_SPAN_TOO_SMALL         14
#define RSP_URV_OVER_SENSOR_LIMITS 14


/*
 * Hardware Status Byte bits.
 * These are in the second byte of the response to a command.
 * Bit pattern is as follows (Bit set for condition = TRUE):
 * 7  6  5  4  3  2  1  0
 * �  �  �  �  �  �  �  �
 * �  �  �  �  �  �  �  PV out of limits
 * �  �  �  �  �  �  Non PV out of limits
 * �  �  �  �  �  PV Analog output saturated
 * �  �  �  �  *PV Analog output fixed
 * �  �  �  More Status Available
 * �  �  *Cold Start
 * �  *Configuration Changed
 * Device Malfunction
 *
 * Bits marked with an asterisk are controlled by the application
 * layer processor and should be clear from the user.
 */
#define BIT_PV_OUT_OF_LIMITS            0x01
#define BIT_NON_PV_OUT_OF_LIMITS        0x02
#define BIT_PV_ANALOG_OUTPUT_SATURATED  0x04
#define BIT_PV_ANALOG_OUTPUT_FIXED      0x08
#define BIT_MORE_STATUS_AVAILABLE       0x10
#define BIT_COLD_START                  0x20
#define BIT_CONFIGURATION_CHANGED       0x40
#define BIT_DEVICE_MALFUNCTION          0x80

/*
 * Many Revision 4 HART instruments simply assumed Rosemount as the
 * manufacturer.
 */
#define ROSEMOUNT_MFID        38

/***************************************************************************
 * Data Structures
 ***************************************************************************/

/*
 * This structure is used by the application layer commands:
 *   happ_format_command
 *   happ_interpret_response
 * The members are the data fields read/written to field devices using
 * universal and common practice HART commands.
 *
 * To build a command that writes a value to an instrument, just store the
 * value in the structure, then call the happ_format_command to build the
 * HART message.
 *
 * When the response to any command is received, send it through the
 * happ_interpret_response function to extract the data into the structure.
 *
 * If you use other HART commands, you will need to add data locations to
 * the structure, and you will need to add cases in the happ_ commands.
 */
typedef struct device_data
{
                                   /* DESCRIPTION               READ WRITE */
                                   /* ------------------------------------ */
     BYTE      cCmdResponse;       /* status byte 1             all        */
     BYTE      cDeviceStatus;      /* status byte 2             all        */
     BYTE      cCmd;

     BYTE      cManufacturer;      /* Manufacturers ID          0/11       */
     BYTE      cDevice;            /* Mfr's device type         0/11       */
     BYTE      cPreambles;         /* preambles required        0/11       */
     BYTE      cUniversalRev;      /* HART universal cmnd rev   0/11       */
     BYTE      cSpecificRev;       /* HART device specific rev  0/11       */
     BYTE      cSoftwareRev;       /* Device software revision  0/11       */
     BYTE      cHardwareRev;       /* Device hardware revision  0/11       */
     BYTE      cFlags;             /* Standard flag byte        0/11       */
     DWORD     dwDeviceId;         /* Device Id number          0/11       */

     BYTE      cPollAddr;          /* polling addr used for short frame  6 */
     BYTE      cPollAddrNew;       /* store new poll address for cmnd 6    */
     DWORD     dwFinalAssyNum;     /* final assembly number     0/16  5/19 */

     BYTE      cPVUnits;           /* Process Variable Units    1 or 3     */
     FLOAT     rPV;                /* Process Variable          1 or 3     */
     FLOAT     rMa;                /* current loop              2 or 3     */
     FLOAT     rPVPctRange;        /*                              2       */
     BYTE      cSVUnits;           /*                              3       */
     FLOAT     rSV;                /*                              3       */
     BYTE      cTVUnits;           /*                              3       */
     FLOAT     rTV;                /*                              3       */
     BYTE      cQVUnits;           /*                              3       */
     FLOAT     rQV;

     char      szMessage[ HART_MESSAGE_LEN + 1 ];           /*  4/12  5/17 */
     char      szTag[ HART_TAG_LEN + 1 ];                   /*  4/13  5/18 */
     char      szDescript[ HART_DESCRIPT_LEN + 1 ];         /*  4/13  5/18 */
     BYTE      acDate[ 3 ];                                 /*  4/13  5/18 */

     DWORD     dwSensorSerialNum;  /* Sensor serial number      4/14    49 */
     BYTE      cLimitUnits;        /* Units for sensor limits   4/14       */
     FLOAT     rLimitUpper;        /* Upper sensor limits       4/14       */
     FLOAT     rLimitLower;        /* Lower sensor limits       4/14       */
     FLOAT     rMinimumSpan;       /* Minimum sensor span       4/14       */

     BYTE      cRangeUnits;        /* Units for sensor reading  4/15    44 */
     FLOAT     rRangeUpper;        /* 20mA point                4/15    35 */
     FLOAT     rRangeLower;        /*  4mA point                4/15    35 */
     FLOAT     rDamping;           /*  damping value in seconds 4/15    34 */
     BYTE      cTransferFunc;      /*                           4/15    47 */
     BYTE      cAlarmSelect;       /*                           4/15       */
     BYTE      cWriteProtect;      /*                           4/15       */
     BYTE      cPrivateLabel;      /* distributor code          4/15       */

     BYTE      cBurstCmd;          /* Burst command                    108 */
     BOOL      bBurst;             /* Burst on: TRUE/FALSE             109 */

     BYTE      cStatusByteCount;   /* number of bytes rcvd.       48       */
     BYTE      acMoreStatus[ 25 ]; /* additional status           48       */

} DEVDATA, *PDEVDATA;


/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

/*
 * HART Master Data Link Layer control.
 */
void hll_initialize_hart( BYTE cMaster, BYTE cRetries );
void hll_flush_hart_queue( void );

BYTE hll_set_preambles( BYTE cPreambles );
BYTE hll_get_preambles( void );

BYTE hll_set_master( BYTE cMaster );
BYTE hll_get_master( void );

BYTE hll_set_retries( BYTE cRetries );
BYTE hll_get_retries( void );

void hll_extend_timeout( BYTE cMilliSec );

FSTAT hll_send_command( PBYTE pcId, PBYTE acMessage, WORD wLength );
FSTAT hll_read_response( BYTE cId, PBYTE acResponse );

/*
 * The LINKSTATS data structure is defined in the common.h file.
 */
FSTAT hll_read_statistics( PLINKSTATS pStats );
FSTAT hll_clear_statistics( void );
FSTAT hll_get_last_burst( PBYTE acBuf );
void  hll_clear_burst_buf( void );

/*
 * HART Utilities.
 */
FSTAT hutil_header_length( BYTE cDelim, PBYTE pcLength );
FSTAT hutil_message_length( PBYTE acMessage, PBYTE pcLength );
FSTAT hutil_checksum( PBYTE acMessage, PBYTE pcCheckSum );
FSTAT hutil_response_code_type( BYTE cCode, PBYTE pcClass );

FSTAT hutil_pad_with_spaces( PBYTE pszString, WORD wLength );
FSTAT hutil_pack_ascii( PBYTE pPacked, CPSTR pszUnPacked );
FSTAT hutil_unpack_ascii( PSTR pszUnPacked, const BYTE* pPacked, BYTE cPackedLength );

FSTAT hutil_float_to_hartbuf( PBYTE pcData, FLOAT rFloat );
FSTAT hutil_hartbuf_to_float( PFLOAT prFloat, PBYTE pcData );

FSTAT hutil_long_to_hartbuf( PBYTE pcData, DWORD dwLong );
FSTAT hutil_hartbuf_to_long( PDWORD pdwLong, PBYTE pcData );

FSTAT hutil_set_nan( PFLOAT prFloat );
BOOL  hutil_is_nan( FLOAT rFloat );

/*
 * HART Application layer command processor.
 */
void  happ_format_header( PBYTE acMessage, PBYTE *ppCnt, BYTE cCmd, PDEVDATA pDevData, PBOOL pbLongFrame );
FSTAT happ_format_command( PBYTE acMessage, PBYTE pcLength, BYTE cCmd, PDEVDATA pDevData );
FSTAT happ_interpret_response( PBYTE acMessage, PDEVDATA pDevData );

/*
 * Special case (to account for a static call made non-static)
 */
void process_gap_timeout( PHART_CTRL phc, WORD wTime );

#endif /* _HARTMDLL_H */


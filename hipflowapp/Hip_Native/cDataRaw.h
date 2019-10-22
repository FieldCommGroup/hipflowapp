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
 * Description: This holds the data description for the entire device.Memory in cDatabase.cpp
 *	
 *	
 * #include "cDataRaw.h"
*/
#pragma once

#ifndef _C_DATARAW_H
#define _C_DATARAW_H
#ifdef INC_DEBUG
#pragma message("In cDataRaw.h") 
#endif

//#include "cSerialFile.h"
//#include <vector>
#include "datatypes.h"
#include "configuration_Default.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::cDataRaw.h") 
#endif

using namespace std;


#define MAX_BURSTMSGS     3
#define MAX_CMD_9_RESP    8
#define MAX_CMD9_LEGACY   4
#define CMD48_SIZE		  9

/******************* SEE IMPORTANT NOTE BELOW before you modify anything *********************/

#define NUMBER_OF_DEVICE_VARIABLES  (INIT_DEV_VAR_CNT + 3) /* 2- loop current and %range; unsupported qv!*/
#define SLOT_244_PERCENT_RANGE_IDX  (INIT_DEV_VAR_CNT + 0)
#define SLOT_245_LOOP__CURRENT_IDX  (INIT_DEV_VAR_CNT + 1)
#define SLOT_UNSUPPORTED_IDX        (INIT_DEV_VAR_CNT + 2)

extern bool Pri_MSA(void);// true if the Primary Master's More Status Available is set
extern void Pri_MSA(bool);//  @true, sets/clears the Primary's More Status available bit
extern bool Sec_MSA(void);// true if the Secondary Master's More Status Available is set
extern void Sec_MSA(bool);//  @true, sets the Secondary's More Status aVAILABLE BIT

extern bool match48( bool isPri );// returns true if exact match master's
#define     PV_cmd48IsSame  (match48(true))
#define     SV_cmd48IsSame  (match48(false))

typedef struct DevVarNV_Raw_s // NonVolatile memory-part of the device variable class
{
	uint8_t  devVarNumber;	  //cmd9element_s.devVarNumber:: in the data base	0 - (#devVars-1) ie a reference to the base value
	uint8_t  Units	;		  //cmd9element_s.dvUnits:: ie digital units
	uint8_t  classification;  //cmd9element_s.dvClass	
	uint8_t  deviceFamily;    //cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
							  //cmd9element_s.dvLimMinSpanUnits
	float    upperSensorLimit;//cmd9element_s.dvUTL
	float    lowerSensorLimit;//cmd9element_s.dvLTL
	float    minSpan;		  //cmd9element_s.dvMinSpan
	float    dampingValue;	  //cmd9element_s.dvDamping... in seconds
	uint32_t updatePeriod;	  //cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	float    high_alarm;		// added device-specific
	float    low_alarm;			// added device-specific
	uint32_t sensorSerialNumber;  //cmd9element_s.dvSerial
	uint8_t  properties;
}
/* typedef */ DevVarNV_Raw_t;

typedef struct DevVarRaw_s  // volatile memory-part of the device variable class
{
	float Value;	         // cmd9element_s.dvValue:: float ie digital value; Nott persistent; convert to xV units if required 
	//cmd9element_s.dvStatus:: 
	//    this is :1100 0000 quality, 
	//             0011 0000 lim_status, 
	//             0000 1111 df_status
	uint8_t  quality;       // ie data quality
	uint8_t  lim_status;	// ie Limit status  Not persistent; 
	uint8_t  devVar_status;     // ie device variable status
	// the data written in 79 is required to be available via 534 at some later time
	// We'll put that data here because it doesn't need to be non-volitile
	uint8_t  WrDVcmdCd;		// last Write Device Variable Command Code
	uint8_t  simulateUnits;
	float    simValue;
}
/* typedef */ DevVarRaw_t;

typedef struct burstMsg_Raw_s
{
	uint8_t	 MsgNum;	
	uint16_t CmdNum;
	uint8_t	 Cntrl;//tested
	uint32_t Period;
	uint32_t MaxPer;

	uint8_t  TrMode;
	uint8_t  TrClas;
	uint8_t  TrUnit;
	float    TrVal;

	uint8_t  Idx[MAX_CMD_9_RESP];
}
/* typedef */ burstMsg_Raw_t;

/**************** IMPORTANT **********************IMPORTANT******************************
 *
 *  struct volatileRaw_s  and  struct NONvolatileRaw_s are written and read to/from a
 *  disc file as a whole. They are constructed with Visual Studio via g++ with the
 *  attendent structure padding.  DO NOT modify the structure without destroying
 *  all file copies in the entire world.
 *  You can add stuff at the end of these structures but doing something like changing
 *  the internals of DevVarRaw_t, which is in an array in these structs, will invalidate
 *  all data files.
 *
 ****************************************************************************************/

typedef struct volatileRaw_s
{
	uint8_t responseCode;
	uint8_t deviceStatus;

	uint8_t cfgChangedZero;/* when master address is zero */
	uint8_t cfgChangedOne; /* when master address is one  */

	uint8_t extended_fld_dev_status;  // dflt 0	
	uint8_t expansion; // first cmd zero byte
/* the following are the data that the PV has that doesn't pertain to any other */	
	float    percentRange;			// PV extension: Not persistent; 
	float    loopCurrent;			// PV extension: Not persistent; 
	uint32_t dateNtimeStamp;
/* for the single totalizer in use here */
	uint32_t totTimeStamp;
	uint8_t  totRelayForced;// enum
	uint8_t  totStatus;
	uint8_t  totAddStatus;

	DevVarRaw_t devVars[NUMBER_OF_DEVICE_VARIABLES];
	uint8_t  cmd48Data[CMD48_SIZE];
}
/* typedef */ volatileRaw;

typedef struct NONvolatileRaw_s
{
	uint8_t mfrID;
	uint8_t devType;
	uint8_t reqPreambles;
	uint8_t univRev;
	uint8_t xmtrRev;
	uint8_t swRev;
	uint8_t hwRev;
	uint8_t flags;	
	uint8_t devID[3];
	/* end of HART 5 zero response */
	uint8_t myPreambles;	
	uint8_t devVarCnt;
	//   configChangeCnt	<- dynamic, endian
	//   extraDevStatus		<- dynamic
	/* end of HART 6 zero response */
	uint8_t MfgId[2];
	uint8_t PrivLabelDistr[2];
	uint8_t Profile;
	/* end of HART 7 zero response */

/* the following are variable via command or device */
	uint16_t configChangeCnt; // configuration change counter
	uint8_t  writeProtectCode;//dflt 0	
	uint8_t  loopCurrentMode; //dflt 0 (disabled) (1 is enabled) for now

	uint8_t pollAddr;// dflt 00	
	uint8_t tag[6];// dflt ff
	uint8_t message[24];// dflt ff
	uint8_t descriptor[12];// dflt ff
	uint8_t date[3]; // dflt ff ff ff
	uint8_t finalAssembly[3];// dflt 00
	uint8_t longTag	[32];// dflt 00
	
/* the following are the data that the PV has that doesn't pertain to any other */
	uint8_t  transferFunction;	
	float    upperRangeValue;		// used to convert % and loop current 
	float    lowerRangeValue;	
	uint8_t  rangeValueUnits;
	// analog_io collection
	uint8_t  alarmSelectionCode; // an enum table 6
	uint8_t  analogChannelFlags; // table 26	input or output...

	uint8_t  totFamDefRev;
	uint8_t  totInputVarCode;// not implemented
	uint8_t  totFailSafeHndling;// enum
	uint8_t  totMode;// enum
	uint8_t  totDirection;// enum
	uint8_t  totRelayInverted;
	uint8_t  totClassification;// enum
	long double totValue;


	DevVarNV_Raw_t devVars[NUMBER_OF_DEVICE_VARIABLES];

	uint8_t        devVar_Map[4]; // contains the Device Variable Number mapped to 0.PV; 1,SV; 2,TV; 3,QV	

	uint8_t        maxMsgs; //MAX_BURSTMSGS

	burstMsg_Raw_t brstMsgs[MAX_BURSTMSGS];
}
/* typedef */ NONvolatileRaw;


typedef struct temporaryRaw_s
{
	uint8_t  longTag	[32];// extracted to see if we have a match
	uint16_t tmpcongigChangeCnt;
	uint8_t  Pricmd48TmpData[CMD48_SIZE];// extracted to see if we have a match
	uint8_t  Seccmd48TmpData[CMD48_SIZE];// extracted to see if we have a match
}
/* typedef */ temporaryRaw;


extern volatileRaw    volatileData;
extern NONvolatileRaw NONvolatileData;
extern temporaryRaw   tempData;
extern burstMsg_Raw_t tempBMraw;
extern bool           cmd48Trigger;
#endif //_C_DATARAW_H
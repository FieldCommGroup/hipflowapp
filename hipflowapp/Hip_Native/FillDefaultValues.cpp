/*************************************************************************************************
 *
 * Workfile: FillDefaultValues.cpp
 * 15Aug19 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		This handles the data setting for a new installation.
 *
 */

//#include "native.h"
//
//#include "debug.h" // holds dbg logs
#include "errval.h"
//#include "hthreads.h"
//#include "sems.h"
#include "cDataRaw.h"
#include "hartPhy.h" 
#include <string.h>
//#include "configuration_Default.h"
//
//#include "hartdefs.h"
//#include "appconnector.h"
//#include "apppdu.h"
#include "PhysicalAlarms.h"
//
//extern AppConnector<AppPdu> *pAppConnector;
//extern int init_ProcessVariables(void);/*  instead of all PV.h  */
extern  uint8_t *pack(uint8_t *dest, const uint8_t *source, int nChar);// in database.cpp
//extern int initBurstStack(); // in burst.cpp

using namespace std;



////debug code
extern uint8_t debugUnitchnge;



// fill all non volatile with default values and save 'em to the open file
// close the file on exit
errVal_t FillDefaultValues()
{
	errVal_t retval = NO_ERROR;
	volatileData.expansion = 254;

	/************************* Default Values ******************************/

	NONvolatileData.mfrID = INIT_DEVTYPE_HI;
	NONvolatileData.devType = INIT_DEVTYPE_LO;
	NONvolatileData.reqPreambles = INIT_REQ_PREAMBL;

	NONvolatileData.univRev = INIT_UNIV_REV;

	NONvolatileData.xmtrRev = INIT_DEV_REV;
	NONvolatileData.swRev = INIT_SW_REV;
	NONvolatileData.hwRev = INIT_BYTE_7;  /* see INIT_HW_REV & */
	NONvolatileData.flags = INIT_FLAGS;
	NONvolatileData.devID[0] = INIT_DEV_ID0;
	NONvolatileData.devID[1] = INIT_DEV_ID1;
	NONvolatileData.devID[2] = INIT_DEV_ID2;
	/* end of HART 5 zero response */
	NONvolatileData.myPreambles = INIT_MASTER_PREAMBL;
	NONvolatileData.devVarCnt = INIT_DEV_VAR_CNT;// assume last valid index
	//   extraDevStatus		<- dynamic
	/* end of HART 6 zero response */
	NONvolatileData.MfgId[0] =
		NONvolatileData.PrivLabelDistr[0] = 0;
	NONvolatileData.MfgId[1] =
		NONvolatileData.PrivLabelDistr[1] = INIT_DEVTYPE_HI;
	NONvolatileData.Profile = 65;
	/* end of HART 7 zero response */

/* the following are variable via command or device */
	NONvolatileData.configChangeCnt = 1; // configuration change counter
	NONvolatileData.writeProtectCode = 0;
	NONvolatileData.loopCurrentMode = 0; //dflt 0 (disabled) (1 is enabled) for now

	NONvolatileData.pollAddr = 0;// dflt 00	
	memset(&(NONvolatileData.tag[0]), 0xff, 6);// dflt ff
	memset(&(NONvolatileData.message[0]), 0xff, 24);// dflt ff
	memset(&(NONvolatileData.descriptor[0]), 0xff, 12);// dflt ff
	memset(&(NONvolatileData.date[0]), 0xff, 3);// dflt ff
	memset(&(NONvolatileData.finalAssembly[0]), 0x00, 3);// dflt 00

	uint8_t buffer[64]; // plenty big
	strncpy((char*)(NONvolatileData.tag), (char*)pack(&(buffer[0]), (uint8_t*)INIT_TAG, TAG_LEN), 6);     // dflt ff
	strncpy((char*)(NONvolatileData.longTag), (char*)INIT_LONG_TAG, 32);// dflt 00


/* the following are the data that the PV has that doesn't pertain to any other */
	NONvolatileData.transferFunction = 0;
	//NONvolatileData.  rangeValueUnits;
	// analog_io collection
	NONvolatileData.alarmSelectionCode = 250; // an enum table 6
	NONvolatileData.analogChannelFlags = 0; // table 26	input channel

	//  main measurement
	//NONvolatileData.devVars[0].Units	;		  //cmd9element_s.dvUnits:: ie digital units


	NONvolatileData.rangeValueUnits = UNITS_KGpH;
	NONvolatileData.devVars[0].Units = UNITS_KGpH;
////debug code
debugUnitchnge =	NONvolatileData.devVars[0].Units;
///////end debug vcode
	NONvolatileData.devVars[1].Units = UNITS_MA;
	NONvolatileData.devVars[2].Units = UNITS_KG;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].Units = UNITS_NONE;        //5
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].Units = UNITS_PERCNT;//3
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].Units = UNITS_MA;    //4

	NONvolatileData.devVars[0].upperSensorLimit = 24000.0;//cmd9element_s.dvUTL
	NONvolatileData.devVars[0].lowerSensorLimit = 0.0;//cmd9element_s.dvLTL

	NONvolatileData.devVars[0].minSpan = 200.0;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[1].upperSensorLimit = 25.0;//cmd9element_s.dvUTL
	NONvolatileData.devVars[1].lowerSensorLimit = -1.0;//cmd9element_s.dvLTL

	NONvolatileData.devVars[1].minSpan = 2.0;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[2].upperSensorLimit = 1.8E308;//cmd9element_s.dvUTL
	NONvolatileData.devVars[2].lowerSensorLimit = 0.0;//cmd9element_s.dvLTL

	NONvolatileData.devVars[2].minSpan = 1.8E308;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].upperSensorLimit = HART_NAN;//cmd9element_s.dvUTL
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].lowerSensorLimit = HART_NAN;//cmd9element_s.dvLTL

	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].minSpan = HART_NAN;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].upperSensorLimit = 100.0;//cmd9element_s.dvUTL
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].lowerSensorLimit = 0.0;//cmd9element_s.dvLTL

	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].minSpan = 1.0;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].upperSensorLimit = 20.0;//cmd9element_s.dvUTL
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].lowerSensorLimit = 4.0;//cmd9element_s.dvLTL

	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].minSpan = 0.5;//cmd9element_s.dvMinSpan

	NONvolatileData.devVars[0].sensorSerialNumber = 0x00654321;  //cmd9element_s.dvSerial
	NONvolatileData.devVars[1].sensorSerialNumber = 0x00456789;  //cmd9element_s.dvSerial
	NONvolatileData.devVars[2].sensorSerialNumber = 0x00000000;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].sensorSerialNumber = 0x00000000;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].sensorSerialNumber = 0x00000000;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].sensorSerialNumber = 0x00000000;

	NONvolatileData.upperRangeValue = 24000.0;		// used to convert % and loop current 
	NONvolatileData.lowerRangeValue = 0.0;


	NONvolatileData.totFamDefRev = 11;
	NONvolatileData.totInputVarCode = HART_NOT_USED;// none assigned, not implemented
	NONvolatileData.totFailSafeHndling = 2;// use last value
	NONvolatileData.totMode = 0;// arithmetic total
	NONvolatileData.totDirection = 0;// add to toal
	NONvolatileData.totRelayInverted = 0;// false - not inverted
	NONvolatileData.totClassification = 0;// Not classified..as per WAP 4/5/19
	NONvolatileData.totValue = 0.0;// will be updated from the file


	volatileData.totTimeStamp = 0;
	volatileData.totRelayForced = 0;// enum
	volatileData.totStatus = 0;
	volatileData.totAddStatus = 0;

	NONvolatileData.devVars[0].classification = NO_CLASS;  //cmd9element_s.dvClass	
	NONvolatileData.devVars[1].classification = NO_CLASS;
	NONvolatileData.devVars[2].classification = NO_CLASS;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].classification = NO_CLASS;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].classification = NO_CLASS;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].classification = NO_CLASS;

	NONvolatileData.devVars[0].deviceFamily = NOT_USED;//cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
	NONvolatileData.devVars[1].deviceFamily = NOT_USED;
	NONvolatileData.devVars[2].deviceFamily = NOT_USED;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].deviceFamily = NOT_USED;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].deviceFamily = NOT_USED;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].deviceFamily = NOT_USED;

	NONvolatileData.devVars[0].dampingValue = HART_NAN;//cmd9element_s.dvDamping... in seconds
	NONvolatileData.devVars[1].dampingValue = HART_NAN;
	NONvolatileData.devVars[2].dampingValue = HART_NAN;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].dampingValue = HART_NAN;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].dampingValue = HART_NAN;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].dampingValue = HART_NAN;

	NONvolatileData.devVars[0].updatePeriod = PHY_UPDATE_PERIOD;//cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	NONvolatileData.devVars[1].updatePeriod = PHY_UPDATE_PERIOD;
	NONvolatileData.devVars[2].updatePeriod = PHY_UPDATE_PERIOD;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].updatePeriod = 0;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].updatePeriod = PHY_UPDATE_PERIOD;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].updatePeriod = PHY_UPDATE_PERIOD;

	NONvolatileData.devVars[0].properties = 0;// bit-enum: 0x01-NOT calculated in device; 0x80-Value is simulated (never set when 0x01 set)
	NONvolatileData.devVars[1].properties = 0;
	NONvolatileData.devVars[2].properties = 0;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].properties = 0;
	NONvolatileData.devVars[SLOT_244_PERCENT_RANGE_IDX].properties = 0;
	NONvolatileData.devVars[SLOT_245_LOOP__CURRENT_IDX].properties = 0;

	NONvolatileData.devVars[0].high_alarm = 200000;
	NONvolatileData.devVars[1].high_alarm = 60;
	NONvolatileData.devVars[2].high_alarm = HART_NAN;// no totalizer alarms
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].high_alarm = HART_NAN;
	NONvolatileData.devVars[0].low_alarm = 2400;
	NONvolatileData.devVars[1].low_alarm = 30;
	NONvolatileData.devVars[2].low_alarm = HART_NAN;
	NONvolatileData.devVars[SLOT_UNSUPPORTED_IDX].low_alarm = HART_NAN;
	
	volatileData.devVars[0].WrDVcmdCd = 0; // normal (not simulated)
	volatileData.devVars[1].WrDVcmdCd = 0;
	volatileData.devVars[2].WrDVcmdCd = 0;
	volatileData.devVars[SLOT_UNSUPPORTED_IDX].WrDVcmdCd = 0;

	volatileData.devVars[SLOT_UNSUPPORTED_IDX].Value = HART_NAN;

	volatileData.devVars[0].devVar_status = (pdq_Good | ls_Not);
	volatileData.devVars[1].devVar_status = (pdq_Good | ls_Not);
	volatileData.devVars[2].devVar_status = (pdq_Good | ls_Not);
	volatileData.devVars[SLOT_UNSUPPORTED_IDX].devVar_status = (pdq_Bad  | ls_Const);

	//=====================================================================================	

	NONvolatileData.devVar_Map[0] = 0;//PV
	NONvolatileData.devVar_Map[1] = 1;//SV
	NONvolatileData.devVar_Map[2] = 2;//TV
	NONvolatileData.devVar_Map[3] = 5;// QV - we are required to support it, set to unsupported
	//=====================================================================================	
	NONvolatileData.brstMsgs[0].MsgNum = 0;//	uint8_t  
	NONvolatileData.brstMsgs[0].CmdNum = 9;//	uint16_t 
	NONvolatileData.brstMsgs[0].Cntrl  = INIT_BURST_ENABLE;//  uint8_t -- 0 ==> Disabled, 4==>burst on HART-IP
	NONvolatileData.brstMsgs[0].Period = 1600;// uint32_t -- 1/32nd mS 1600 = 50mS
	NONvolatileData.brstMsgs[0].MaxPer = 3200;// uint32_t -- .1 sec

	NONvolatileData.brstMsgs[0].TrMode = 0;//uint8_t  -- 0 = continuous(1=Window; 4=ValueChange)
	NONvolatileData.brstMsgs[0].TrClas = HART_NOT_CLASFD;//uint8_t  
	NONvolatileData.brstMsgs[0].TrUnit = NOT_USED;//uint8_t
	NONvolatileData.brstMsgs[0].TrVal  = HART_NAN;//float    

	NONvolatileData.brstMsgs[0].Idx[0] = 0;//uint8_t   thru 8];
	NONvolatileData.brstMsgs[0].Idx[1] = 1;
	NONvolatileData.brstMsgs[0].Idx[2] = 2;
	NONvolatileData.brstMsgs[0].Idx[3] = NOT_USED;
	NONvolatileData.brstMsgs[0].Idx[4] = NOT_USED;
	NONvolatileData.brstMsgs[0].Idx[5] = NOT_USED;
	NONvolatileData.brstMsgs[0].Idx[6] = NOT_USED;
	NONvolatileData.brstMsgs[0].Idx[7] = NOT_USED;
	//	//  //  //  //  //  //  //  // ///  //  //  //
	NONvolatileData.brstMsgs[1].MsgNum = 1;//	uint8_t  
	NONvolatileData.brstMsgs[1].CmdNum = 48;//	uint16_t 
	NONvolatileData.brstMsgs[1].Cntrl  = INIT_BURST_ENABLE;//  uint8_t
	NONvolatileData.brstMsgs[1].Period = 160000;//.uint32_t - 5 seconds
	NONvolatileData.brstMsgs[1].MaxPer = 160000;//uint32_t 

	NONvolatileData.brstMsgs[1].TrMode = 0;//uint8_t  -- 0 = continuous(1=Window; 4=ValueChange)
	NONvolatileData.brstMsgs[1].TrClas = HART_NOT_CLASFD;//uint8_t  
	NONvolatileData.brstMsgs[1].TrUnit = NOT_USED;//uint8_t
	NONvolatileData.brstMsgs[1].TrVal = HART_NAN;//float    

	NONvolatileData.brstMsgs[1].Idx[0] = NOT_USED;//uint8_t   thru 8];
	NONvolatileData.brstMsgs[1].Idx[1] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[2] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[3] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[4] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[5] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[6] = NOT_USED;
	NONvolatileData.brstMsgs[1].Idx[7] = NOT_USED;
	//	//  //  //  //  //  //  //  // ///  //  //  //
	NONvolatileData.brstMsgs[2].MsgNum = 2;//	uint8_t  
	NONvolatileData.brstMsgs[2].CmdNum = 38;//	uint16_t 
	NONvolatileData.brstMsgs[2].Cntrl = INIT_BURST_ENABLE;//  uint8_t
	NONvolatileData.brstMsgs[2].Period = 38400000;//.uint32_t 20 minutes (shouldn't be used here)
	NONvolatileData.brstMsgs[2].MaxPer = 115200000;//uint32_t 1 hour

	NONvolatileData.brstMsgs[2].TrMode = 4;//uint8_t  -- 0 = continuous(1=Window; 4=ValueChange)
	NONvolatileData.brstMsgs[2].TrClas = HART_NOT_CLASFD;//uint8_t  
	NONvolatileData.brstMsgs[2].TrUnit = NOT_USED;//uint8_t
	NONvolatileData.brstMsgs[2].TrVal = HART_NAN;//float    

	NONvolatileData.brstMsgs[2].Idx[0] = NOT_USED;//uint8_t   thru 8];
	NONvolatileData.brstMsgs[2].Idx[1] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[2] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[3] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[4] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[5] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[6] = NOT_USED;
	NONvolatileData.brstMsgs[2].Idx[7] = NOT_USED;

	/* * * * * * * * End of Default Values * * * * * * * * * * * * * * */

	return retval;
}
/*************************************************************************************************
 *
 * Workfile: cDatabase.cpp
 * 04Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		This all of the dataitems.
 *
 */
 
 #include <string.h> /* for memcpy) */
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <map>
#include "cDatabase.h"
#include "cDataRaw.h"
#include "hartPhy.h"
#include "InsertExtract.h"

/******************  statics ****************************/
const float    constant_zero = 0.0f;
const float    constant_nan  = NaN_value;// this no-workee
const uint8_t  constant_ui8 = 0;
const uint16_t constant_ui16= 0;
const uint32_t constant_ui32 = 0;
const uint8_t  constant_NotUsed =250;

/******************  globals ****************************/
volatileRaw    volatileData;  // the actual memory
NONvolatileRaw NONvolatileData;// the actual memory
burstMsg_Raw_t tempBMraw;      // memory for temp
temporaryRaw   tempData;      // the actual memory
bool           cmd48Trigger = false;// trigger burst on cmd 48 change

unsigned char nanarray[4] = { 0x7F, 0xA0, 0x00, 0x00 };//ARM bigendian..always bigendian going on the wire
const float hartBENaN = *((float *) nanarray);

unsigned char nanLEarray[4] = { 0x00,  0x00, 0xA0, 0x7F };//little endian..always bigendian going on the wire
const float hartLENaN = *((float *) nanLEarray);

/* constant dataItems */
uint8_t notUsed = HART_NOT_USED;
dataItem constUnused(false, false, ht_int8, &(notUsed));
uint8_t constU8_zero= 0x00;
dataItem constU8zero(false, false, ht_int8, &(constU8_zero));


bool    configChangedBit_Pri = false;	
bool	configChangedBit_Sec = false;

bool    writeNeeded = false; // set when config changed in a command

static bool    moreStatusAvail_Pri  = false;
static bool    moreStatusAvail_Sec  = false;

// true if the Primary Master's More Status Available is set
bool Pri_MSA(void) { return moreStatusAvail_Pri; };
//  @true, sets/clears the Primary's More Status aVAILABLE BIT
void Pri_MSA(bool b) {
	moreStatusAvail_Pri = b; };
// true if the Secondary Master's More Status Available is set
bool Sec_MSA(void) { return moreStatusAvail_Sec; };
//  @true, sets the Secondary's More Status available bit
void Sec_MSA(bool b) {
	moreStatusAvail_Sec = b;
};

bool match48(bool isPri)
{
	uint8_t *pMasterData = (isPri)? tempData.Pricmd48TmpData : tempData.Seccmd48TmpData;

	return  (volatileData.cmd48Data[0] == pMasterData[0]) && // Device Specific
		    (volatileData.cmd48Data[6] == pMasterData[6]) && // EFDS
		    (volatileData.cmd48Data[8] == pMasterData[8]) ;  // Simulation active
	//for (int k = 0; k < CMD48_SIZE; k++)
	//{//     rcv'd data           actual data
	//	if (pCmdDataInUse[k] != volatileData.cmd48Data[k])
	//	{
	//		match = false;
	//		break; // outa for loop
	//	}
	//}
}
//dataItem(bool isVolatile, bool isConfig, hartTypes_t type, void *data); ptr into a data struct

//              bool isVol,isConfig, hartTypes_t,      void *data,       int sz=1 
dataItem responseCode(true,  false, ht_int8, &(volatileData.responseCode) );// do not store
dataItem deviceStatus(true,  false, ht_int8, &(volatileData.deviceStatus) );// do not store

dataItem  extended_fld_dev_status(true, false, ht_int8, &(volatileData.extended_fld_dev_status) );// uint8_t do not save;;  // dflt 0	

/* these are constant for a given device */
dataItem expansion(true,  false, ht_int8, &(volatileData.expansion) );// do not store
dataItem mfrID    (false, false, ht_int8, &(NONvolatileData.mfrID) );// uint8_t-save
dataItem devType  (false, false, ht_int8, &(NONvolatileData.devType) );// uint8_t-save;
dataItem reqPreambles(false,true,ht_int8,&(NONvolatileData.reqPreambles) );// uint8_t-save;
dataItem univRev  (false, false, ht_int8, &(NONvolatileData.univRev) );// uint8_t-save;
dataItem xmtrRev  (false, false, ht_int8, &(NONvolatileData.xmtrRev) );// uint8_t-save;
dataItem swRev    (false, false, ht_int8, &(NONvolatileData.swRev) );// uint8_t-save;
dataItem hwRev    (false, false, ht_int8, &(NONvolatileData.hwRev) );// uint8_t-save;// has to have physical signalling code in it
dataItem flags    (false, false, ht_int8, &(NONvolatileData.flags) );// uint8_t-save;
dataItem devID    (false, false, ht_int8, &(NONvolatileData.devID), 3 );// uint8_t-save;
	/* end of HART 5 zero response */
dataItem myPreambles(false, true, ht_int8, &(NONvolatileData.myPreambles) );// uint8_t-save	
dataItem devVarCnt(false, false, ht_int8, &(NONvolatileData.devVarCnt) );// uint8_t-save
dataItem  configChangeCnt(false, false, ht_int16, &(NONvolatileData.configChangeCnt) ); //uint16_t configuration change counter	
dataItem				tempconfigChangeCnt(true, false, ht_int16, &(tempData.tmpcongigChangeCnt) );
	/* end of HART 6 zero response */
dataItem MfgId(false, false, ht_int8, &(NONvolatileData.MfgId),2 );// uint8_t-save;;
dataItem PrivLabelDistr(false, false, ht_int8, &(NONvolatileData.PrivLabelDistr),2 );// uint8_t-save;;
dataItem Profile(false, false, ht_int8, &(NONvolatileData.Profile) );// uint8_t-save;;
	/* end of HART 7 zero response */

/* the following are variable via command or device */
dataItem  writeProtectCode(false, false, ht_int8, &(NONvolatileData.writeProtectCode) );// uint8_t-save;;//dflt 0	
dataItem  loopCurrentMode(false, true, ht_int8, &(NONvolatileData.loopCurrentMode) );// uint8_t-save;; //dflt 0 (disabled) (1 is enabled) for now

dataItem pollAddr(false, true, ht_int8, &(NONvolatileData.pollAddr) );// uint8_t-save;;// dflt 00	
dataItem tag(false, true, ht_packed, &(NONvolatileData.tag),6 );// uint8_t-save;;// dflt ff
dataItem message(false, true, ht_packed, &(NONvolatileData.message),24 );// uint8_t-save;;// dflt ff
dataItem descriptor(false, true, ht_packed, &(NONvolatileData.descriptor),12 );// uint8_t-save;;// dflt ff
dataItem date(false, true, ht_int8, &(NONvolatileData.date),3 );// uint8_t-save;; // dflt ff ff ff
dataItem finalAssembly(false, true, ht_int8, &(NONvolatileData.finalAssembly),3 );// uint8_t-save;;// dflt 00
dataItem longTag(false, true, ht_int8, &(NONvolatileData.longTag),32 );// uint8_t-save;;// dflt 00
dataItem			templongTag(true, false, ht_int8, &(tempData.longTag),32 );


deviceVar devVarArray[NUMBER_OF_DEVICE_VARIABLES] = { 0, 1, 2    ,244,245,255 };// these are mapped to orig: 246, 247, 248, 249 };

dataItem PVidx(false, true, ht_int8, &(NONvolatileData.devVar_Map[0]));
dataItem SVidx(false, true, ht_int8, &(NONvolatileData.devVar_Map[1]));
dataItem TVidx(false, true, ht_int8, &(NONvolatileData.devVar_Map[2]));

//PVextension
dataItem percentRange(true,  false, ht_float, &(volatileData.percentRange) );
dataItem loopCurrent (true,  false, ht_float, &(volatileData.loopCurrent ) );		/* Not persistent; */

dataItem transferFunction(false, true, ht_int8, &(NONvolatileData.transferFunction) );
dataItem upperRangeValue (false, true, ht_float,&(NONvolatileData.upperRangeValue) );/* used to convert % and loop current */
dataItem lowerRangeValue (false, true, ht_float,&(NONvolatileData.lowerRangeValue) );	
dataItem rangeValueUnits (false, true, ht_int8, &(NONvolatileData.rangeValueUnits) );
// analog_io collection
dataItem alarmSelectionCode(false, true, ht_int8, &(NONvolatileData.alarmSelectionCode) );	
dataItem analogChannelFlags(false, true, ht_int8, &(NONvolatileData.analogChannelFlags) );// table 26	

//PV,SV,TV,QV all have deviceVariables number...PV just has added stuff

dataItem dateTimeStamp(true, false, ht_int32, &(volatileData.dateNtimeStamp));

// totalizer data - (bool Volitile, bool isConfig, hartTypes_t type, void *data, int sz=1 )
dataItem totStatus(true, false, ht_int8, &(volatileData.totStatus));
dataItem totAddStatus(true,false, ht_int8, &(volatileData.totAddStatus));
dataItem totFamDefRev(false,false, ht_int8, &(NONvolatileData.totFamDefRev));
// mapped input is not supported here...dataItem totInputVarCode(false,false, ht_int8, &(NONvolatileData.totInputVarCode));
dataItem totFalSafHndling(false,true, ht_int8, &(NONvolatileData.totFailSafeHndling));
dataItem totMode(false,true, ht_int8, &(NONvolatileData.totMode));
dataItem totDirection(false,true, ht_int8, &(NONvolatileData.totDirection));
dataItem totRelayForced(true,false, ht_int8, &(volatileData.totRelayForced));
dataItem totRelayInverted(false,true, ht_int8, &(NONvolatileData.totRelayInverted));
dataItem totClassification(false,false, ht_int8, &(NONvolatileData.totClassification));
// use the device var Value.....dataItem totValueFlt(false,false, ht_float, &(NONvolatileData.totValue));		//update both when setting
dataItem totValue(false, false, ht_double, &(NONvolatileData.totValue));		//update both when setting
dataItem totTimeStamp(true,false, ht_int32, &(volatileData.totTimeStamp));

dataItem maxBurstMsgCnt(false, false, ht_int8, &(NONvolatileData.maxMsgs));


			burstMessage burstMsgArray[MAX_BURSTMSGS]{ 0,1,2 };
			burstMessage tempBurstMsg(0xff);

//static - gives a pointer from the number OR null on not supported
deviceVar* deviceVar::devVarPtr(uint8_t slotNumber)
{
	switch (slotNumber)
	{
	case 0:
	case 1:
	case 2:
	{
		return &(devVarArray[slotNumber]);
	}
	break;
	case 3:// the not-supported values
	{
		return &(devVarArray[5]);
	}
	break;
	case 244:
	{//Percent Range (Unit Code is "percent"; Device Variable Classification is "Device Variable Not Classified")

		return &(devVarArray[3]);
	}
	break;
	// loop current is not supported as device variable 4     case 4:
	case 245:
	{//Loop Current (Unit Code is "milliamperes"9; Device Variable Classification is "Device Variable Not Classified")

		return &(devVarArray[4]);
	}
	break;
	case 246:
	{//Primary Variable
		return &devVar_PV;
	}
	break;
	case 247:
	{//Secondary Variable
		return &devVar_SV;
	}
	break;
	case 248:
	{//Tertiary Variable
		return &devVar_TV; // (devVarArray[NONvolatileData.devVar_Map[2]])
	}
	break;
	case 249:
	{//Quaternary Variable - not supported
		return &devVar_QV; ///  burst requires us to support 4
		//                 // (devVarArray[NONvolatileData.devVar_Map[3]])
		//                                 |    5                      |
	}
	break;
	default:
	{// invalid device variable
		return NULL;
	}
	break;
	}
}

uint8_t deviceVar::DevVar2Index(uint8_t slotNumber)
{
	switch (slotNumber)
	{
	case 0:
	case 1:
	case 2:
	{
		return slotNumber;
	}
	break;
	case 3:// the not-supported values
	{
		return 5;
	}
	break;
	case 244:
	{//Percent Range (Unit Code is "percent"; Device Variable Classification is "Device Variable Not Classified")
		return 3;
	}
	break;
	case 4:
	case 245:
	{//Loop Current (Unit Code is "milliamperes"9; Device Variable Classification is "Device Variable Not Classified")
		return 4;
	}
	break;
	case 246:
	{//Primary Variable
		return 0;// we're not mapping 'em at this point
	}
	break;
	case 247:
	{//Secondary Variable
		return 1;// we're not mapping 'em at this point
	}
	break;
	case 248:
	{//Tertiary Variable
		return 2;// we're not mapping 'em at this point
	}
	break;
	case 249:
	{//Quaternary Variable - not supported
		return 5;// we're not mapping 'em at this point
		//                                 |    5                      |
	}
	break;
	default:
	{// invalid device variable
		return 0xff;
	}
	break;
	}
}
/* made global so any identity command can use it */
uint8_t insert_Ident(uint8_t &ByteCnt, uint8_t *pData) 
{
	uint8_t ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( (ret = expansion.      insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = mfrID.          insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = devType.        insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = reqPreambles.   insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = univRev.        insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = xmtrRev.        insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = /*swRev.*/      insert( SOFTWAREVERSION, &pData, ByteCnt)) ) break; // was    insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = hwRev.          insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = flags.          insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = devID.          insertSelf( &pData, &ByteCnt )  ) ) break;
#if defined(_IS_HART7) || defined(_IS_HART6)
		if ( (ret = myPreambles.    insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = devVarCnt.      insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = configChangeCnt.insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = extended_fld_dev_status.insertSelf( &pData, &ByteCnt )  ) ) break;
  #if defined(_IS_HART7) 
		if ( (ret = MfgId.          insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = PrivLabelDistr. insertSelf( &pData, &ByteCnt )  ) ) break;
		if ( (ret = Profile.        insertSelf( &pData, &ByteCnt )  ) ) break;
  #endif
#endif  
	}
	while(false);// execute once

	if ( ret )
	{
		fprintf(stderr,"Data insertion error cmd_00. ret = %d.\n", ret);
		ret     = RC_DEV_SPEC;//Device Specific Command error
	}

    return ret;
}

// pack nChar characters from source into dest. Assume dest is big enough to hold it
uint8_t * pack(uint8_t *dest, const uint8_t *source, int nChar)
{
	const uint8_t *s = source;
	uint8_t 
		*d = dest,
		 i;

	uint32_t p;
	for (; nChar > 0; nChar -= 4) {
		// fill p with the 4 packed ascii char (24bits into p)
		for (p = i = 0; i < 4 && i < nChar; i++) 
		{
			p <<= 6;
			p |= toupper(*s) & 0x3f;
			s++;
		}
		// put the three bytes into the packed ascii destination
		d += 2;
		for (i = 0; i < 3; i++) {
			*d-- = (uint8_t)(p & 0xff);
			p >>= 8;
		}
		d += 4;
	}
	return dest;
}

// we don't currently need unpack

//===================================================================================================


deviceVar::deviceVar(int devVarNum)
{
	dVN = DevVar2Index( devVarNum & 0xff );
printf("DevVar #%d instantiated as %d",devVarNum,dVN);
	if (dVN < 3)// a device deviceVariable  0,1,2
	{
printf(" as an actual device variable.\n");
		ctor_Device();
	}
	else
	if (dVN == 5 || dVN == 0xff)
	{
printf(" as an unsupported device variable.\n");
		ctor_Unsupported();
	}
	else // a standard device variable..has to be 3,4 of -1 error
	{
printf(" as a standard device variable.\n");
		ctor_Stnd();
	}
}

void deviceVar::ctor_Device()
{
	Value.set(true, false, ht_float, &(volatileData.devVars[dVN].Value));
	//cmd9element_s.dvStatus:: this is :1100 0000 quality, 0011 0000 lim_status, 0000 1111 df_status
	quality.set(true, false, ht_int8, &(volatileData.devVars[dVN].quality));      // ie data quality
	lim_status.set(true, false, ht_int8, &(volatileData.devVars[dVN].lim_status));// ie Limit status  Not persistent; 
	devVar_status.set(true, false, ht_int8, &(volatileData.devVars[dVN].devVar_status));  // ie used as Device Variable Status in cmd 9...device family status
	lastDVcmdCode.set(true, false, ht_int8, &(volatileData.devVars[dVN].WrDVcmdCd));
	simulateUnits.set(true, false, ht_int8, &(volatileData.devVars[dVN].simulateUnits));
	simulateValue.set(true, false, ht_float, &(volatileData.devVars[dVN].simValue));

	// data >>>> NonVolatile
	Units.set(false, true, ht_int8, &(NONvolatileData.devVars[dVN].Units));//cmd9element_s.dvUnits:: ie digital units
	classification.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].classification));//cmd9element_s.dvClass	
	deviceFamily.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].deviceFamily)); //cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
					  //cmd9element_s.dvLimMinSpanUnits
	upperSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].upperSensorLimit));//cmd9element_s.dvUTL
	lowerSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].lowerSensorLimit));//cmd9element_s.dvLTL
	minSpan.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].minSpan));//cmd9element_s.dvMinSpan
	dampingValue.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].dampingValue)); //cmd9element_s.dvDamping... in seconds
	updatePeriod.set(false, true, ht_int32, &(NONvolatileData.devVars[dVN].updatePeriod));//cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	sensorSerialNumber.set(false, false, ht_int24, &(NONvolatileData.devVars[dVN].sensorSerialNumber));//cmd9element_s.dvSerial
	properties.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].properties));

	switch (dVN)// set constant unit selection; UnitSet isa UnitList
	{
		case 0:
		{
			DevVar0UnitFill();
		}
		break;
		case 1:
		{
			DevVar1UnitFill();
		}
		break;
		case 2:
		{
			DevVar2UnitFill();
		}
		break;
		//case 3:
		//{
		//	DevVar3UnitFill();
		//}
		//break;
		//case 244:
		//{
		//	DevVarPRUnitFill();// %Range
		//}
		//break;
		//case 245:
		//{
		//	DevVarLCUnitFill();
		//}
		//break;
		default:
		{
			DevVarBadUnitFill();
		}
		break;
	}// ensswitch
}

void deviceVar::ctor_Stnd()
{	
	uint8_t idx = dVN;// was  0xff;
	//  the other 4 are redirected to device devVarsin the command handler via devVarPtr
	if (dVN == 3)//244 -percent range,
	{
		Value.set(true, false, ht_float, &(volatileData.percentRange));
		idx = SLOT_244_PERCENT_RANGE_IDX;
		DevVarPRUnitFill();
	}
	else
	if (dVN == 4)//245 - loop current,
	{
		Value.set(true, false, ht_float, &(volatileData.loopCurrent));		// constant NaN
		idx = SLOT_245_LOOP__CURRENT_IDX;
		DevVarLCUnitFill();
	}
	else
	{// do nothing
		return;
	}

	//cmd9element_s.dvStatus:: this is :1100 0000 quality, 0011 0000 lim_status, 0000 1111 df_status
	quality.set(true, false, ht_int8, &(volatileData.devVars[idx].quality));      // ie data quality
	lim_status.set(true, false, ht_int8, &(volatileData.devVars[idx].lim_status));// ie Limit status  Not persistent; 
	devVar_status.set(true, false, ht_int8, &(volatileData.devVars[idx].devVar_status));  // ie used as Device Variable Status in cmd 9...device family status
	lastDVcmdCode.set(true, false, ht_int8, &(volatileData.devVars[idx].WrDVcmdCd));
	simulateUnits.set(true, false, ht_int8, &(volatileData.devVars[idx].simulateUnits));
	simulateValue.set(true, false, ht_float, &(volatileData.devVars[idx].simValue));

	// data >>>> NonVolatile
	Units.set(false, true, ht_int8, &(NONvolatileData.devVars[idx].Units));//cmd9element_s.dvUnits:: ie digital units
	classification.set(false, false, ht_int8, &(NONvolatileData.devVars[idx].classification));//cmd9element_s.dvClass	
	deviceFamily.set(false, false, ht_int8, &(NONvolatileData.devVars[idx].deviceFamily)); //cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
					  //cmd9element_s.dvLimMinSpanUnits
	upperSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[idx].upperSensorLimit));//cmd9element_s.dvUTL
	lowerSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[idx].lowerSensorLimit));//cmd9element_s.dvLTL
	minSpan.set(false, true, ht_float, &(NONvolatileData.devVars[idx].minSpan));//cmd9element_s.dvMinSpan
	dampingValue.set(false, true, ht_float, &(NONvolatileData.devVars[idx].dampingValue)); //cmd9element_s.dvDamping... in seconds
	updatePeriod.set(false, true, ht_int32, &(NONvolatileData.devVars[idx].updatePeriod));//cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	sensorSerialNumber.set(false, false, ht_int24, &(NONvolatileData.devVars[idx].sensorSerialNumber));//cmd9element_s.dvSerial
	properties.set(false, false, ht_int8, &(NONvolatileData.devVars[idx].properties));
}


// devVarUnSup
void deviceVar::ctor_Unsupported()
{
	if (dVN == 0xff)  dVN = 5;
	Value.set(true, false, ht_float, &(volatileData.devVars[dVN].Value));
	//cmd9element_s.dvStatus:: this is :1100 0000 quality, 0011 0000 lim_status, 0000 1111 df_status
	quality.set(true, false, ht_int8, &(volatileData.devVars[dVN].quality));      // ie data quality
	lim_status.set(true, false, ht_int8, &(volatileData.devVars[dVN].lim_status));// ie Limit status  Not persistent; 
	devVar_status.set(true, false, ht_int8, &(volatileData.devVars[dVN].devVar_status));  // ie used as Device Variable Status in cmd 9...device family status
	lastDVcmdCode.set(true, false, ht_int8, &(volatileData.devVars[dVN].WrDVcmdCd));
	simulateUnits.set(true, false, ht_int8, &(volatileData.devVars[dVN].simulateUnits));
	simulateValue.set(true, false, ht_float, &(volatileData.devVars[dVN].simValue));

	// data >>>> NonVolatile
	Units.set(false, true, ht_int8, &(NONvolatileData.devVars[dVN].Units));//cmd9element_s.dvUnits:: ie digital units
	classification.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].classification));//cmd9element_s.dvClass	
	deviceFamily.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].deviceFamily)); //cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
					  //cmd9element_s.dvLimMinSpanUnits
	upperSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].upperSensorLimit));//cmd9element_s.dvUTL
	lowerSensorLimit.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].lowerSensorLimit));//cmd9element_s.dvLTL
	minSpan.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].minSpan));//cmd9element_s.dvMinSpan
	dampingValue.set(false, true, ht_float, &(NONvolatileData.devVars[dVN].dampingValue)); //cmd9element_s.dvDamping... in seconds
	updatePeriod.set(false, true, ht_int32, &(NONvolatileData.devVars[dVN].updatePeriod));//cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	sensorSerialNumber.set(false, false, ht_int24, &(NONvolatileData.devVars[dVN].sensorSerialNumber));//cmd9element_s.dvSerial
	properties.set(false, false, ht_int8, &(NONvolatileData.devVars[dVN].properties));

	DevVarBadUnitFill();
}




void deviceVarUnits::DevVar0UnitFill()		// PV
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_KGpH, UNITSNAME_KGpH);
	wrkUnit.SetCvt(0.0, UNITSMULT_KGpH, 0.0);
	UnitSet[UNITS_KGpH] = wrkUnit;

	wrkUnit.NameUnit(UNITS_KGpM, UNITSNAME_KGpM);
	wrkUnit.SetCvt(0.0, UNITSMUTL_KGpM, 0.0);
	UnitSet[UNITS_KGpM] = wrkUnit;

	wrkUnit.NameUnit(UNITS_LBpM, UNITSNAME_LBpM);
	wrkUnit.SetCvt(0.0, UNITSMUTL_LBpM, 0.0);
	UnitSet[UNITS_LBpM] = wrkUnit;

	wrkUnit.NameUnit(UNITS_LBpH, UNITSNAME_LBpH);
	wrkUnit.SetCvt(0.0, UNITSMUTL_LBpH, 0.0);
	UnitSet[UNITS_LBpH] = wrkUnit;

	wrkUnit.NameUnit(UNITS_TpM, UNITSNAME_TpM);
	wrkUnit.SetCvt(0.0, UNITSMUTL_TpM, 0.0);
	UnitSet[UNITS_TpM] = wrkUnit;

	wrkUnit.NameUnit(UNITS_TpH, UNITSNAME_TpH);
	wrkUnit.SetCvt(0.0, UNITSMUTL_TpH, 0.0);
	UnitSet[UNITS_TpH] = wrkUnit;
}

void deviceVarUnits::DevVar1UnitFill()//	SV
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_MA, UNITSNAME_MA);
	wrkUnit.SetCvt(0.0, UNITSMULT_MA, 0.0);
	UnitSet[UNITS_MA] = wrkUnit;
}

void deviceVarUnits::DevVar2UnitFill()	// TV
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_KG, UNITSNAME_KG);
	wrkUnit.SetCvt(0.0, UNITSMULT_KG, 0.0);
	UnitSet[UNITS_KG] = wrkUnit;

	wrkUnit.NameUnit(UNITS_POUND, UNITSNAME_POUND);
	wrkUnit.SetCvt(0.0, UNITSMULT_POUND, 0.0);
	UnitSet[UNITS_POUND] = wrkUnit;

	wrkUnit.NameUnit(UNITS_TON, UNITSNAME_TON);
	wrkUnit.SetCvt(0.0, UNITSMULT_TON, 0.0);
	UnitSet[UNITS_TON] = wrkUnit;
}
void deviceVarUnits::DevVar3UnitFill()	// QV
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_NONE, UNITSNAME_NONE);
	wrkUnit.SetCvt(0.0, UNITSMUTL_NONE, 0.0);
	UnitSet[UNITS_NONE] = wrkUnit;
}
void deviceVarUnits::DevVarPRUnitFill()// 244  %Range
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_PERCNT, UNITSNAME_PERCNT);
	wrkUnit.SetCvt(0.0, UNITSMULTI_PERCNT, 0.0);
	UnitSet[UNITS_PERCNT] = wrkUnit;
}
void deviceVarUnits::DevVarLCUnitFill()// 245  loop current
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_MA, UNITSNAME_MA);
	wrkUnit.SetCvt(0.0, UNITSMULT_MA, 0.0);
	UnitSet[UNITS_MA] = wrkUnit;
}
void deviceVarUnits::DevVarBadUnitFill()	// default - should never be used
{
	Unit wrkUnit;

	wrkUnit.NameUnit(UNITS_NONE, UNITSNAME_NONE);
	wrkUnit.SetCvt(0.0, UNITSMUTL_NONE, 0.0);
	UnitSet[UNITS_NONE] = wrkUnit;
}

dataItem& getBurstUnit(uint8_t burstMessageNumber)
{
	int cn = ItemValue(burstMsgArray[burstMessageNumber].command_number,uint16_t);

	switch (cn)
	{
	case 1:
	case 3:	
	{
		int U = ItemValue(devVar_PV.Units, uint8_t);
		int NVu = NONvolatileData.devVars[0].Units;
		if ( U  == HART_NOT_USED)  
			printf(" getBurstUnits 1/3 shows PV.Units as %d and NV.Units as %d\n",U,NVu);
		if (NVu == HART_NOT_USED)  
			printf(" getBurstUnits 1/3 shows PV.NV-Units as %d and PV.Units as %d\n", NVu, U);
		return devVar_PV.Units;
	}
	break;
	case 2:
	{
		dataItem& rv  = devVarArray[SLOT_244_PERCENT_RANGE_IDX].Units;
		return rv;
	}
		break;
	case 9:
	{
		uint8_t dvIdx = ItemValue(burstMsgArray[burstMessageNumber].indexList[0], uint8_t);
		if (dvIdx > 2)
		{
			printf("getBurstUnit for Cmd# 9, Index %d in Msg# %d is made UNUSED.\n",dvIdx, burstMessageNumber);
			return constUnused;
		}
		else
			return devVarArray[dvIdx].Units;
	}
		break;
	case 38:
	case 48:
		return constUnused;
	default:
	{
		printf("getBurstUnit for Cmd# %d in Msg# %d is not found so Unit is made UNUSED.\n",cn, burstMessageNumber);
		return constUnused;

	}
		break;
	}// endswitch
}




void deviceVar::WriteRealValue(float newValue)
{
	// get semaphore
	lastRealVal = newValue;
	// @ simulation inactive
	//		set Value to   newValue
	// else - simulating
	//		we're done
	// put semaphore
}

void deviceVar::Set_Simulation()
{
	// get semaphore
	// @ lastDVcmdCode = Fixed ( ie start simulation )
	//		set Value to simulateValue  
	//		set properties 0x80 bit to Set
	//    check to see if we need to set anything else here (cmd 48 bit?? device status PV fixed...)
	// else - code is Normal
	//		set properties 0x80 bit to Clr - we're NOT simulating any more
	//		set Value to lastRealVal (latest running average)
	// put semaphore
}



uint8_t dataItem::insertSelf( uint8_t **ppData, uint8_t *pInserCnt )
{
	uint8_t ret = -1;
	int i;
//fprintf(stderr, " Dataitem Insert entered.\n"); fflush(stderr);

	uint8_t *pSrc = (uint8_t *)pRaw;

	if (pSrc == 0x0000)
	{
		fprintf(stderr," Dataitem has Null data pointer.\n");fflush(stderr);
		return RC_DEV_SPEC;
	}

//fprintf(stderr, " Dataitem Insert started.\n"); fflush(stderr);
	for ( i = 0; i < len; i++ )
	{
//fprintf(stderr, " Dataitem Insert byte # %d type:: %d.\n", i, itemType); fflush(stderr);
		switch(itemType)
		{
		case ht_packed:	//12, x0c - we only save these to disc as binary, leave packed
		case ht_int8:	// 1 - 8 bit int
		{
//fprintf(stderr, " Dataitem Insert 8 bit[%d]::>  %p.\n", i, (pSrc + i)); fflush(stderr);
			ret = insert( * (pSrc + i), ppData, *pInserCnt );
		}
		break;
		case ht_int16:
		{
			ret = insert( ((uint16_t *) pRaw)[i], ppData, *pInserCnt );
		}
		break;
		case ht_int24:
		{
			ret = insert24(((uint32_t *)pRaw)[i], ppData, *pInserCnt);
		}
		break;
		case ht_int32:  // 4
		{
			ret = insert( ((uint32_t *) pRaw)[i], ppData, *pInserCnt );
		}
		break;
		case ht_int40:	// 5
		{
		}
		break;
		case ht_int48:
		{
		}
		break;
		case ht_int56:
		{
		}
		break;
		case ht_int64:
		{
		}
		break;
		case ht_float:		//9
		{
			ret = insert( ((float *) pRaw)[i], ppData, *pInserCnt );
		}
		break;
		case ht_double:	//10
		{
		}
		break;
		case ht_ascii:
		{
		}
		break;
		default:
		{
		}
		break;
		}// endswitch
	}// next
	if ( ret == -1 )
	{
		fprintf(stderr, "insertSelf doesn't support type %d at this time.\n",itemType);
		ret = RC_DEV_SPEC;		
	}
	return ret;
}



uint8_t dataItem::extractSelf( uint8_t **ppData, uint8_t *pInserCnt, uint8_t& /*in+out*/ changeCnt )
{

	uint8_t ret = -1;
	int i;// these are static so they aren't continuously being implemented
	static uint8_t  data_ui8Arr[128];
	static uint16_t data_ui16;
	static uint32_t data_ui32;
	static float    data_float;
	bool   changed = false;



	uint8_t *pSrc = (uint8_t *)pRaw;
	for ( i = 0; i < len; i++ )
	{
		switch(itemType)
		{
		// the only arrays in this app are packed or uint8
		case ht_packed:	//12, x0c - we only save these to disc as binary, leave packed
		case ht_int8:	// 1 - 8 bit int
		{
			//ret = extract( * (pSrc + i), ppData, *pInserCnt );
			ret = extract(data_ui8Arr[ i ], ppData, *pInserCnt);
			if (!ret && data_ui8Arr[i] != *(pSrc + i))
			{
				changed = true;
			}
			if ( !ret && changed && ((i + 1) >= len) )
			{// transfer
				for (int k = 0; k < len; k++)
				{
					*(pSrc + k) = data_ui8Arr[k];
				}
			}
		}
		break;
		case ht_int16:
		{
			assert(len == 1);
			//ret = extract( ((uint16_t *) pRaw)[i], ppData, *pInserCnt );
			ret = extract( data_ui16, ppData, *pInserCnt );
			if (!ret && data_ui16 != *((uint16_t *)pRaw) )
			{
				changed = true;
			}
			if ( !ret && changed )
			{// transfer
				*((uint16_t *)pRaw) = data_ui16;
			}
			else
			{
				changed = false;// we didn't transfer it so there is no change
			}
		}
		break;
		case ht_int24:
		{
		}
		break;
		case ht_int32:
		{
			assert(len == 1);
			//ret = extract( ((uint32_t *) pRaw)[i], ppData, *pInserCnt );
			ret = extract(data_ui32, ppData, *pInserCnt);
			if (!ret && data_ui32 != *((uint32_t *)pRaw))
			{
				changed = true;
			}
			if (!ret && changed)
			{// transfer
				*((uint32_t *)pRaw) = data_ui32;
			}
			else
			{
				changed = false;// we didn't transfer it so there is no change
			}
		}
		break;
		case ht_int40:	// 5
		{
		}
		break;
		case ht_int48:
		{
		}
		break;
		case ht_int56:
		{
		}
		break;
		case ht_int64:
		{
		}
		break;
		case ht_float:
		{
			assert(len == 1);
			// ret = extract( ((float *) pRaw)[i], ppData, *pInserCnt );
			ret = extract(data_float, ppData, *pInserCnt);
			if (!ret && data_float != *((float *)pRaw))
			{
				changed = true;
			}
			if (!ret && changed)
			{// transfer
				*((float *)pRaw) = data_float;
			}
			else
			{
				changed = false;// we didn't transfer it so there is no change
			}
		}
		break;
		case ht_double:	//10
		{
		}
		break;
		case ht_ascii:
		{
		}
		break;
		default:
		{
		}
		break;
		}// endswitch
	}// next
	if ( ret < 0 )
	{
		fprintf(stderr, "extractSelf doesn't support type %d at this time.\n",itemType);		
		ret = RC_DEV_SPEC;
	}
	else
	if (ret == 0 && changed)
	{
		trigger(true);// trigger burst for this variable
		if (isConfig)
		{
			changeCnt++;  // indicate to command one changed
		}

		if (!isVolatile)
		{
			writeNeeded = true;
		}
	}

	return ret;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
   Set Values
   = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

bool dataItem::setValue(const int8_t& rDatum)
{
	uint8_t aDatum = (uint8_t)rDatum;
	setValue(aDatum);
}


bool dataItem::setValue( const uint8_t& rDatum )
{// set current dataitem value to the value in the unsigned char
	bool changed = false;
	switch(itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum) changed = true;
		*pDst = rDatum;
	}
	break;
	case ht_int16:
	{// we will NOT sign extend an improper assignment
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum) changed = true;
		*pDst = 0 + rDatum;
	}
	break;
	case ht_int24:
	{
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0 + rDatum;
	}
	break;
	case ht_int32:
	{
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int40:	// 5
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int48:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int56:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int64:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_float:
	{
		float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (float)0.00;
		*pDst += rDatum;
	}
	break;
	case ht_double:	//10
	{
		double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += rDatum;
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr,"DataItem setValue Error! attempt to set ascii from unsigned char.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr,"DataItem setValue Error! attempt to set packed from unsigned char.\n");
	}
	break;
	default:
	{
		fprintf(stderr,"DataItem setValue Error! Unknown hart type (%d)\n",(int)itemType);
	}
	break;
	}// endswitch
	trigger(changed); // trigger burst for this variable

	return (changed);
}

bool dataItem::setValue(const int16_t& rDatum)
{
	uint16_t aDatum = (uint16_t)rDatum;
	return setValue(aDatum);
}
bool dataItem::setValue(const uint16_t& rDatum)
{
	bool changed = false;
	switch (itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		fprintf(stderr, "DataItem setValue Warning 8 bit variable set by 16 bit parameter.\n");
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (uint8_t)(rDatum & 0x00ff);
	}
	break;
	case ht_int16:
	{// we will NOT sign extend an improper assignment
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = rDatum;
	}
	break;
	case ht_int24:
	{
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0 + rDatum;
	}
	break;
	case ht_int32:
	{
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int40:	// 5
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int48:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int56:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int64:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_float:
	{
		fprintf(stderr, "DataItem setValue Warning float variable set by 16 bit parameter.\n");
		float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (float)0.00;
		*pDst += rDatum;
	}
	break;
	case ht_double:	//10
	{
		fprintf(stderr, "DataItem setValue Warning double variable set by 16 bit parameter.\n");
		double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += rDatum;
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set ascii from unsigned short.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set packed from unsigned short.\n");
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch

	trigger(changed);
	return (changed);
}

bool dataItem::setValue(const int32_t& rDatum)
{
	uint32_t aDatum = (uint32_t)rDatum;
	return setValue(aDatum);
}
bool dataItem::setValue(const uint32_t& rDatum)
{
	bool changed = false;
	switch (itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		fprintf(stderr, "DataItem setValue Warning 8 bit variable set by 32 bit parameter.\n");
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (uint8_t)(rDatum & 0x00ff);
	}
	break;
	case ht_int16:
	{
		fprintf(stderr, "DataItem setValue Warning 16 bit variable set by 32 bit parameter.\n");
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (uint16_t)(rDatum & 0xffff);
	}
	break;
	case ht_int24:
	{// we will NOT sign extend an improper assignment
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int32:
	{
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = rDatum;
	}
	break;
	case ht_int40:	// 5
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int48:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int56:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_int64:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_float:
	{
		fprintf(stderr, "DataItem setValue Warning float variable set by 32 bit parameter.\n");
		float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (float)0.00;
		*pDst += (float)rDatum;
	}
	break;
	case ht_double:	//10
	{
		fprintf(stderr, "DataItem setValue Warning double variable set by 32 bit parameter.\n");
		double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += rDatum;
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set ascii from unsigned int.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set packed from unsigned int.\n");
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch

	trigger(changed);
	return (changed);
}

// we will NOT sign extend an improper assignment
bool dataItem::setValue(const int64_t& rDatum)
{
	uint64_t aDatum = (uint64_t)rDatum;
	return setValue(aDatum);
}
bool dataItem::setValue(const uint64_t& rDatum)
{
	bool changed = false;
	switch (itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		fprintf(stderr, "DataItem setValue Warning 8 bit variable set by 64 bit parameter.\n");
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (uint8_t)(rDatum & 0x00ff);
	}
	break;
	case ht_int16:
	{
		fprintf(stderr, "DataItem setValue Warning 16 bit variable set by 64 bit parameter.\n");
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (uint16_t)(rDatum & 0xffff);
	}
	break;
	case ht_int24:
	{
		fprintf(stderr, "DataItem setValue Warning 24 bit variable set by 64 bit parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += (uint32_t)(rDatum & 0x00ffffff);
	}
	break;
	case ht_int32:
	{
		fprintf(stderr, "DataItem setValue Warning 32 bit variable set by 64 bit parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst += (uint32_t)(rDatum & 0x00000000ffffffff);
	}
	break;
	case ht_int40:	// 5
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += (rDatum & 0x000000ffffffffff);;
	}
	break;
	case ht_int48:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		*pDst = 0;
		*pDst += (rDatum & 0x0000ffffffffffff);;
	}
	break;
	case ht_int56:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += (rDatum & 0x00ffffffffffffff);;
	}
	break;
	case ht_int64:
	{
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		*pDst += rDatum;
	}
	break;
	case ht_float:
	{
		fprintf(stderr, "DataItem setValue Warning float variable set by 64 bit parameter.\n");
		float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = (float)0.00;
		*pDst += (float)rDatum;
	}
	break;
	case ht_double:	//10
	{
		fprintf(stderr, "DataItem setValue Warning double variable set by 64 bit parameter.\n");
		double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += (double)rDatum;
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set ascii from unsigned int.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set packed from unsigned int.\n");
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch

	trigger(changed);
	return (changed);
}


bool dataItem::setValue(const float& rDatum)
{
	bool changed = false;
	switch (itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		fprintf(stderr, "DataItem setValue Warning 8 bit variable set by float parameter.\n");
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);
		*pDst =  (uint8_t)(aDatum & 0x00ff);
	}
	break;
	case ht_int16:
	{
		fprintf(stderr, "DataItem setValue Warning 16 bit variable set by float parameter.\n");
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);
		*pDst = (aDatum & 0x0000ffff);
	}
	break;
	case ht_int24:
	{
		fprintf(stderr, "DataItem setValue Warning 24 bit variable set by float parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);
		*pDst += (uint32_t)(aDatum & 0x00ffffff);
	}
	break;
	case ht_int32:
	{
		fprintf(stderr, "DataItem setValue Warning 32 bit variable set by float parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);
		*pDst += aDatum;
	}
	break;
	case ht_int40:	// 5
	{
		fprintf(stderr, "DataItem setValue Warning 40 bit variable set by float parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);
		*pDst += (aDatum & 0x000000ffffffffff);;
	}
	break;
	case ht_int48:
	{
		fprintf(stderr, "DataItem setValue Warning 48 bit variable set by float parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);
		*pDst += (aDatum & 0x0000ffffffffffff);;
	}
	break;
	case ht_int56:
	{
		fprintf(stderr, "DataItem setValue Warning 56 bit variable set by float parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);;
		*pDst += (aDatum & 0x00ffffffffffffff);;
	}
	break;
	case ht_int64:
	{
		fprintf(stderr, "DataItem setValue Warning 64 bit variable set by float parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);;
		*pDst += aDatum;
	}
	break;
	case ht_float:
	{
		// in order to set NaNs and odd values, we have to do bytewise transfer
		uint8_t *a_flt = (uint8_t*)(pRaw);// dest
		uint8_t *a_FLT = (uint8_t*)(&rDatum);//sorc

		changed = (a_flt[0] != a_FLT[0]) || (a_flt[1] != a_FLT[1]) || (a_flt[2] != a_FLT[2]) || (a_flt[3] != a_FLT[3]);

		a_flt[0] = a_FLT[0];
		a_flt[1] = a_FLT[1];
		a_flt[2] = a_FLT[2];
		a_flt[3] = a_FLT[3];

		/*float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = rDatum;*/
	}
	break;
	case ht_double:	//10
	{
		// you can't put a double NaN into a float NaN
		double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += rDatum;
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set ascii from float.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set packed from float.\n");
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch

	trigger(changed);

	return changed;	
}

bool dataItem::setValue(const double& rDatum)
{
	bool changed = false;
	switch (itemType)
	{
	case ht_int8:	// 1 - 8 bit int
	{
		fprintf(stderr, "DataItem setValue Warning 8 bit variable set by double parameter.\n");
		uint8_t *pDst = (uint8_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);;
		*pDst = (aDatum & 0x00ff);
	}
	break;
	case ht_int16:
	{
		fprintf(stderr, "DataItem setValue Warning 16 bit variable set by double parameter.\n");
		uint16_t *pDst = (uint16_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);;
		*pDst = (aDatum & 0x0000ffff);
	}
	break;
	case ht_int24:
	{
		fprintf(stderr, "DataItem setValue Warning 24 bit variable set by double parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);;
		*pDst += (uint32_t)(aDatum & 0x00ffffff);
	}
	break;
	case ht_int32:
	{
		fprintf(stderr, "DataItem setValue Warning 32 bit variable set by double parameter.\n");
		uint32_t *pDst = (uint32_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint32_t aDatum = static_cast<uint32_t>(rDatum);;
		*pDst += aDatum;
	}
	break;
	case ht_int40:	// 5
	{
		fprintf(stderr, "DataItem setValue Warning 40 bit variable set by double parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);;
		*pDst += (aDatum & 0x000000ffffffffff);;
	}
	break;
	case ht_int48:
	{
		fprintf(stderr, "DataItem setValue Warning 48 bit variable set by double parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);
		*pDst += (aDatum & 0x0000ffffffffffff);;
	}
	break;
	case ht_int56:
	{
		fprintf(stderr, "DataItem setValue Warning 56 bit variable set by double parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);
		*pDst += (aDatum & 0x00ffffffffffffff);;
	}
	break;
	case ht_int64:
	{
		fprintf(stderr, "DataItem setValue Warning 64 bit variable set by double parameter.\n");
		uint64_t *pDst = (uint64_t *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0;
		uint64_t aDatum = static_cast<uint64_t>(rDatum);
		*pDst += aDatum;
	}
	break;
	case ht_float:
	{
		// let's assume this is a normal occurance....fprintf(stderr, "DataItem setValue Warning float variable set by double parameter.\n");
		float *pDst = (float *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = static_cast<float>(rDatum); 
	}
	break;
	case ht_double:	//10
	{
		// in order to set NaNs and odd values, we have to do bytewise transfer
		uint8_t *a_flt = (uint8_t*)(pRaw);// dest
		uint8_t *a_FLT = (uint8_t*)(&rDatum);//sorc

		changed = (a_flt[0] != a_FLT[0]) || (a_flt[1] != a_FLT[1]) || (a_flt[2] != a_FLT[2]) || (a_flt[3] != a_FLT[3]) ||
			      (a_flt[4] != a_FLT[4]) || (a_flt[5] != a_FLT[5]) || (a_flt[6] != a_FLT[6]) || (a_flt[7] != a_FLT[7]);

		a_flt[0] = a_FLT[0];
		a_flt[1] = a_FLT[1];
		a_flt[2] = a_FLT[2];
		a_flt[3] = a_FLT[3];
		a_flt[4] = a_FLT[4];
		a_flt[5] = a_FLT[5];
		a_flt[6] = a_FLT[6];
		a_flt[7] = a_FLT[7];

		/*double *pDst = (double *)pRaw;
		if (*pDst != rDatum)changed = true;
		*pDst = 0.00;
		*pDst += rDatum;*/
	}
	break;
	case ht_ascii:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set ascii from double.\n");
	}
	break;
	case ht_packed:
	{
		fprintf(stderr, "DataItem setValue Error! attempt to set packed from double.\n");
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch

	trigger(changed);
	return(changed);
}

// set the value in our pRaw to the value in the passed in pRaw
void dataItem::dataEqual(void* pOtherRaw, hartTypes_t otherType, uint8_t SLen)
{
	uint32_t  mask = 0xffffffff;
	uint64_t wmask = 0;
	if (otherType != itemType || SLen != len)
	{	fprintf(stderr, "DataItem dataEqual Warning: parameter type mismatch (me %d; other %d.\n",itemType,otherType);
		return;// doing nothing
	}
	if (pOtherRaw == NULL || pRaw == NULL)
	{
		fprintf(stderr, "DataItem dataEqual has a null data pointer..\n");
		return;// doing nothing
	}

	switch (itemType)// my itemtype
	{
	case ht_int8:	// 1 - 8 bit int
	{
		uint8_t *pDst = (uint8_t *)pRaw;
		*pDst = 0;
		uint8_t *pDatum = static_cast<uint8_t*>(pOtherRaw);;
		*pDst = *pDatum;
	}
	break;
	case ht_int16:
	{
		uint16_t *pDst = (uint16_t *)pRaw;
		*pDst = 0;
		uint16_t *pDatum = static_cast<uint16_t*>(pOtherRaw);;
		*pDst = *pDatum;
	}
	break;
	case ht_int24:
		mask = 0x00ffffff;
	case ht_int32:
	{// else default is 0xffffffff
		uint32_t *pDst = (uint32_t *)pRaw;
		*pDst = 0;
		uint32_t *pDatum = static_cast<uint32_t*>(pOtherRaw);;
		*pDst = (*pDatum) & mask;
	}
	break;
	case ht_int40:	// 5
		wmask = 0x000000ffffffffff;
	case ht_int48:
		if (wmask == 0)   wmask = 0x0000ffffffffffff;
	case ht_int56:
		if (wmask == 0 )  wmask = 0x00ffffffffffffff;
	case ht_int64:
	{
		if (wmask == 0 )  wmask = 0xffffffffffffffff;
		uint64_t *pDst = (uint64_t *)pRaw;
		*pDst = 0;
		uint64_t *pDatum = static_cast<uint64_t*>(pOtherRaw);;
		*pDst = (*pDatum) & mask;
	}
	break;
	case ht_float:
	{
		float *pDst = (float *)pRaw;
		*pDst = *( static_cast<float*>(pOtherRaw));
	}
	break;
	case ht_double:	//10
	{
		double *pDst = (double *)pRaw;
		*pDst = *(static_cast<double*>(pOtherRaw));
	}
	break;
	case ht_ascii:
	case ht_packed:
	{
		uint8_t *pDst = (uint8_t *)pRaw;
		*pDst = 0;
		uint8_t *pDatum = static_cast<uint8_t*>(pOtherRaw);;
		memcpy(pDst, pDatum, SLen);
	}
	break;
	default:
	{
		fprintf(stderr, "DataItem setValue Error! Unknown hart type (%d)\n", (int)itemType);
	}
	break;
	}// endswitch
}
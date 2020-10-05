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
 * Description: This holds definition of the primative data objects for this application.
 *				The object factory will use this to generate the actual DataSet.
 *
 * #include "PrimativeDataDesc.h"
*/
#pragma once

#ifndef _A_DATADESC_H
#define _A_DATADESC_H
#ifdef INC_DEBUG
#pragma message("In PrimativeDataDesc.h") 
#endif

#include "AllDataObjects.h"


#ifdef INC_DEBUG
#pragma message("    Finished Includes::PrimativeDataDesc.h") 
#endif


class classGenerator
{
public:
	static dataObject_Base* makeClass(hartTypes_t ht)
	{
		switch ( ht )
		{
		case ht_int8:
		{
			return new do_int8();
		}
		break;
		case ht_int16:
		{
			return new do_int16();
		}
		break;
		case ht_int24:
		{
			return new do_int24();
		}
		break;
		case ht_int32:
		{
			return new do_int32();
		}
		break;
		case ht_float:
		{
			return new do_float();
		}
		break;
		case ht_double:
		{
			return new do_double();
		}
		break;
		case ht_ascii:
		{
			return new do_ascii();
		}
		break;
		case ht_packed:
		{
			return new do_packed();
		}
		break;
		case ht_int40:	
		case ht_int48:
		case ht_int56:
		case ht_int64:
		{
			assert(0);// unimplemented data class   // no assert side effect 
		}
		break;
		case ht_Unknown:
		default:
		{
			assert(0);// unknown data class   // no assert side effect 
		}
		break;
		}// endswitch
	}

};

#define DESC_TYPE \
	string varName; \
	hartTypes_t varHtype; \
	int    varArrLen; \
	bool varConfig; \
	bool varVol;

// "itemName",    hartType,arrLen, isConfig, isVolatile, 
#define DATA_DESC  {\
{"responseCode",	ht_int8,  0,   false,   false},\
{"deviceStatus",	ht_int8,  0,   false,   false},\
{"expansion",		ht_int8,},\
{"mfrID",			ht_int8,},\
{"devType",			ht_int8,},\
{"reqPreambles",	ht_int8,},\
{"univRev",			ht_int8,},\
{"xmtrRev",			ht_int8,},\
{"swRev",			ht_int8,},\
{"hwRev",			ht_int8,},\
{"flags",			ht_int8,},\
{"devID",			ht_int8,},\
{"myPreambles",},\
{"devVarCnt",},\
{"configChangeCnt",},\
{"extended_fld_dev_status",},\
{"MfgId",},\
{"PrivLabelDistr",},\
{"Profile",},\
/* end of HART 7 zero response */\
{"writeProtectCode",},\
{"loopCurrentMode",},\
{"pollAddr",},\
{"tag",},\
{"message",},\
{"descriptor",},\
{"date",},\
{"finalAssembly",},\
{"longTag",},\
{"PVidx",},\
{"SVidx",},\
{"TVidx",},\
/* PV extension data */\
{"percentRange",},\
{"percentRange",},\
{"transferFunction",},\
{"upperRangeValue",},\
{"lowerRangeValue",},\
{"rangeValueUnits",},\
{"alarmSelectionCode",},\
{"analogChannelFlags",},\
/* totalizer data */\
{"totStatus",},\
{"totAddStatus",},\
{"totFamDefRev",},\
{"totFalSafHndling",},\
{"totMode",},\
{"totDirection",},\
{"totRelayForced",},\
{"totRelayInverted",},\
{"totClassification",},\
{"totValue",},\
{"totTimeStamp",},\
/* other */\
{"dateTimeStamp",},\
{"maxBurstMsgCnt",},\
}

// --- these all rely on the dataObject interfaces ---
// >>>    public dataObject_Base, public dataObjectConfig
//                                HART-type,  Raw-type,  length
//class do_int8  : public dataObject< ht_int8,   uint8_t,  1 > {};
//class do_int16 : public dataObject< ht_int16,  uint16_t, 2 > {};
//class do_int24 : public dataObject< ht_int24,  uint32_t, 3 > {};
//class do_int32 : public dataObject< ht_int32,  uint32_t, 4 > {};
//class do_float : public dataObject< ht_float,  float,    1 > {};
//class do_double: public dataObject< ht_double, double,   1 > {};
//class do_packed: public dataObject< ht_packed, uint8_t,  1 > {};

#endif // _A_DATADESC_H
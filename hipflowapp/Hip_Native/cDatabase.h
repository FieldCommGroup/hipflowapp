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
 * Description: This holds the data for the entire device and is the primary entrance.
 *	
 *	
 * #include "cDatabase.h"
*/
#pragma once

#ifndef _C_DATABASE_H
#define _C_DATABASE_H
#ifdef INC_DEBUG
#pragma message("In cDatabase.h") 
#endif

#include "./hipserver/hipserver/Common/datatypes.h"
// VS can't find this for some reason   #include "datatypes.h"
#include "cDataRaw.h"
#include "Units.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::cDatabase.h") 
#endif

//This is software +version+ in command 0
#define SOFTWAREVERSION  (uint8_t)50

extern bool isBIGendian;
extern const float hartBENaN;
extern const float hartLENaN;

#define NaN_value   ( (isBIGendian)?hartBENaN:hartLENaN )

//         ( dataitem, type )  eg float y = ItemValue(loopCurrent,float);
#define ItemValue( di,   ty )    (*(( ty * )di.pRaw))

extern  uint8_t insert_Ident(uint8_t &ByteCnt, uint8_t *pData);
extern  void    updateTimeStamp(void);// date+time to uint32
extern  uint8_t *pack(uint8_t *dest, const uint8_t *source, int nChar);

extern	bool    configChangedBit_Pri;
extern	bool	configChangedBit_Sec;

extern  bool    writeNeeded;


// cant be const or they won't gointo a reference
extern const float constant_zero;
extern const float constant_nan;
extern const uint8_t  constant_ui8;// 0
extern const uint16_t constant_ui16;//0
extern const uint32_t constant_ui32;//0
extern const uint8_t  constant_NotUsed;//250

class  dataItem;
extern dataItem  configChangeCnt;

class dataItem
{
	int  len;// size is len * size-of-type(itemType), if > 1, assume an array
public:
	bool isVolatile;	// is not saved in non-volatile storage
	bool isConfig;		// is a configuration variable
	bool isHasValue;	// false when Unitialized
	bool isChanged_pri;		// changed by internal, command or phy, clr by cmd48 write
	bool isChanged_sec;		// changed by internal, command or phy, clr by cmd48 write

	bool triggerOnChange;	// trigger burst message when the raw value changes
	bool hasBeenTriggered;	// set when trigger conditions met (level or changed)
							// clr once read by isTriggered()
	hartTypes_t itemType;
	void       *pRaw;			// pointer into a raw data type, requires a cast

public:
	dataItem(): isVolatile(false),isConfig(false), isHasValue(false),
		isChanged_pri(false),isChanged_sec(false), len(0),
		itemType(ht_Unknown),  pRaw(NULL), triggerOnChange(false), hasBeenTriggered(false){};
	dataItem(bool Vol, bool Con, hartTypes_t type, void *data, int sz=1 ): len(sz),isVolatile(Vol),isConfig(Con),
		isHasValue(false),isChanged_pri(false),isChanged_sec(false),itemType(type),pRaw(data), triggerOnChange(false),
		hasBeenTriggered(false)
	{
		set(Vol, Con, type, data, sz );
	};

	// set the value in our pRaw to the value in the passed in pRaw
	void dataEqual(void* pOtherRaw, hartTypes_t otherType, uint8_t SLen);

	dataItem(const dataItem& di){*this=di;};
	
	// used for initialization, sets default values and assigns memory
	void set(bool Vol, bool Con, hartTypes_t type, void *data, int sz=1 )
	{
		isVolatile = Vol;
		isConfig   = Con;
		itemType   = type;
		len        = sz;
		pRaw       = data;
		isHasValue   = false;
		isChanged_pri= false;
		isChanged_sec= false;
		if ( data == NULL )
			throw "DataItem failed";
	};

	~dataItem(){};// pRaw doesn't belong to us, nothing to do here

	dataItem& operator=(const dataItem &SRC)
	{
		isVolatile = SRC.isVolatile;
		isConfig = SRC.isConfig;
		itemType = SRC.itemType;
		len      = SRC.len;
		isHasValue   = SRC.isHasValue;
		isChanged_pri= SRC.isChanged_pri;
		isChanged_sec= SRC.isChanged_sec;
		triggerOnChange = SRC.triggerOnChange;
		hasBeenTriggered= false;

		dataEqual(SRC.pRaw, SRC.itemType, SRC.len);

		return *this;
	};

	uint8_t insertSelf ( uint8_t **ppData, uint8_t *pInserCnt );
	uint8_t extractSelf( uint8_t **ppData, uint8_t *pInserCnt, uint8_t& /*in+out*/ changeCnt);
	
	void trigger(bool hasChanged)
	{		
		if (hasChanged && triggerOnChange)
		{
			hasBeenTriggered = true;
		}
	}
	
	bool setValue(const  uint8_t & rDatum);
	bool setValue(const   int8_t & rDatum);
	bool setValue(const uint16_t & rDatum);
	bool setValue(const  int16_t & rDatum);
	bool setValue(const uint32_t & rDatum);
	bool setValue(const  int32_t & rDatum);
	bool setValue(const uint64_t & rDatum);
	bool setValue(const  int64_t & rDatum);
	bool setValue(const  float   & rDatum);
	bool setValue(const  double  & rDatum);

	void setTrigger() { triggerOnChange =true;    };
	void clrTrigger() { triggerOnChange  = false; };
	bool is_Triggered(){
	//printf("Database is_Triggered.\n");
		bool r = hasBeenTriggered; 
		if (triggerOnChange)
		{
			hasBeenTriggered = false;
		}
		return r;
	};
};// end dataItem


class deviceVarUnits
{
public:  // the list for this var
	UnitList  UnitSet;

protected:
	void DevVar0UnitFill();
	void DevVar1UnitFill();
	void DevVar2UnitFill();
	void DevVar3UnitFill();
	void DevVarPRUnitFill();
	void DevVarLCUnitFill();
	void DevVarBadUnitFill();
};

class deviceVar : public deviceVarUnits
{
	uint8_t dVN;	  //cmd9element_s.devVarNumber:: in the data base	0 - (#devVars-1) ie a reference to the base value
	float   lastRealVal; // from running average

public: // data >>>> volatile
	dataItem Value;	        // cmd9element_s.dvValue:: float ie digital value; Nott persistent; convert to xV units if required 
	// unused.... dataItem quality;       // ie data quality
	// unused.... dataItem lim_status;	// ie Limit status  Not persistent; 
	dataItem devVar_status;     // ie device family status

	dataItem lastDVcmdCode; // Write Device Variable Command Code	uint8_t  WrDVcmdCd;		// last Write Device Variable Command Code
	dataItem simulateUnits; //                                      uint8_t  simulateUnits;
	dataItem simulateValue; //                                        uint8_t  simulateUnits;
	dataItem simulateStats; //                                     float    simValue;

public: // data >>>> NonVolatile
	dataItem Units	;		  //cmd9element_s.dvUnits:: ie digital units
	dataItem classification;  //cmd9element_s.dvClass	
	dataItem deviceFamily;    //cmd9element_s.dvFamily  250 if not-used, bits in dvStatus need to be cleared as well
							  //cmd9element_s.dvLimMinSpanUnits
	dataItem upperSensorLimit;//cmd9element_s.dvUTL
	dataItem lowerSensorLimit;//cmd9element_s.dvLTL
	dataItem minSpan;		  //cmd9element_s.dvMinSpan
	dataItem dampingValue;	  //cmd9element_s.dvDamping... in seconds
	dataItem updatePeriod;	  //cmd9element_s.dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	dataItem sensorSerialNumber;  //cmd9element_s.dvSerial
	dataItem properties;      // bit-enum: 0x01-NOT calculated in device; 0x80-Value is simulated (never set when 0x01 set)

// Units in the base class

public: // c/dtor
	deviceVar(int devVarNum);
		void ctor_Device();
		void ctor_Stnd();
		void ctor_Unsupported();
	~deviceVar(){};// no pointers

public: // external update capability
	void Set_Simulation ();// from simulate data
	void WriteRealValue (float newValue);
	uint8_t devVarNumber() { return dVN;};

	static deviceVar* devVarPtr(uint8_t slotNumber);
	static inline bool isDevVar(uint8_t slotNumber){ return (devVarPtr(slotNumber) == NULL)?false:true; };
	static inline uint8_t DevVar2Index( uint8_t slotNumber );// maps the 'standard' DV# to DevVar Array indexes
};


extern dataItem maxBurstMsgs;

extern int step(uint32_t& aTime, unsigned &ret);

class burstMessage
{

public: // we'll start out with everything public and work from there

	dataItem message_number; 		//	0 thru 7 (my number)
	dataItem command_number;		//  16 bit
	dataItem burstModeCtrl;			// enum:0=off, 1burst toekn-pass, 2 and up other physical
	dataItem burstCommPeriod;		//  1 / 32 Ms
	uint32_t burstCommIn_50mS_ticks;
	dataItem maxBurstCommPeriod;		//  1 / 32 Ms
	uint32_t maxburstCommIn_50mS_ticks;
	// Trigger struct:
	dataItem TrigLvlMode;		//enum 0=continuous,1=Window,2=Rising,3=Falling;4=AnyChangeInMsgVars
	dataItem trigLvlClass;		
	dataItem trigLvlUnits;
	dataItem trigLvlValue;
	float    risingTrigVal;
	float    fallingTrigVal;
	int      activeIndexCount;
	dataItem indexList[MAX_CMD_9_RESP];

public:
	burstMessage(uint8_t x);// array position in ctor
	int stepTimes(void);
	void emptyTheList();
	bool setTriggerValues();// returns true on ERROR
	bool resetTriggerValues();
	void updateBurstOperation(void);
	void printParts(void);
	burstMessage& operator=(const burstMessage &SRC);

	static  int  step(uint32_t& aTime, unsigned &ret);
	static  bool isAburstCmd(uint16_t cmdNo);// tied to getBurstUnit
};

extern dataItem expansion;
extern dataItem mfrID   ;
extern dataItem devType  ;
extern dataItem reqPreambles;
extern dataItem univRev;
extern dataItem xmtrRev;
extern dataItem swRev;
extern dataItem hwRev;
extern dataItem flags;
extern dataItem devID;
	/* end of HART 5 zero response */
extern dataItem myPreambles;
extern dataItem devVarCnt;
	//   configChangeCnt	<- dynamic, endian
	//   extraDevStatus		<- dynamic
	/* end of HART 6 zero response */
extern dataItem MfgId;
extern dataItem PrivLabelDistr;
extern dataItem Profile;
	/* end of HART 7 zero response */

/* the following are variable via command or device */
// above dataItem    extern dataItem  configChangeCnt;
extern dataItem  tempconfigChangeCnt;
extern dataItem  extended_fld_dev_status;
extern dataItem  writeProtectCode;
extern dataItem  loopCurrentMode;

extern dataItem pollAddr;
extern dataItem tag;
extern dataItem message;
extern dataItem descriptor;
extern dataItem date;
extern dataItem finalAssembly;
extern dataItem longTag;
extern dataItem templongTag;


extern deviceVar devVarArray[NUMBER_OF_DEVICE_VARIABLES];
extern dataItem PVidx;
extern dataItem SVidx;
extern dataItem TVidx;

//PVextension
extern dataItem percentRange;
extern dataItem loopCurrent;		/* Not persistent; */

extern dataItem transferFunction;
extern dataItem upperRangeValue;/* used to convert % and loop current */
extern dataItem lowerRangeValue;	
extern dataItem rangeValueUnits;
// analog_io collection
extern dataItem alarmSelectionCode;	
extern dataItem analogChannelFlags;

extern dataItem dateTimeStamp;

// Totalizer Extension data
extern dataItem totStatus;
extern dataItem totAddStatus;
extern dataItem totFamDefRev;
// we are not implementing this::> extern dataItem totInputVarCode;
extern dataItem totFalSafHndling;
extern dataItem totMode;
extern dataItem totDirection;
extern dataItem totRelayForced;
extern dataItem totRelayInverted;

extern dataItem totClassification;
extern dataItem totValue;		//a double or long double
extern dataItem totTimeStamp;

extern burstMessage burstMsgArray[MAX_BURSTMSGS];
extern burstMessage tempBurstMsg;

extern dataItem responseCode;
// use getDeviceStatus()....extern dataItem deviceStatus;
extern dataItem constUnused;

extern dataItem& getBurstUnit(uint8_t burstMessageNumber);


// these are not actually mapped in this app, the infrastructure is in place though
#define devVar_PV    (devVarArray[NONvolatileData.devVar_Map[0]])
#define devVar_SV    (devVarArray[NONvolatileData.devVar_Map[1]])
#define devVar_TV    (devVarArray[NONvolatileData.devVar_Map[2]])
#define devVar_QV    (devVarArray[NONvolatileData.devVar_Map[3]]) /* support is required */
#define devVar_Total (devVarArray[NONvolatileData.devVar_Map[2]])/* is 2 */

//#define IS_TOTALIZER( a )  ( a == 0 || a == 2 ) /* DV is Flow OR DV is Total */
// in order to get two totalizers to track the same input, all totalizer cmds 
//  must only answer to totalizer DV in the request.  (as per SJV & WAP 03may2019)
#define IS_TOTALIZER( a )  ( a == 2 ) /*  DV is Total */

#endif //_C_DATABASE_H
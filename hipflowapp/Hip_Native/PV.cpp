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
 */

#include <string.h>
#include <stdio.h>

#include "app.h"
#include "msg.h"
#include "InsertExtract.h" // includes datatypes

#include "cDatabase.h"
#include "PV.h"



/*******  externs *******/
extern void sv_alarm(double newValue);
extern void pv_alarm(double newValue);

/******** globals ********/
rtData RTdata;

/**** note that Tim hard coded this in common.h and again in hartdefs.h in hip-server
instead of fighting it, I'll use those. This is left as a comment if you ever need the endian

extern   bool        isBIGendian;// false on ARM Pi...ie it's little endian
extern   const float hartBENaN;
extern   const float hartLENaN;
#define   HART_NAN    ((isBIGendian)?hartBENaN:hartLENaN)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/******************************************************************************
 *pressure data.  first the calibration data
 *   manual calibration:
 *		min counts    20000
 *		max counts	8388607
 *		min value   240   kgH
 *		max value	24000 kgH
 *      slope = (24000-240) / (8388607 - 20000) = 0.00283918219603334
 *		value = ( (rawValue-mincounts) * slope ) + minvalue
 */

 #ifdef _PRESSUREDEVICE
/* pressure settings */
float zero = 3355444.0,
    span =0.0000048025, /* 0 - 100 */
    lowerRawValue = 3355444.0,
    upperRawValue = 1251328.0,
    lowerTrimPt = -0.5,
    upperTrimPt = 147.2,
    lastValue = 845824.0;
#else
    #ifdef _FLOWDEVICE
	#else
	 /* potentiometer settings */
	float zero = 20000.0,
		span = 0.00283918219603334,
		lowerRawValue = 20000.0,
		upperRawValue = 8388607.0,
		lowerTrimPt = 240.0,
		upperTrimPt = 24000.0,
		lastValue = 4096.0;
	#endif
#endif

	uint8_t PV_analogChannelFlags = 0;// we output on the non-existent analog channel.


/* then the dynamic / device variable data
 */
#ifdef _PRESSUREDEVICE
 #define UNITS_PV  UNITS_PSI
 #define UNITS_SV  UNITS_F
#else
    #ifdef _FLOWDEVICE		/* note that these are the standard units */
     #define UNITS_PV  UNITS_KGpH
     #define UNITS_SV  UNITS_MA
    #else
     #define UNITS_PV  UNITS_NONE
	#endif
 #endif

unsigned char pressureUnits = UNITS_PV;
unsigned char currentUnits  = UNITS_SV;

float pressure  = 20.0,
    pctRange    = 16.0,
	current     = 60.0
  ;// this is now a data item  ,loopCurrent = hartLENaN;
#if 0 // ------------------hold till needed 
/* command 14/15 data
 */
struct  {
    unsigned char sensor_serial_number[3], 
        pressureUnits;
    float upperSensorLimit,        
        lowerSensorLimit,        
        minimumSpan;
} 
#ifdef _PRESSUREDEVICE 
    cmd14Data = { {12, 34, 56}, 6, 100.0, 0.0 , 5.0 };
#else
    #ifdef _FLOWDEVICE     
		cmd14Data = { {65, 43, 21}, 75, 24000.0, 0.0 , 10000.0 };
    #else    
		cmd14Data = { {12, 34, 56}, 6, 100.0, 0.0 , 5.0 };
	#endif
 #endif

struct  {
    unsigned char alarmCode,        
        xferFunc,
        pressureUnits;
    float upperRangeValue, /* for % range and loop current */
        lowerRangeValue,
        dampingValue;    
    unsigned char writeProtectCode,  
        distCode; // PrivateLabelDistributor...same as HART 7 cmd 0 PLdest
}
#ifdef _PRESSUREDEVICE 
 cmd15Data = { 0, 0, 6, 145.0, 0.0, 0.05, 0, 0xEF } ;
#else
    #ifdef _FLOWDEVICE     
		cmd15Data = { 0, 0, 6, 24000.0, 0.0, 0.05, 0, 0xEF } ;
    #else    
		cmd15Data = { 0, 0, 6, 145.0, 0.0, 0.05, 0, 0xEF } ;
	#endif
 #endif
                        

typedef struct cmd9element_s
{   unsigned char devVarNumber; 
	unsigned char dvClass,
				  dvUnits;
	float         dvValue;
	unsigned char dvStatus;
	uint32_t      dvSerial;
	uint8_t       dvLimMinSpanUnits;
	float         dvUTL;   // upper transducer Limit
	float         dvLTL;   // lower transducer Limit   units=dvLimMinSpanUnits
	float         dvDamping;//Sec
	float         dvMinSpan;//                         units=dvLimMinSpanUnits
	uint8_t       dvFamily; // 250 if not-used, bits in dvStatus need to be cleared as well
	uint32_t      dvAcqPeriod;// units=1/32mS; must have at least 1 acquasition in this time
	uint8_t       dvProperties; // commonTable...undefined
}/*typedef*/cmd9element_t;




typedef enum elem_e  // must match order of cmd9minDV to give valid indexes
{
	ELEM_000,
	ELEM_001,
	ELEM_002,
	ELEM_003,
	ELEM_244,
	ELEM_245,
	ELEM_246,
	ELEM_247,
	ELEM_248,
	ELEM_249
}/* typedef*/  elem_t;

// mass flow classification 72, current  is 84
cmd9element_t cmd9minDV[devVarSlots] ={ 
			{0, NO_CLASS, UNITS_PV,        0.0, DV_STATUS_GOOD,      0,UNITS_PV,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },
			{1, NO_CLASS, UNITS_SV,        0.0, DV_STATUS_GOOD,      0,UNITS_SV,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },
			{2, NO_CLASS, UNITS_NONE, HART_NAN, UNUSED_STATUS,  0,   UNITS_NONE,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0},
			{3, NO_CLASS, UNITS_NONE, HART_NAN, UNUSED_STATUS,  0,   UNITS_NONE,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0} ,
			//244	Percent Range (Unit Code is "percent";  Classification is "Not Classified(ie 0)")							
			{244, NO_CLASS, UNITS_PERCNT,    0.0, DV_STATUS_GOOD,    0,UNITS_PERCNT,  HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },// percent range
			// 245	Loop Current (Unit Code is "milliamperes"; Classification is " Not Classified(ie 0)")
			{245, NO_CLASS, UNITS_SV,   HART_NAN, DV_STATUS_GOOD,    0,UNITS_SV,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },// loop Current
			{246, NO_CLASS, UNITS_PV,        0.0, DV_STATUS_GOOD,    0,UNITS_PV,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },//246	Primary Variable
			{247, NO_CLASS, UNITS_SV,        0.0, DV_STATUS_GOOD,    0,UNITS_SV,      HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0 },//247	Secondary Variable
			{248, NO_CLASS, UNITS_NONE, HART_NAN, UNUSED_STATUS,0,UNITS_NONE,    HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0},//248	Tertiary Variable
			{249, NO_CLASS, UNITS_NONE, HART_NAN, UNUSED_STATUS,0,UNITS_NONE,    HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0}  };//249	Quaternary Variable
			
cmd9element_t scratchElement = 
            {250, NO_CLASS, UNITS_NONE, HART_NAN, UNUSED_STATUS,0,UNITS_NONE,    HART_NAN,HART_NAN,HART_NAN,HART_NAN,250,2500,0} ;

int getDevVarIndex(uint8_t desiredVar)
{
	int i;
	for ( i = 0; i < devVarSlots; i++)
	{
		if (desiredVar == cmd9minDV[i].devVarNumber)
		{
			return i;
		}
	}
	return -1;
}







/******************************************************************************
 *readPV
 *
 *command 1 service routine
 *
 ******************************************************************************
 */
char readPV(MSG * pMsg )             /* cmd 1 */
{
	uint32_t dummy = 0;
    uint8_t *pData =(uint8_t *)( pMsg->data);
    
    pMsg->byteCount = 2 + 1 + sizeof( float );

    getSema();// KS_lockw(L_PRESS);

    *pData ++ = pressureUnits;
    //*((float *)pData) = pressure; 
	insert(pressure, &pData, dummy);

    giveSema();//KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *readLoopCurrent
 *
 *command 2 service routine
 *
 ******************************************************************************
 */
char readLoopCurrent(MSG * pMsg )     /* cmd 2 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint32_t dummy =0;
        
    pMsg->byteCount = 2 + 2*sizeof( float );

   /* KS_lockw(L_PRESS);
    
    (*((float *)pData++)) = loopCurrent;
    (*((float *)pData))   = pctRange; 

    KS_unlock(L_PRESS);*/
	getSema();
	insert(loopCurrent, &pData, dummy);
	insert(pctRange,    &pData, dummy);
	giveSema();

    return 0;
}


/******************************************************************************
 *readDVwStatus
 *
 *command 9 service routine
 *
 ******************************************************************************
 */
char readDVwStatus(MSG * pMsg )   /* cmd 9 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	char reqCnt = pMsg->byteCount, dataCnt = 0;
	uint8_t reqBytes[8];
	int i, elemIdx;
	uint32_t dummyCnt = 0;
	cmd9element_t *pElem = NULL;
	bool isProtected = false;// true if it's a live data element
	memcpy(reqBytes, pData, 8);// we'll only look at the first reqCnt  of 'em

    if (reqCnt < 1 )     
	{
		if ( pMsg->byteCount == 0 )
		{// this is burst message - hard code response
			reqCnt = pMsg->byteCount = 2;
			reqBytes[0] = 0; // PV
			reqBytes[1] = 1; // SV
		}
		else
		{// it's a bad message
			pMsg->byteCount = 2;
			return 5;				/* too few data bytes */
		}
    }
	if (reqCnt > 8)// forward compatibility
	{
		reqCnt = 8;
	}

	// try this first byte without protection....
	// appears to be a valid request packet...
	*pData ++ = extended_fld_dev_status; dataCnt++;
	

	// can be 1 to 8 slots in request
	// we must answer 1 -4 & 244 - 250
	// the only valid data is in 0 & 1 & 246 & 247
	for ( i = 0; i < reqCnt; i++)
	{
		if (reqBytes[i] > 249)// ie 250 - 255
		{// abort
			pMsg->byteCount = 2;	// discard any work done
			return 2;               // invalid selection 
		}
		if ( ( elemIdx = getDevVarIndex(reqBytes[i]) ) >= 0  )
		{ 			
			pElem = &(cmd9minDV[elemIdx]);
			if (reqBytes[i] == 0 || reqBytes[i] == 1|| reqBytes[i] == 246 || reqBytes[i] == 247)
			{
				isProtected = true;
			}
		}
		else // not supported
		{
			scratchElement.devVarNumber = reqBytes[i];
			pElem = &scratchElement;
		}
		if (isProtected)
		{
			getSema();
		}
		// endian issues...memcpy(pData, pElem, sizeof(cmd9element_t) );
		*pData++ = pElem->devVarNumber;
		*pData++ = pElem->dvClass;
		*pData++ = pElem->dvUnits;
		insert(pElem->dvValue,&pData, dummyCnt);
		*pData++ = pElem->dvStatus;

		if (isProtected)
		{
			giveSema();
			isProtected = false;
		}
		dataCnt += 8; // this 'sizeof(cmd9element_t);' is size 12 (x0c)...
	}
	//The  Time consists of a unsigned 32-bit binary integer with the LSB representing 1/32 of a mS  (i.e., 0.03125 milliseconds).
	uint32_t y = time1_32mS;
	if ( ! isBIGendian ) // little endian
	{
	// for some reason the following does nothing
	//	uint32_t y = REVERSE_L(time1_32mS);
	// so code it out here...
		uint16_t g = (uint16_t)((y >> 16) & 0xffff);// hi word
		uint16_t h = (uint16_t)(y & 0xffff);
		y = (((uint32_t)REVERSE_S(h)) << 16) | (uint32_t)REVERSE_S(g);
	}
	memcpy(pData, &y, sizeof(uint32_t) );
	dataCnt += sizeof(uint32_t);

	pMsg->byteCount = dataCnt + 2;

	// rc 30  Command Response Truncated.... not used
    return 0;
}
/******************************************************************************
 *readDV
 *
 *command 3 service routine
 *
 ******************************************************************************
 */
char readDV(MSG * pMsg )             /* cmd 3 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint32_t dummyCnt=0;
        
    pMsg->byteCount = 2 + 1 + 1 + 3*sizeof( float );

    getSema();//KS_lockw(L_PRESS);

    //(*((float *)pData)) = loopCurrent;
	insert(loopCurrent,&pData,dummyCnt);
	// pData += sizeof(float);
    *pData ++ = pressureUnits;
    //*((float *)pData)   = pressure; 
	insert(pressure,&pData,dummyCnt);

	
    *pData ++ = cmd9minDV[ELEM_001].dvUnits;
	insert(cmd9minDV[ELEM_001].dvValue, &pData, dummyCnt);
    
    giveSema();//KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *readSnsrInfo
 *
 *command 14 service routine
 *
 ******************************************************************************
 */
char readSnsrInfo(MSG * pMsg )         /* cmd 14 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint32_t dummyCnt=0;

    unsigned int i;
    //char *pData = pMsg->data,
    //    *pSource = (char *) &cmd14Data;
        
    pMsg->byteCount = 2 + sizeof( cmd14Data );

    getSema();//KS_lockw(L_PRESS);
   /* for ( i = 0; i < sizeof( cmd14Data ); i++ ) {
        *pData++ = *pSource++;
    }*/
	memcpy(pData,(uint8_t*)&cmd14Data, 4); // first 4 bytes
	pData += 4;
	dummyCnt=4;// for debugging
	insert(cmd14Data.upperSensorLimit, &pData, dummyCnt);
	insert(cmd14Data.lowerSensorLimit, &pData, dummyCnt);
	insert(cmd14Data.minimumSpan,      &pData, dummyCnt);
    giveSema();//KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *readOutInfo
 *
 *command 15 service routine
 *
 ******************************************************************************
 */
char readOutInfo(MSG * pMsg )         /* cmd 15 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint32_t dummyCnt=0;

    /*unsigned int i;
    char *pData = pMsg->data,
        *pSource = (char *) &cmd15Data;*/
        
    pMsg->byteCount = 2 + sizeof( cmd15Data );

    getSema();//KS_lockw(L_PRESS);
   /* for ( i = 0; i < sizeof( cmd15Data ); i++ ) {
        *pData++ = *pSource++;
    }*/
	insert(cmd15Data.alarmCode,        &pData, dummyCnt);
	insert(cmd15Data.xferFunc,         &pData, dummyCnt);
	insert(cmd15Data.pressureUnits,    &pData, dummyCnt);
	insert(cmd15Data.upperRangeValue,  &pData, dummyCnt);
	insert(cmd15Data.lowerRangeValue,  &pData, dummyCnt);
	insert(cmd15Data.dampingValue,     &pData, dummyCnt);
	insert(cmd15Data.writeProtectCode, &pData, dummyCnt);
	insert(cmd15Data.distCode,         &pData, dummyCnt);
#if defined(_IS_HART7) || defined(_IS_HART6)
	insert(PV_analogChannelFlags,      &pData, dummyCnt);
	pMsg->byteCount++;
#endif
    giveSema(); //KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *readDVinfo
 *
 *command 54 service routine
 *
 ******************************************************************************
 */
char readDVinfo(MSG * pMsg )  /* cmd 54 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint8_t elemIdx = 0, dataCnt = 0;;
	cmd9element_t *pElem = NULL;
	bool isProtected = false;
	uint32_t dummyCnt;

	if ( pMsg->byteCount < 1 )
	{
        pMsg->byteCount = 2;
        return 5;            /* Too few data bytes received */
	}
	uint8_t dvIdx = *(pData);

	if (dvIdx > 249)// ie 250 - 255
	{// abort
		pMsg->byteCount = 2;	// discard any work done
		return 2;               // invalid selection 
	}
	if ( ( elemIdx = getDevVarIndex(dvIdx) ) >= 0  )
	{ 			
		pElem = &(cmd9minDV[elemIdx]);
		if (dvIdx == 0 || dvIdx == 1|| dvIdx == 246 || dvIdx == 247)
		{
			isProtected = true;
		}
	}
	else // not supported
	{
		scratchElement.devVarNumber = dvIdx;
		pElem = &scratchElement;
	}
	if (isProtected)
	{
		getSema();
	}
	// endian issues...memcpy(pData, pElem, sizeof(cmd9element_t) );
	uint8_t   temp32[4]; 
	uint8_t * pTemp = &(temp32[0]);

	*pData++ = pElem->devVarNumber;            dataCnt++;
	insert( pElem->dvSerial, &pTemp, dummyCnt);dataCnt+=3;
	*pData++ = temp32[1]; // 24 bit integer reversed so the hi byte is 0
	*pData++ = temp32[2];
	*pData++ = temp32[3];
	*pData++ = pElem->dvLimMinSpanUnits;      dataCnt++;
	insert(pElem->dvUTL,    &pData, dummyCnt);dataCnt+=sizeof(float);
	insert(pElem->dvLTL,    &pData, dummyCnt);dataCnt+=sizeof(float);
	insert(pElem->dvDamping,&pData, dummyCnt);dataCnt+=sizeof(float);
	insert(pElem->dvMinSpan,&pData, dummyCnt);dataCnt+=sizeof(float);
	*pData++ = pElem->dvClass;                dataCnt++;
	*pData++ = pElem->dvFamily;               dataCnt++;
	insert(pElem->dvAcqPeriod,&pData, dummyCnt);dataCnt+=4;
	*pData++ = pElem->dvProperties;           dataCnt++;

	if (isProtected)
	{
		giveSema();
		isProtected = false;
	}

	pMsg->byteCount = dataCnt + 2;
	return 0;//success
}

/******************************************************************************
 * sensor trim commands
 */

/******************************************************************************
 * readTrimPoints
 *
 * command 80 service routine
 ******************************************************************************
 */
char readTrimPoints(MSG * pMsg)     /* cmd 80 */
{
    uint8_t *pData = (uint8_t *)(pMsg->data);
	uint8_t vc;
	uint32_t dummyCnt=0;
    //char *pData = pMsg->data;

    if (pMsg->byteCount < 1)     /* too few data bytes */
	{
        pMsg->byteCount = 2;
        return 5;
    }

    if (*pData != 0 && *pData != 1) /* we only have 2 */
	{
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }
	else
	{
		vc = *pData;
	}
    
    //pMsg->byteCount = 2 + 2 + 2*sizeof( float );
	pMsg->byteCount = 2 + 1 + 1 + 2*sizeof( float );

	pData += 1;// we will leave the Device Variable code
	if (vc == 1) // we do now.....don't support trim on this guy
	{
		/*insert((char)250, &pData, dummyCnt);
		insert(hartLENaN, &pData, dummyCnt);
		insert(hartLENaN, &pData, dummyCnt);*/
		
		// no reason to lock (has to change via another cmd) getSema(); //KS_lockw(L_PRESS);
				
		insert(currentUnits,  &pData, dummyCnt);// we don't allow unit changes
		insert(c_lowerTrimPt, &pData, dummyCnt);
		insert(c_upperTrimPt, &pData, dummyCnt);
		// no reason to lock (has to change via another cmd) giveSema();//KS_unlock(L_PRESS);
	}
	else // it's PV...0
	{
		// no reason to lock (has to change via another cmd) getSema(); //KS_lockw(L_PRESS);

		//*pData ++ = 6;                /* we only support PSI */
		//(*((float *)pData++)) = lowerTrimPt; 
		//*((float *)pData) = upperTrimPt; 
		
		insert(pressureUnits, &pData, dummyCnt);// we don't allow unit changes
		insert(lowerTrimPt,   &pData, dummyCnt);
		insert(upperTrimPt,   &pData, dummyCnt);
		// no reason to lock (has to change via another cmd) giveSema();//KS_unlock(L_PRESS);
	}
 
    return 0;
}

/******************************************************************************
 *readTrimGuide
 *              
 * command 81 service routine
 ******************************************************************************
 */
char readTrimGuide(MSG * pMsg) {     /* cmd 81 */
/*********************************************************still big endian **********/
    char *pData = pMsg->data;
	uint8_t vc = 0;

    if (pMsg->byteCount < 1)     /* too few data bytes */
	{
        pMsg->byteCount = 2;
        return 5;
    }
	

    if (*pData != 0 && *pData != 1) /* we only have 2 */
	{
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }
	else
	{
		vc = *pData;
	}
    
    pMsg->byteCount = 2 + 3 + 5*sizeof( float );
 
    // no reason to lock (has to change via another cmd) getSema(); //KS_lockw(L_PRESS);

	*pData ++ = 2;                /* number of trim points */
	if ( vc ) // current
	{
		*pData ++ = currentUnits;// we don't allow unit changes
//		these are wrong
		*((float *)pData++) = cmd14Data.lowerSensorLimit; //min lowertrim point
		*((float *)pData++) = cmd14Data.upperSensorLimit; //max lower trim point
		*((float *)pData++) = cmd14Data.lowerSensorLimit; //min uppertrimpoint
		*((float *)pData++) = cmd14Data.upperSensorLimit; //max upper trim point
		*((float *)pData)   = cmd14Data.minimumSpan; 
	}
	else // pv press
	{
		*pData ++ = 2;                /* number of trim points */
		*pData ++ = pressureUnits;
		*((float *)pData++) = cmd14Data.lowerSensorLimit; 
		*((float *)pData++) = cmd14Data.upperSensorLimit; 
		*((float *)pData++) = cmd14Data.lowerSensorLimit; 
		*((float *)pData++) = cmd14Data.upperSensorLimit; 
		*((float *)pData)   = cmd14Data.minimumSpan; 
	}
    // no reason to lock (has to change via another cmd) giveSema();//KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *writeTrimPoint
 *               
 * command 82 service routine
 ******************************************************************************
 */
char writeTrimPoint(MSG * pMsg) {    /* cmd 82 */
/*********************************************************still big endian **********/
    uint8_t *pData = (uint8_t*)(pMsg->data),
        trimPt,
        retVal = 0;
	uint32_t   dataLen = pMsg->byteCount;
    float trimVal;

    if (pMsg->byteCount < (3 + sizeof(float)) ) 
	{
        pMsg->byteCount = 2;
        return 5;                /* too few data bytes */
    }

	uint8_t dynVarNum = *pData++; dataLen--;
    if (dynVarNum > 1  )         /* we only have two transmitter variables */
	{
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }
	
    
    /* make sure it is a trim point we support upper or lower*/
    trimPt = *pData ++; dataLen--;
    if (trimPt == 0 || trimPt > 2) {
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }

	uint8_t theseUnits = *pData ++; dataLen--;
    if ( ( dynVarNum == 0 && theseUnits != pressureUnits) ||
	     ( dynVarNum == 1 && theseUnits != currentUnits ) )
	{        /* we only support fixed units */
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }
	//int extract( float   & x, uint8_t **ppData, uint32_t& dataCnt)
    //trimVal = ( * (float *) pData);
	extract( trimVal, &pData, dataLen);

    /* is the trimVal in range?     */
    if (trimVal > cmd14Data.upperSensorLimit) {
        pMsg->byteCount = 2;
        return 3;            /* parameter to large */
    }

    if (trimVal < cmd14Data.lowerSensorLimit) {
        pMsg->byteCount = 2;
        return 4;            /* parameter to small */
    }

    pMsg->byteCount = 2 + 3 + sizeof(float);
 
    getSema();//KS_lockw(L_PRESS);

    switch (trimPt) {

    case 1:                        /* lower trim point */

        if ( (upperTrimPt - trimVal) < cmd14Data.minimumSpan) {
            pMsg->byteCount = 2;
            return 14;            /* span too small */
        }

        zero = lowerRawValue = lastValue;
        lowerTrimPt = trimVal;
        span = (upperTrimPt - lowerTrimPt) /
            (upperRawValue - lowerRawValue);
        break;
    
    case 2:                        /* upper trim point */

        if ( (trimVal - lowerTrimPt) < cmd14Data.minimumSpan) 
        {    pMsg->byteCount = 2;
            return 14;            /* span too small */
        }

        upperRawValue = lastValue;
        upperTrimPt = trimVal;
        span = (upperTrimPt - lowerTrimPt) /
            (upperRawValue - lowerRawValue);
        break;
    }
	double l = lastValue;
    updatePressure( l );  /* recalc reading */

    giveSema(); //KS_unlock(L_PRESS);
    return retVal;
}

/******************************************************************************
 *resetTrim
 *          
 * command 83 service routine
 ******************************************************************************
 */
char resetTrim(MSG * pMsg) {         /* cmd 83 */
/*********************************************************still big endian **********/
    char *pData = pMsg->data;

    if (pMsg->byteCount < sizeof(float) ) {
        pMsg->byteCount = 2;
        return 5;                /* too few data bytes */
    }

    if (*pData ++ != 1) {        /* we only have one transmitter variable */
        pMsg->byteCount = 2;
        return 2;                /* invalid selection */
    }
    
    pMsg->byteCount = 2 + 1;

    getSema();//KS_lockw(L_PRESS);

#ifdef _PRESSUREDEVICE
    zero = 3355444.0;
    span = 0.0000048025;
    lowerRawValue = 3355444.0;
    upperRawValue = 1251328.0;
    lowerTrimPt = -0.5;
    upperTrimPt = 147.2;
#else
    #ifdef _FLOWDEVICE
    zero = 4096.0;
    span = 1/1048576.0;
    lowerRawValue = 4096.0;
    upperRawValue = 1048576.0 + 4096.0;
    lowerTrimPt = 0.0;
    upperTrimPt = 30.0;
	#else
    zero = 4096.0;
    span = 1/1048576.0;
    lowerRawValue = 4096.0;
    upperRawValue = 1048576.0 + 4096.0;
    lowerTrimPt = 0.0;
    upperTrimPt = 30.0;
	#endif
#endif

    giveSema();//KS_unlock(L_PRESS);
    return 0;
}

/******************************************************************************
 *writeRawValue
 *              
 * command 200 service routine
 * used to test the pv calculation
 ******************************************************************************
 */
char writeRawValue(MSG * pMsg) {     /* cmd 200 */
/*********************************************************still big endian **********/
    char *pData = pMsg->data;

    pMsg->byteCount = 2 + sizeof( float );
	float y = * (float *) pMsg->data; // ERROR: this is big endian , we need extract float
	double z = y;
    updatePressure ( z );
 
    return 0;
}

#endif  // <<<<<<<<----------hold till needed 


// the cmd9minDV for real data is protected
//void getSema()
//{
//	sem_wait( &RTdataSema );
//}
//void giveSema()
//{
//	sem_post( &RTdataSema);
//}




/******************************************************************************
 * updatePrimaryVariable   pressure or flow

 *
 * called from adc task to update main reading
 * takes the average adc value and calculates the PV. 
 ******************************************************************************
 */
//char updatePrimaryVariable(double rawValue) 
//{
//    getSema();
//
//    lastValue = rawValue;
//
//	double localDouble = (rawValue-zero) * span + lowerTrimPt;
//	devVar_PV.Value.setValue(localDouble);
//	pv_alarm( localDouble );// handles all status
//
//	#ifdef hold_DEBUG
//		printf("  set PV to %f.\n",localDouble);
//	#endif
//	//pressure = cmd9minDV[ELEM_000].dvValue = cmd9minDV[ELEM_246].dvValue =
//	//(rawValue-zero) * span + lowerTrimPt;
//
//#ifdef xxxNOW_DONE_IN_PV_ALARM_ABOVE_____FLOWDEVICE
//	if (cmd48Trigger)
//	{
//		//************************************************* detect change and trigger burst
//	}
//	uint8_t local4compiler = DV_STATUS_GOOD;
//	devVar_PV.devVar_status.setValue(local4compiler);
//	//cmd9minDV[ELEM_000].dvStatus = DV_STATUS_GOOD;// default	
//	volatileData.cmd48Data[0]&= 0xFE;// clear low flow
//	//Data48.device_specific_status_0 &= 0xFE;// clear low flow
//	uint8_t localDevStat = *((uint8_t*)deviceStatus.pRaw);
//	localDevStat &= ~0x02;// clear PV out of limits
//	deviceStatus.setValue(localDevStat);
//	//devStat &= ~0x02;// clear PV out of limits
//
//	if (localDouble < 240)
//	{
//		float float4compiler = constant_zero;
//		devVar_PV.Value.setValue(float4compiler);
//		//pressure = cmd9minDV[ELEM_000].dvValue = cmd9minDV[ELEM_246].dvValue = 0.0;
//		//devStat |= 0x02;// PV out of limits
//		localDevStat |= 0x02;// PV out of limits
//		deviceStatus.setValue(localDevStat);
//	}
//	if (localDouble < 2400 )
//	{
//		// set device variable status to 'poor accuracy'
//		local4compiler &= ~DV_STATUS_GOOD;// clr two
//		//cmd9minDV[ELEM_000].dvStatus &= ~DV_STATUS_GOOD;// clr two
//		local4compiler &= DV_STATUS_POOR;//set one
//		//cmd9minDV[ELEM_000].dvStatus |= DV_STATUS_POOR;//set one
//		devVar_PV.devVar_status.setValue(local4compiler);
//
//		// cmd 48 byte 0, low-flow set
//		//Data48.device_specific_status_0 |= 0x01; // set low flow
//		volatileData.cmd48Data[0] |= 0x01; // set low flow
//	}
//	else
//	if (localDouble > 23000 )
//	{
//		// leave  device variable status to 'poor accuracy' off
//		// leave low flow off
//		//extended_fld_dev_status
//		localDevStat |= 0x02;// PV out of limits
//		deviceStatus.setValue(localDevStat);
//	}
//	// else leave default DV_STATUS_GOOD
//#endif
//
//
//
//#ifdef _FLOWDEVICE
//#define MULTIPLIER  100.0
//#else
//#define MULTIPLIER 100.0
//#endif
//    float pct_Range = MULTIPLIER * (localDouble - ItemValue(lowerRangeValue, float)) /
//	//	(cmd15Data.upperRangeValue - cmd15Data.lowerRangeValue);
//        (ItemValue( upperRangeValue, float) - ItemValue( lowerRangeValue, float));
//
//    // we want to leave it at NaN since HART-IP doesn't do   loopCurrent = 4.0 + 0.16 * pctRange;
//
//	percentRange.setValue(pct_Range);
//
//// next deal with the totalizer
//#ifdef _FLOWDEVICE
//#endif
//
//
//    giveSema(); 
//
//    return 0;
//}
//
//char updateSecondaryVariable(double rawValue)
//{
//    getSema();
//
//    c_lastValue = rawValue;
//
//	double localDouble = ((rawValue - b_zero) * b_span) + b_lowerTrimPt;
//	devVar_SV.Value.setValue(localDouble);
//	#ifdef hold_DEBUG
//		printf("  set SV to %f.\n", localDouble);
//	#endif
//	//current = cmd9minDV[ELEM_001].dvValue = cmd9minDV[ELEM_247].dvValue =
//	//((rawValue-b_zero) * b_span) + b_lowerTrimPt;
//	sv_alarm(localDouble);
//
//#ifdef xxxNOW_DONE_IN_PV_ALARM_ABOVE_____FLOWDEVICE	
//	uint8_t localDevStat = *((uint8_t*)deviceStatus.pRaw);
//	uint8_t localExtended= *((uint8_t*)extended_fld_dev_status.pRaw);
//	if ( localDouble > MA_ALARM )
//	{	
//		localDevStat |= 0x91; //devicestatus to SV out of limits & Malfunction & More Status available
//		
//		//set cmd 48 bit hi-drive-current 
//		//Data48.device_specific_status_0 |= 0x02; // set hi current
//		volatileData.cmd48Data[0] |= 0x02;
//		
//		localExtended |= 0x01;// device needs maintence
//	}
//	else
//	{
//		localDevStat &= ~0x91; //undo devicestatus to SV out of limits & Malfunction & MSA
//		volatileData.cmd48Data[0] &= ~0x02; // clr hi current
//		localExtended &= 0xFE;// device needs maintence
//	}
//	deviceStatus.setValue(localDevStat);
//	extended_fld_dev_status.setValue(localExtended);
//#endif
//
//    giveSema();
// 
//    return 0;
//}
//
//char updateTertiaryVariable(double rawValue)
//{
//	getSema();
//
//#if 0
//	c_lastValue = rawValue;
//	double localDouble = ((rawValue - b_zero) * b_span) + b_lowerTrimPt;
//	devVar_SV.Value.setValue(localDouble);
//#ifdef hold_DEBUG
//	printf("  set SV to %f.\n", localDouble);
//#endif
//	//current = cmd9minDV[ELEM_001].dvValue = cmd9minDV[ELEM_247].dvValue =
//	//((rawValue-b_zero) * b_span) + b_lowerTrimPt;
//
//#ifdef _FLOWDEVICE	
//	uint8_t localDevStat = *((uint8_t*)deviceStatus.pRaw);
//	uint8_t localExtended = *((uint8_t*)extended_fld_dev_status.pRaw);
//	if (localDouble > MA_ALARM)
//	{
//		localDevStat |= 0x91; //devicestatus to SV out of limits & Malfunction & More Status available
//
//		//set cmd 48 bit hi-drive-current 
//		//Data48.device_specific_status_0 |= 0x02; // set hi current
//		volatileData.cmd48Data[0] |= 0x02;
//
//		localExtended |= 0x01;// device needs maintence
//	}
//	else
//	{
//		localDevStat &= ~0x91; //undo devicestatus to SV out of limits & Malfunction & MSA
//		volatileData.cmd48Data[0] &= ~0x02; // clr hi current
//		localExtended &= 0xFE;// device needs maintence
//	}
//	deviceStatus.setValue(localDevStat);
//	extended_fld_dev_status.setValue(localExtended);
//#endif
//#endif // commented out
//	giveSema();
//
//	return 0;
//}


/******************************************************************************
 * zeroDV
 *
 * Cmd 52 - Zero device variable with current raw value 
 ******************************************************************************
 */
void zeroDV(uint8_t dv2Bzeroed)
{
	printf("zeroDV()  Not Yet Implemented in PV.cpp\n");
}

///******************************************************************************
// * init_ProcessVariables
// *
// * starts gen'ing PV data 
// ******************************************************************************
// */
//
//int init_ProcessVariables(void)
//{ 
//    int rtn;
//
//	if  ( (rtn = sem_init(&RTdataSema,    0, 1)) == -1 )
//	{
//		return rtn;
//	}
//	uint8_t local4compiler = UNITS_PV;
//	devVar_PV.Units.setValue(local4compiler);
//	updatePrimaryVariable(4096.0);// arbitrary
//	
//    return 0;
//}    


/***************************************************************************************/
rtData::rtData()
{	
	int rtn;

	if ((rtn = sem_init(&RTdataSema, 0, 1)) == -1)
	{
		throw(EXCP_NO_SEMAPHORE);
	}
	rtFlow = rtCurrent = rtTotalizer=rtPercentRng = 0.0;
	float zero = 17536.0,
		span = (float)0.00286701665772516,
		lowerRawValue = 20000.0,
		upperRawValue = 8388607.0,
		lowerTrimPt = 10.0,
		upperTrimPt = 24000.0,
		lastValue = 4096.0;


	uint8_t local4compiler = UNITS_PV;
	devVar_PV.Units.setValue(local4compiler);
	updateFlow(4096.0);// arbitrary number

}

rtData::~rtData()
{
	sem_close(&RTdataSema);
}

char rtData::updateFlow(double raw)
{
	sem_wait(&RTdataSema);

	lastValue = raw;

	rtFlow = (raw - zero) * span + lowerTrimPt;
	float local = (float)rtFlow;// we lose precision here
	volatileData.devVars[rtFlowSlot].Value = local;
	pv_alarm(rtFlow);// handles all status

	float rtPercentRng = 100.0 * (rtFlow - ItemValue(lowerRangeValue, float)) /
		//	(cmd15Data.upperRangeValue - cmd15Data.lowerRangeValue);
		(ItemValue(upperRangeValue, float) - ItemValue(lowerRangeValue, float));
	volatileData.percentRange = rtPercentRng;

	sem_post(&RTdataSema);

	return 0;
}

char rtData::updateCurrent(double raw)
{
	sem_wait(&RTdataSema);

	c_lastValue = (float)raw;

	rtCurrent = ((raw - b_zero) * b_span) + b_lowerTrimPt;
	float local = (float)rtCurrent;// we lose precision here
	volatileData.devVars[rtCurrentSlot].Value = local;
	sv_alarm(rtCurrent);

	sem_post(&RTdataSema);

	return 0;
}


char rtData::updateTotal(double raw)
{
	sem_wait(&RTdataSema);
	// find out if/what/ how to total
	// add/subtract from total
	// do alarms / other actions
	//volatileData.devVars[rtTotalizerSlot].Value = local;
	sem_post(&RTdataSema);
	return 0;
}

double rtData::getFlow(void)
{	double r; sem_wait(&RTdataSema); r = rtFlow; sem_post(&RTdataSema); return r;
};
double rtData::getCurrent(void)
{	double r; sem_wait(&RTdataSema); r = rtCurrent; sem_post(&RTdataSema); return r;
};
double rtData::getTotal(void)
{
	double r; sem_wait(&RTdataSema); r = rtTotalizer; sem_post(&RTdataSema); return r;
};
double rtData::getPercent(void)
{
	double r; sem_wait(&RTdataSema); r = rtPercentRng; sem_post(&RTdataSema); return r;
};

//static - gives a  number OR NaN on not supported
double rtData::getDevVar(uint8_t slotNumber)
{
	switch (slotNumber)
	{
	case 0:
	case 246:
		return rtFlow;
	case 1:
	case 247:
		return rtCurrent;
	case 2:
	case 248:
		return rtTotalizer;
	case 244:
		return rtPercentRng;
	case 3:
	case 245:
	case 249:
	default:
		return NaN_value;
	}
}

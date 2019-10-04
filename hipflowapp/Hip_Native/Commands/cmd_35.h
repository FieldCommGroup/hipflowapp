/*************************************************************************************************
 *
 * Workfile: cmd_35.h
 * 14Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_35.h"
 */

#ifndef CMD_35_H_
#define CMD_35_H_

#include <math.h>
#include "command.h"


class cmd_35 : public cmd_base
{
	uint8_t tempRangeValueUnits;
	float   tempUpperRangeValue;
	float   tempLowerRangeValue;

	uint8_t checkNsetValues(void);

public: // c.dtor
	cmd_35():cmd_base(35) {	};
	~cmd_35(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_35::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	if (ByteCnt < 9)
	{
		ByteCnt = 0;
		return RC_TOO_FEW;// too few data bytes
	}

	do // once
	{
		ret = extract(tempRangeValueUnits, &pData, ByteCnt);
		ret = extract(tempUpperRangeValue, &pData, ByteCnt);
		ret = extract(tempLowerRangeValue, &pData, ByteCnt);

		ret = checkNsetValues();
	} while (false);// execute once, allow breaks

	if (ret)
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_35::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if (ret = rangeValueUnits.insertSelf(&pData, &ByteCnt)) break;//  6		
		if (ret = insert(tempUpperRangeValue, &pData, ByteCnt)) break;//+12	
		if (ret = insert(tempLowerRangeValue, &pData, ByteCnt)) break;//+ 3
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

// note:  the units are just a string to put on the screen.
// the user has to calibrate to a particular unit, no attention is paid
// to units in the device.
uint8_t cmd_35::checkNsetValues(void)
{
	uint8_t retCode = RC_SUCCESS;
	bool ischanged = false;

	bool have1 = false, have2 = false;
	float USL = ItemValue(devVar_PV.upperSensorLimit, float);
	float LSL = ItemValue(devVar_PV.lowerSensorLimit, float);
	float Spn = ItemValue(devVar_PV.minSpan, float);

	// check for supported units code
	if (devVar_PV.UnitSet.isaUnit(tempRangeValueUnits) == false)
	{
		// is not recognized by this device
		printf(" cmd_35::checkNsetValues invalid units %d.\n", tempRangeValueUnits);
		return RC_INVALID;// don't change anything		
	}

	// convert range value units to standard units for comparison and storage
	float LRV, URV;	// in standard units
	try {
		URV = devVar_PV.UnitSet[tempRangeValueUnits].to_Standard(tempUpperRangeValue);
		LRV = devVar_PV.UnitSet[tempRangeValueUnits].to_Standard(tempLowerRangeValue);
	}
	catch (...)
	{
		printf(" cmd_35::checkNsetValues cannot convert from PV.Units %d to standard units.\n", tempRangeValueUnits);
		return RC_INVALID;// don't change anything
	}

	// don't compare to each other, just sensor limits
	if (LRV > USL)//LRV is too big
	{
		retCode = RC_MULTIPLE_9;//LRV too Hi
		have1   = true;// first test, can't already be true
	}
	if (LRV < LSL )// LRV is less than Lower-Sensor-Limit
	{
		retCode = RC_MULTIPLE_10;//LRV too Lo
		have1 = true;// this will over-ride the previous and still show LRV out
	}



	if (URV < LSL)// when URV is less than lowest sensor value
	{
		retCode = RC_MULTIPLE_12;// URV too Lo
		have2 = true;// this will over-ride the previous and butl show URV out too
	}
	if (URV > USL)// URV is greater than the Upper-Sensor-Limit
	{
		retCode = RC_MULTIPLE_11;//URV too Hi
		have2 = true;// this will over-ride the previous and still show URV out
	}
	if (have1 && have2)
	{
		return RC_MULTIPLE_13; // they're both out, don't change values
	}
	if (have1 || have2)
	{
		return retCode;
	}
	//if we get here we change values
	if (fabs(URV - LRV) < Spn)
	{
		retCode = RC_WARN_14;
	}
	
	ischanged  = upperRangeValue.setValue(URV);/* used to convert % and loop current */
	ischanged |= lowerRangeValue.setValue(LRV);
	ischanged |= rangeValueUnits.setValue(tempRangeValueUnits);

	if (ischanged)
	{
		if (upperRangeValue.isConfig || lowerRangeValue.isConfig || rangeValueUnits.isConfig )
			bumpConfigCnt();

		if ((!upperRangeValue.isVolatile) || (!lowerRangeValue.isVolatile) || (!rangeValueUnits.isVolatile))
			writeNeeded = true; // actual write done in NativeApp::handle_device_message
	}
	return retCode;
};

#endif // CMD_35_H_
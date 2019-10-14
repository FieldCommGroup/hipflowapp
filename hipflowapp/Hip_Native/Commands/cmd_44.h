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
 * Description: This holds one command
 *
 * #include "cmd_44.h"
 */

#ifndef CMD_44_H_
#define CMD_44_H_

#include "command.h"


class cmd_44 : public cmd_base
{
	uint8_t tempUnits;
	uint8_t checkValue(void);

public: // c.dtor
	cmd_44():cmd_base(44) {	};
	~cmd_44(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_44::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	// write pv units (1 byte)
	if (ByteCnt < 1)
	{
		ByteCnt = 0;
		return RC_TOO_FEW;// too few data bytes
	}

	ret = extract(tempUnits, &pData, ByteCnt);

	ret = checkValue();

	if (!isError(ret) && chngCnt != 0)// something changed in this extraction
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_44::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	devVar_PV.Units.insertSelf( &pData, &ByteCnt );

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

uint8_t cmd_44::checkValue(void)
{
	uint8_t retCode = RC_SUCCESS;
	bool ischanged = false;

	// check for supported units code
	if (devVar_PV.UnitSet.isaUnit(tempUnits) == false)
	{	
		// tempUnits is not recognized by this device
		return RC_INVALID;// don't change anything		
	}

	//if we get here we change values
	ischanged = devVar_PV.Units.setValue(tempUnits);

	if (ischanged)
	{
		if (devVar_PV.Units.isConfig)
			bumpConfigCnt();

		if (!devVar_PV.Units.isVolatile)
			writeNeeded = true; // actual write done in NativeApp::handle_device_message
	}

	return retCode;
}
#endif // CMD_44_H_
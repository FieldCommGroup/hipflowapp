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
 * #include "cmd_14.h"
 */

#ifndef CMD_14_H_
#define CMD_14_H_

#include "command.h"


class cmd_14 : public cmd_base
{

public: // c.dtor
	cmd_14():cmd_base(14) {	};
	~cmd_14(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_14::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// nothing to extract here

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_14::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = devVar_PV.sensorSerialNumber.insertSelf( &pData, &ByteCnt )  ) break;//0-2	Unsigned-24	Transducer Serial Number 
		if ( ret = devVar_PV.Units             .insertSelf( &pData, &ByteCnt )  ) break;//3	    Enum	Transducer Limits and Minimum Span Units Code (refer to Common Tables Specification)
		if ( ret = convertAndInsert(devVar_PV.upperSensorLimit, devVar_PV, &pData, &ByteCnt )  ) break;//4-7	Float	Upper Transducer Limit
		if ( ret = convertAndInsert(devVar_PV.lowerSensorLimit, devVar_PV, &pData, &ByteCnt )  ) break;//8-11	Float	Lower Transducer Limit
		if ( ret = convertAndInsert(devVar_PV.minSpan         , devVar_PV, &pData, &ByteCnt )  ) break;//12-15	Float	Minimum Span
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_14_H_
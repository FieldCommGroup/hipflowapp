/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
	 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: This holds one command
 *
 * #include "cmd_15.h"
 */

#ifndef CMD_15_H_
#define CMD_15_H_

#include "command.h"
#include "factory_reset.h"


class cmd_15 : public cmd_base
{

public: // c.dtor
	cmd_15():cmd_base(15) {	};
	~cmd_15(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_15::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// nothing to extract here

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_15::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	uint8_t wrtPrtCd = 0;

	// #135
	wrtPrtCd = (uint8_t) writeProtectSet();

	do // once
	{
		if ( ret = alarmSelectionCode    .insertSelf( &pData, &ByteCnt )  ) break;//0	Enum	PV Alarm Selection Code (see Common Table 6, Alarm Selection Codes).  The Alarm Selection Code indicates the action taken by the device under error conditions.
		if ( ret = transferFunction		 .insertSelf( &pData, &ByteCnt )  ) break;//1	Enum	PV Transfer Function Code (see Common Table 3, Transfer Function Codes). The Transfer Function Code must return "0", Linear, if transfer functions are not supported by the device.
		if ( ret = devVar_PV.Units.       insertSelf( &pData, &ByteCnt )  ) break;//2	Enum
		if ( ret = convertAndInsert(upperRangeValue, devVar_PV, &pData, &ByteCnt )  ) break;//3-6	Float	PV Upper Range Value
		if ( ret = convertAndInsert(lowerRangeValue, devVar_PV, &pData, &ByteCnt )  ) break;//7-10	Float	PV Lower Range Value
		if ( ret = devVar_PV.dampingValue.insertSelf( &pData, &ByteCnt )  ) break;//11-14	Float	PV Damping Value (units of seconds)
		if ((ret = insert( wrtPrtCd, &pData, ByteCnt ) ) != RC_SUCCESS ) break;//15	Enum	Write Protect Code (see Common Table 7, Write Protect Codes). The Write Protect Code must return "251", None,  when write protect is not implemented by a device.
		{ *pData++ = 250; ByteCnt++; }						                      //16	Enum	Reserved. Must be set to "250", Not Used.
		if ( ret = analogChannelFlags	 .insertSelf( &pData, &ByteCnt )  ) break;//17	Bits	PV Analog Channel Flags (see Common Table 26, Analog Channel Flags)
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_15_H_
/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
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
 * #include "cmd_18.h"
 */

#ifndef CMD_18_H_
#define CMD_18_H_

#include "command.h"


class cmd_18 : public cmd_base
{

public: // c.dtor
	cmd_18():cmd_base(18) {	};
	~cmd_18(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_18::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0, dbgChar = *pData;
	if ( ByteCnt < 21 )
	{
		ByteCnt = 0;
		return RC_TOO_FEW;// too few data bytes
	}
	
	do // once
	{
		if ( ret = tag.       extractSelf( &pData, &ByteCnt, chngCnt)  ) break;
		if ( ret = descriptor.extractSelf( &pData, &ByteCnt, chngCnt)  ) break;
		if ( ret = date.      extractSelf( &pData, &ByteCnt, chngCnt)  ) break;
	}
	while(false);// execute once, allow breaks

	if ( ret )
	{
		printf("Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	if (!isError(ret) && chngCnt != 0)// something changed in this extraction
	{
		printf("Cmd 18 shows something changed, tag byte 0 = 0x%02x, sent byte 0 = 0x%02x\n", NONvolatileData.tag[0], dbgChar);
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_18::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = tag.       insertSelf( &pData, &ByteCnt )  ) break;//  6		
		if ( ret = descriptor.insertSelf( &pData, &ByteCnt )  ) break;//+12
		if ( ret = date.      insertSelf( &pData, &ByteCnt )  ) break;//+ 3
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_18_H_
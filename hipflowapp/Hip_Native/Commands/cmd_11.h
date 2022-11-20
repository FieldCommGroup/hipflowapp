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
 * #include "cmd_11.h"
 */

#ifndef CMD_11_H_
#define CMD_11_H_

#include "command.h"
#include "cmd_00.h"


class cmd_11 : public cmd_base
{

public: // c.dtor
	cmd_11():cmd_base(11) {	};
	~cmd_11(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error, or DO-NOT-SEND
// cmd-11 has NO valid responde codes sow we have SUCCESS or DO-NOT-SEND
uint8_t cmd_11::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	uint8_t incoming[6];
	if (ByteCnt < 6)
	{
		fprintf(stderr,"Cmd 11 byte count error\n");
		return RC_DONOTSEND;
	}
	
	for ( int i = 0; i < 6; i++)
	{
		if ( (ret = extract( incoming[i], &pData, ByteCnt)) != RC_SUCCESS)
		{
			fprintf(stderr, "Cmd 11 tag extraction error.\n");
			return RC_DONOTSEND;
		}
		else // it came out OK
		if ( incoming[i] != NONvolatileData.tag[i])
		{
			fprintf(stderr, "Cmd 11 tag mismatch.tag:0x%02X, cmp:0x%02X\n", NONvolatileData.tag[i], incoming[i]);
			return RC_DONOTSEND;// not our tag
		}
		//else nop...// still good and equal, keep extracting
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_11::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{	
	if (insert_Ident(ByteCnt, pData))
	{
		fprintf(stderr, "Cmd 11 tag insert error.\n");
		return RC_DONOTSEND;// there are no valid RC for this command (except success)
	}
};

#endif // CMD_11_H_
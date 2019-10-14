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
 * #include "cmd_52.h"
 */

#ifndef CMD_52_H_
#define CMD_52_H_

#include "command.h"
#include "InsertExtract.h"

extern void zeroDV(uint8_t dv2Bzeroed); // in PV.cpp

class cmd_52 : public cmd_base
{
	uint8_t dv2Bzeroed;

public: // c.dtor
	cmd_52():cmd_base(52) {	};
	~cmd_52(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_52::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	// write pv units (1 byte)
	if (ByteCnt < 1)
	{
		ByteCnt = 0;
		return RC_TOO_FEW;// too few data bytes
	}

	ret = extract(dv2Bzeroed, &pData, ByteCnt);

	if (ret)
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	else
	if (deviceVar::devVarPtr(dv2Bzeroed) == NULL)// unsigned, can't be < 0
	{
		return RC_INVALID;// in valid selection
	}
	else
	{
		zeroDV(dv2Bzeroed);
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_52::insert_Data(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;
	ByteCnt = 0; // just in case

	ret = insert(dv2Bzeroed, &pData, ByteCnt);

	if (ret)
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_52_H_
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
 * #include "cmd_95.h"
 */

#ifndef CMD_95_H_
#define CMD_95_H_

#include "command.h"
#include "appconnector.h"

extern AppConnector<AppPdu> *pAppConnector;

class cmd_95 : public cmd_base
{

public: // c.dtor
	cmd_95():cmd_base(95) {	};
	~cmd_95(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_95::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// no extraction data

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_95::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	uint16_t stx, ack, bak, snt;
	snt = pAppConnector->getStats(stx, ack, bak);
	do // once
	{
		ret = insert(stx, &pData, ByteCnt);// InsertExtract.cpp
		if ( ! ret )
		ret = insert(ack, &pData, ByteCnt);// InsertExtract.cpp
		if ( ! ret )
		ret = insert(bak, &pData, ByteCnt);// InsertExtract.cpp		
	}
	while(false);// execute once

	if ( ret )// not that insert() only returns zero
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_95_H_
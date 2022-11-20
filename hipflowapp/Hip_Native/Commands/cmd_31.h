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
 * #include "cmd_31.h"
 */

#ifndef CMD_31_H_
#define CMD_31_H_

#include "command.h"


class cmd_31 : public cmd_base
{

public: // c.dtor
	cmd_31():cmd_base(31) {	};
	~cmd_31(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_31::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	// if we are here, then we have a comand 31, no expanded command number
	uint8_t ret = RC_TOO_FEW;
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_31::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
};

#endif // CMD_31_H_
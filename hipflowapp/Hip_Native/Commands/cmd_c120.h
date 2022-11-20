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
 * Description: This holds the command of the test system..it is never responded to
 *
 * #include "cmd_c120.h"
 */

#ifndef CMD_120_H_
#define CMD_120_H_

#include "command.h"
#include "cmd_00.h"


// global test info
char testName[15];
float testNumber = 0; // .0 is none, .1 is xxxa, .2 is xxxb etc

class cmd_120 : public cmd_base
{

public: // c.dtor
	cmd_120():cmd_base(120) {	};
	~cmd_120(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error, or DO-NOT-SEND
// cmd-120 has NO valid responde codes sow we have SUCCESS or DO-NOT-SEND
uint8_t cmd_120::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	if (ByteCnt > 10)
	{	
		char number[5];
		int numcnt = 0;
		uint8_t ret = 0;
		testNumber = 0.0;
		testName[0]='\0';

		ByteCnt+= 3;
		pData  += 3;

		for ( int i = 0; i < 10; i++)
		{
			if ( (ret = extract(testName[i], &pData, ByteCnt)) != RC_SUCCESS)
			{
				fprintf(stderr, "Cmd 120 tag extraction error.\n");
				return RC_DONOTSEND;
			}
			else // it came out OK
			if (testName[i] >= 0x30 && testName[i] <= 0x39 )
			{
				number[numcnt++]=testName[i];
				number[numcnt]='\0';
			}
			else // it came out OK & is not a number
			if (numcnt == 3 && (testName[i] >= 0x61 && testName[i] <= 0x69))
			{
				int t = atoi(number);
				testNumber = t;
				testNumber += ((testName[i] - 0x60) * 0.10);
				break;
			}
			else// it came out OK & is not a number nor a letter
			if (numcnt == 3 && testName[i] == 0x2E )
			{
				int t = atoi(number);
				testNumber = t;
				break;
			}
			//else - just part of the name
			testName[i+1]='\0';
		}
		fprintf(stderr, ">>>>        Begin Test # %5.1f\n", testNumber);
if (NONvolatileData.devVars[0].updatePeriod > 0xc80)printf("Value = %d pre-test.\n", NONvolatileData.devVars[0].updatePeriod);

	} // else either test exit or error
	return RC_DONOTSEND;
}


// generate reply
uint8_t cmd_120::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{	
	return RC_DONOTSEND;// in case we get called
};

#endif // CMD_120_H_
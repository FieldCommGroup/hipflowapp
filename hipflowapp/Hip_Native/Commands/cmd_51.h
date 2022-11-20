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
 * #include "cmd_51.h"
 */

#ifndef CMD_51_H_
#define CMD_51_H_

#include "command.h"

 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  *
  *  Command is not used - Mappable PV is not yet implemented
  *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class cmd_51 : public cmd_base
{

public: // c.dtor
	cmd_51():cmd_base(51) {	};
	~cmd_51(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_51::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	do // once
	{
		//  if there is less than 4 requests, DO NOT SET too-few-bytes-recieved....it's a V5 host
		if (ret = PVidx.extractSelf(&pData, &ByteCnt)) break;
		if (NONvolatileData.devVar_Map[0] != 0 )
		{
			NONvolatileData.devVar_Map[0] = 0;// non-mappable
			printf( "Data error. idx0 = %d...Invalid\n", NONvolatileData.devVar_Map[0]);
			ret = 2;//Invalid Selection error
			ByteCnt = 0;
		}
		if (ByteCnt != 0)// ie it's not a version 5 host
			ret = SVidx.extractSelf(&pData, &ByteCnt); 
		if (NONvolatileData.devVar_Map[1] != 1 )
		{
			NONvolatileData.devVar_Map[1] = 1;// not mappable
			printf( "Data error. idx1 = %d...Invalid\n", NONvolatileData.devVar_Map[1]);
			ret = 2;//Invalid Selection error
			ByteCnt = 0;
		}
		if (ByteCnt != 0)// ie it's not a version 5 host
			ret = TVidx.extractSelf(&pData, &ByteCnt);
		if (NONvolatileData.devVar_Map[2] != 0 && NONvolatileData.devVar_Map[2] != 2)
		{
			printf( "Data error. idx1 = %d...Invalid\n", NONvolatileData.devVar_Map[1]);
			ret = 2;//Invalid Selection error
			ByteCnt = 0;
		}
		// we only support 2 channels, dump the rest
	} while (false);// execute once
	
	if (RC_SUCCESS == ret)
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_51::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if (ret = PVidx.insertSelf(&pData, &ByteCnt)) break;
		if (ret = SVidx.insertSelf(&pData, &ByteCnt)) break;
		if (ret = TVidx.insertSelf(&pData, &ByteCnt)) break;
		*pData++ = 250; ByteCnt++;
	}
	while(false);// execute once

	if ( ret )
	{
		printf("Data insertion error. ret = %d.\n", ret);
		ret     = RC_DEV_SPEC;//Device Specific Command error
		ByteCnt = 0;
	}

    return ret;
};

#endif // CMD_51_H_
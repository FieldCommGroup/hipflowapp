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
 * #include "cmd_48.h"
 */

#ifndef CMD_48_H_
#define CMD_48_H_

#include "command.h"
#include "hartmdll.h"


class cmd_48 : public cmd_base
{
protected:
	bool mismatch;

public: // c.dtor
	cmd_48():cmd_base(48) {	};
	~cmd_48(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

	void update_MoreStatus(uint8_t * pData);
};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_48::extractData(uint8_t &ByteCnt, uint8_t *pData)
{// wally says to assume a HART 7 host in this project
 // real world ain't like that
	uint8_t  ret = RC_SUCCESS;
	if (ByteCnt == 0)// legacy request (transaction 1)
		return ret; // success, nothing to do here
	uint8_t  requestSize = ByteCnt;
	if (requestSize > CMD48_SIZE)
		requestSize = CMD48_SIZE;
	if (requestSize < CMD48_SIZE)
		ret = RC_TOO_FEW;// by spec

uint8_t * pcmd48data; // actually an array

	if (fromPrimary)
	{
		pcmd48data = tempData.Pricmd48TmpData;
	}
	else
	{
		pcmd48data = tempData.Seccmd48TmpData;
	}

	int y = 0;
	while ( (requestSize > 0) && (ret == RC_SUCCESS) )
	{// pData is incremented and requestSze is decremented in extract
		ret = extract(pcmd48data[y++], &pData, requestSize);
		y++;
	}

	if ( ret == RC_SUCCESS )
	{
		update_MoreStatus(pcmd48data);
	}

	return ret;// the response is the same for both transactions
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_48::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		/*for ( int y = 0; y < CMD48_SIZE && ret == RC_SUCCESS; y++ )
		{
			ret = insert(volatileData.cmd48Data[y], &pData, ByteCnt);
		}*/
		//we use 0 and EFDS, everything else is zero
		ret = insert(volatileData.cmd48Data[0], &pData, ByteCnt);
		if (ret) return ret;
		for (int y = 1; y < 6; y++)
		{
			*pData++ = (uint8_t)0; ByteCnt++;
		}
		ret = extended_fld_dev_status.insertSelf(&pData, &ByteCnt) ;
		if (ret) return ret;
		*pData++ = (uint8_t)0; ByteCnt++;
		*pData++ = (uint8_t)0; ByteCnt++;

		/* * * *   volatileData.cmd48Data[] 0 - 13
		0 - 5	Bits or Enum Only	Device-Specific Status (refer to appropriate device-specific document for detailed information)
		6	Bits	Extended Device Status (refer to Common Table 17, Extended Device Status Information)
		7	Bits	Device Operating Mode (refer to Common Table 14, Operating Mode Codes)
		8 	Bits	Standardized Status 0 (refer to Common Table 29)
					1:aDeviceVar-Sim-Active - ExDevStat - FuncCheck
					2:NVM fail              - ExDevStat - Failure
					4:RAM fail              - ExDevStat - Failure
					8:WatchDog Triggered    - ExDevStat - Failure
					10:PwrSupOutOfRange     - ExDevStat - OutOfSpec...etc
		9	Bits	Standardized Status 1 (refer to Common Table 30)
		10	Bits	Analog Channel Saturated  (refer to Common Table 27)
		11	Bits	Standardized Status 2 (refer to Common Table 31)
		12	Bits	Standardized Status 3 (refer to Common Table 32)
		13	Bits	Analog Channel Fixed (refer to Common Table 28)

		14 - 24	Bits or Enum Only	Device-Specific Status (refer to appropriate device-specific document for detailed information)
		* * * */
	}
	while(false);// execute once

	if ( ret )// note that insert always returns zero
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}
	if (mismatch)// from extraction
		ret = RC_WARN_14;

    return ret;
};


void cmd_48::update_MoreStatus(uint8_t * pData)// pdata is an array of cmd 48 data extracted
{
	bool match = true;
	for ( int k = 0; k < 14; k++)
	{
		if (pData[k] != volatileData.cmd48Data[k] )
		{
			match = false;
			break; // outa for loop
		}
	}
	if ( match )
	{// clear MSA bit
		mismatch = false;
		if ( fromPrimary ) Pri_MSA(false);
		else               Sec_MSA(false);
		// Device Status is set in NativeApp::handle_device_message(AppPdu *pPDU)
	}
	else // - leave 'em alone'
	{
		mismatch = true;
	}
};

#endif // CMD_48_H_
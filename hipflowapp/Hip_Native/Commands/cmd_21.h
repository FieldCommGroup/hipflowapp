/*************************************************************************************************
 *
 * Workfile: cmd_21.h
 * 14Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_21.h"
 */

#ifndef CMD_21_H_
#define CMD_21_H_

#include "command.h"


class cmd_21 : public cmd_base
{

public: // c.dtor
	cmd_21():cmd_base(21) {	};
	~cmd_21(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_21::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;

	if ( templongTag.extractSelf( &pData, &ByteCnt, chngCnt) )
	{
		ret = RC_DONOTSEND;// no valid response code
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, DO-NOT-SEND on error (there are no valid response codes except success)
uint8_t cmd_21::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	bool isEqual = true;
	for ( int j = 0; j < 32; j++)
	{
		if ( ((uint8_t*)(templongTag.pRaw))[j] != ((uint8_t*)(longTag.pRaw))[j]  )
		{
			isEqual = false;
			break;// out of for loop 
		}// else it'll 
	}
	if (isEqual )// our address 
	{
		if (insert_Ident(ByteCnt, pData))// a zero packet
		{
			ret = RC_DONOTSEND;
		}
	}
	else// mismatch 
	{
		ret = RC_DONOTSEND;
	}

    return ret;
};

#endif // CMD_21_H_
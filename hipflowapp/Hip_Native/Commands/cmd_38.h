/*************************************************************************************************
 *
 * Workfile: cmd_38.h
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
 * #include "cmd_38.h"
 */

#ifndef CMD_38_H_
#define CMD_38_H_

#include "command.h"


class cmd_38 : public cmd_base
{
	bool isVer5Host;

public: // c.dtor
	cmd_38():cmd_base(38) {	};
	~cmd_38(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_38::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;
	bool OK2Clear = false;

	// insert extraction here
	if ( ByteCnt == 0 ) // version 5 master
	{
		OK2Clear  = true;
		isVer5Host= true;
	}
	else
	{
		isVer5Host = false;
		ret = tempconfigChangeCnt.extractSelf( &pData, &ByteCnt, chngCnt);
		if ( ret == 0 &&   *((uint16_t*)(tempconfigChangeCnt.pRaw)) == *((uint16_t*)(configChangeCnt.pRaw))  )
		{
			OK2Clear = true;
		}
		else
		if ( ret == 0)
		{
			OK2Clear = false;
			ret = RC_CFGMISMATCH;
		}
		// else too few data bytes error
	}
	if (OK2Clear)
	{
		if (  fromPrimary || isVer5Host)
		{
			configChangedBit_Pri = false;// clear primary flag
		}
		// has to be secondary
		if ( (!fromPrimary) || isVer5Host)
		{
			configChangedBit_Sec = false; // clear secondary flag
fprintf(stderr, "        SecCCB clr %s.\n",(isVer5Host)?"Version5":"NotV5"); fflush(stderr);
		}
	}// else do nothing, leave everything as found

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_38::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	// latest spec says return the config counter regardlwss of host type
	//if (isVer5Host)
	//{
	//	return RC_SUCCESS;// no response here
	//}
	//ret = tempconfigChangeCnt.insertSelf( &pData, &ByteCnt ) ;
	ret = configChangeCnt.insertSelf( &pData, &ByteCnt ) ;

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_38_H_
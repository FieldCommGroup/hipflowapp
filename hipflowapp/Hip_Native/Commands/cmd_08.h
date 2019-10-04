/*************************************************************************************************
 *
 * Workfile: cmd_08.h
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
 * #include "cmd_08.h"
 */

#ifndef CMD_08_H_
#define CMD_08_H_

#include "command.h"


class cmd_08 : public cmd_base
{

public: // c.dtor
	cmd_08():cmd_base(0x08) {	};
	~cmd_08(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_08::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;

	// insert extraction here

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_08::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
	// units followed by float value
		if ( ret = devVar_PV.classification.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = devVar_SV.classification.insertSelf( &pData, &ByteCnt )  ) break;
		if ( ret = devVar_TV.classification.insertSelf( &pData, &ByteCnt )  ) break;
		// latest - unpublished - spec127 specifies a dev var that is unsupported must still
		// return 'Unclassified' for the device family
		//ret = insert( HART_NOT_USED, &pData, ByteCnt );//QV
		ret = insert(HART_NOT_CLASFD, &pData, ByteCnt);//QV
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_08_H_
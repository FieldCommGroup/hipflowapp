/*************************************************************************************************
 *
 * Workfile: cmd_95.h
 * 11Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
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
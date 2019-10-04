/*************************************************************************************************
 *
 * Workfile: cmd_11.h
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
 * #include "cmd_11.h"
 */

#ifndef CMD_11_H_
#define CMD_11_H_

#include "command.h"
#include "cmd_00.h"


class cmd_11 : public cmd_base
{

public: // c.dtor
	cmd_11():cmd_base(11) {	};
	~cmd_11(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error, or DO-NOT-SEND
// cmd-11 has NO valid responde codes sow we have SUCCESS or DO-NOT-SEND
uint8_t cmd_11::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	uint8_t incoming[6];
	if (ByteCnt < 6)
	{
		fprintf(stderr,"Cmd 11 byte count error\n");
		return RC_DONOTSEND;
	}
	
	for ( int i = 0; i < 6; i++)
	{
		if ( (ret = extract( incoming[i], &pData, ByteCnt)) != RC_SUCCESS)
		{
			fprintf(stderr, "Cmd 11 tag extraction error.\n");
			return RC_DONOTSEND;
		}
		else // it came out OK
		if ( incoming[i] != NONvolatileData.tag[i])
		{
			fprintf(stderr, "Cmd 11 tag mismatch.tag:0x02X, cmp:0x02X\n", NONvolatileData.tag[i], incoming[i]);
			return RC_DONOTSEND;// not our tag
		}
		//else nop...// still good and equal, keep extracting
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_11::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{	
	if (insert_Ident(ByteCnt, pData))
	{
		fprintf(stderr, "Cmd 11 tag insert error.\n");
		return RC_DONOTSEND;// there are no valid RC for this command (except success)
	}
};

#endif // CMD_11_H_
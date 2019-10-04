/*************************************************************************************************
 *
 * Workfile: command.cpp
 * 10Dec2018 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		This is the command base class, there will be one for each command.
 *
 */

#include "command.h"
#include "PV.h"
 
cmd_base::cmd_base(int num) :  cmdNumber(num)
{
}


cmd_base::~cmd_base()
{
}

// return a return-code , 
int cmd_base::handleCmd(AppPdu *pPDU) 
{ 
	int returnValue = RC_SUCCESS;
	uint8_t  rtnCd  = RC_SUCCESS, inRC = RC_SUCCESS, dataBC = 0, *pData = NULL;
	responseCode.setValue(rtnCd); // rtnCd is zero now, success

	pData  = pPDU->DataBytes();
	dataBC = pPDU->RequestByteCount();
	rtnCd = extractData(dataBC, pData);
	dataBC = 0;// in case we had an error or, prep for insertion
	fromPrimary = pPDU->isPrimary();

	pPDU->msg2Ack(); // convert to reply pkt for all commands
	if (rtnCd == RC_DONOTSEND)
	{
		returnValue = rtnCd;// do nothing, return the 0x80, BC still 0
	}
	else
	if ( isError(rtnCd) )//   an error return code
	{
		printf("Data extraction error. Cmd %d; Return code = %d.\n", pPDU->CmdNum(), rtnCd);
		// do this in callerpPDU->clear();
		returnValue = rtnCd;// BC still 0
	}
	else // success or warning
	{
		pData  = pPDU->DataBytes();// bc is zero, since masg2Ack, this is rawdata+2
		inRC = insert_Data(dataBC, pData);
		if (inRC == RC_DONOTSEND)
		{
			returnValue = inRC;// do nothing, return the 0x80, BC still 0
		}
		else
		if ( isError(inRC) )// insertion error
		{
			dataBC = 0; // do not return data with error
			returnValue = inRC;// be sure to return it
		}
		else
		if (inRC == 0 && rtnCd != 0 )// inserted SUCCESS but extract had a warning
		{
			returnValue = rtnCd;
		}
		else // both success,both warning-use insert's, extract success & insert warning-use insert's
		{
			returnValue = inRC;
		}
	}

	// add 2 for RC/DS (ie from RequestByteCount to ResponseByteCount)
	pPDU->SetByteCount(dataBC + 2);// insert should have zeroed BC on error
	return returnValue;
}


// set the variable, return a return-code
int cmd_base::setRespCode(uint8_t RespCode) 
{
	responseCode.setValue(RespCode);
	return aReturnCode( RespCode );
}


/*  returns 
		RC-17: DV not in device
		RC-19: DV is not a totalizer
		RC-0 : DV is one of the totalizer Device Variables 

	NOTE:  STANDARD DEVICE VARIABLES HAVE TO BE ADDED HERE
*/
uint8_t cmd_base::isDevVarIndex2Totalizer(uint8_t qDVarIndex)
{
	if (qDVarIndex < 3)// query DV --- add standard handling here
	{
		if ( IS_TOTALIZER(qDVarIndex) )
		{ 
			return RC_SUCCESS;
		}
		else // valid DV but not a totalizer
		{
			return RC_INDEX_DISALLOWED;
		}	
	}
	else // not in this device
	{
			return RC_BAD_INDEX;
	}
}

// this routine is to only fill the data
// commands  that need the index list must override this,
// fill the proper index and then call this to do the inssert.
// then do any clean up required
// address & delimiter must be set BEFORE entring this routine

uint8_t cmd_base::burstThisCmd(dataItem indexList[], AppPdu *pPDU)
{
	uint8_t returnValue = RC_SUCCESS;
	uint8_t rtnCd = RC_SUCCESS;
	responseCode.setValue(rtnCd); // rtnCd is zero now, success

	fromPrimary = pPDU->isPrimary();

	//not needed for the burst packet...	pPDU->msg2Ack(); // convert to reply pkt for all commands-turns OFF burst bit

	uint8_t dataBC = 0;
	uint8_t *pData = pPDU->DataBytes();// bc is zero, since masg2Ack, this is rawdata+2	
	
	
	rtnCd = insert_Data(dataBC, pData);
	if (rtnCd == RC_DONOTSEND)
	{
		returnValue = rtnCd;// do nothing, return the 0x80, BC still 0
	}
	else
	if (isError(rtnCd))// insertion error
	{
		dataBC = 0; // do not return data with error
		returnValue = rtnCd;// be sure to return it
	}
	else
	{
		returnValue = rtnCd;
	}	

	// add 2 for RC/DS (ie from RequestByteCount to ResponseByteCount)
	pPDU->SetByteCount(dataBC + 2);// insert should have zeroed BC on error

	return returnValue;
}

void cmd_base::triggerOnChanged(dataItem indexList[MAX_CMD_9_RESP])
{
	// stubbed out for now
}

bool cmd_base::isTriggered()
{
	// stubbed out for now
	return false;
}

// use this method to convert device var values to desired units for display only
// the values are always maintained in standard units (kg/hr)
int cmd_base::convertAndInsert(deviceVar &dvar, uint8_t **ppData, uint8_t *dataCnt)
{
	return convertAndInsert(dvar.Value, dvar, ppData, dataCnt);
}

// this method covers the cases where the value is other than .Value (upperRaneValue for instance)
int cmd_base::convertAndInsert(dataItem &val, deviceVar &unitsVar, uint8_t **ppData, uint8_t *dataCnt)
{
	// retrieve volatile data value with semaphore protection
//	getSema();
	float x = ItemValue(val, float);
//	giveSema();

	// convert it from standard units to desired units
	int U = ItemValue(unitsVar.Units, uint8_t);
	x = unitsVar.UnitSet[U].frmStandard(x);

	// insert the converted float into the response
	return insert(x, ppData, *dataCnt);
}

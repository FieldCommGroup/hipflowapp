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
 * Description:
 *		This is the command base class, there will be one for each command.
 *
 */

#include "command.h"
#include "PV.h"
#include "burst.h"
 
cmd_base::cmd_base(int num) :  cmdNumber(num), fromPrimary(true)
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
	uint16_t cmdNum = 0;
	responseCode.setValue(rtnCd); // rtnCd is zero now, success

	cmdNum = pPDU->CmdNum();
	pData  = pPDU->RequestBytes();
	dataBC = pPDU->RequestByteCount();
	fromPrimary = pPDU->isPrimary();
	rtnCd = extractData(dataBC, pData);
	dataBC = 0;// in case we had an error or, prep for insertion

	//if (! fromPrimary)
	//	printf("        Secondary cmd Rcv'd. Cmd %d\n", pPDU->CmdNum());

	pPDU->msg2Ack(TPDELIM_FRAME_STX, isBursting());  // convert to reply pkt for all commands
	cmdNum = (cmdNum <= 255) ? pPDU->CmdNum() : cmdNum; // #39
	if(pPDU->CmdNum1Byte() == HART_CMD_EXP_FLAG && pPDU->ByteCount() < 2)
	{
		// command 31 but no extended command number followsa
		rtnCd = RC_TOO_FEW;
	}
		
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
		pData  = pPDU->ResponseBytes();// bc is zero, since masg2Ack, this is rawdata+2
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

	pPDU->setCommandNumber(cmdNum);
	
	bool isError = (dataBC == 0);		// insert should have zeroed BC on error
	dataBC += 2;						// RC + DS bytes  #36
	dataBC += pPDU->IsExpCmd() ? 2 : 0;	// add 2 for expanded command number is included in the data field
	dataBC += (isError && pPDU->IsExpCmd()) ? 2 : 0; // #27
	pPDU->SetByteCount(dataBC);	
	
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

bool cmd_base::isTriggered(burstMessage & bMsg)//enum 0=continuous,1=Window,2=Rising,3=Falling;4=AnyChangeInMsgVars
{// for non-burst commands...
	printf("Cmd Base isTriggered.\n");
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

/*************************************************************************************************
 *
 * Workfile: DataObjectBase.h
 * 16May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds the raw data interface definition
 *
 *
 * #include "DataObjectBase.h"
*/
#pragma once

#ifndef _C_DATAOBJECT_BASE_H
#define _C_DATAOBJECT_BASE_H
#ifdef INC_DEBUG
#pragma message("In DataObjectBase.h") 
#endif

#include <tuple>
#include "../hipserver/hipserver/Common/datatypes.h"
#include "varient.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::DataObjectBase.h") 
#endif


//. pure virtual base class
class dataObject_Base
{
public:
	virtual uint8_t			extractSelf(uint8_t **ppData, uint8_t& dataCnt) = 0;
	virtual uint8_t			insertSelf (uint8_t **ppData, uint8_t& dataCnt) = 0;
	virtual std::tuple<uint8_t*, int> 
							saveSelfInfo() = 0;//  returns tuple of data ptr, length 
	virtual void            restoreSelf(uint8_t*, int) = 0;
	virtual void			setSelfValue(CValueVarient&) = 0;
	virtual bool            getIsConfig(void) = 0;
	virtual CValueVarient	getSelfValue(void) = 0;
	virtual hartTypes_t		getSelfType(void) = 0;
	// may need virtual int        		getSelfSize(void) = 0;
};

#endif // _C_DATAOBJECT_BASE_H
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
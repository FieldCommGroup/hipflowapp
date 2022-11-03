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
 * Description: This holds the configuration interface
 *
 *
 * #include "DataObjectConfig.h"
*/
#pragma once

#ifndef _C_DATAOBJECTCONFIG_H
#define _C_DATAOBJECTCONFIG_H
#ifdef INC_DEBUG
#pragma message("In DataObjectConfig.h") 
#endif

#include "DataObjectConfig.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::DataObjectConfig.h") 
#endif

// this is the configuration interface for the data objects
class dataObjectConfig
{
public: // ctor / dtor
	virtual void do_Configure(bool is_Config) = 0;
	virtual void do_ConfigureArray(bool is_Config, uint8_t datasize) = 0;
};

#endif // _C_DATAOBJECTCONFIG_H
/*************************************************************************************************
 *
 * Workfile: DataObjectConfig.h
 * 17May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
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
/*************************************************************************************************
 *
 * Workfile: variableFactory.h
 * 20May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds the declaraion of the variable factory class.
 *
 *
 * #include "variableFactory.h"
*/
#pragma once

#ifndef _C_INT8_H
#define _C_INT8_H
#ifdef INC_DEBUG
#pragma message("In variableFactory.h") 
#endif

#include "PrimativeDataDesc.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::variableFactory.h") 
#endif

struct descriptor {
	DESC_TYPE
};

static descriptor variableDescs[]= DATA_DESC;

class variableFactory
{
};

#endif // _C_INT8_H
/*************************************************************************************************
 *
 * Workfile: DataSet.h
 * 17May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds definition of the central database.
 *
 *
 * #include "DataSet.h"
*/
#pragma once

#ifndef _C_DATASET_H
#define _C_DATASET_H
#ifdef INC_DEBUG
#pragma message("In DataSet.h") 
#endif

#include "AllDataObjects.h"
#include <map>

using namespace std;

#ifdef INC_DEBUG
#pragma message("    Finished Includes::DataSet.h") 
#endif


class dataset
{	// The key is the string of the variables name, it will be used in command
	// configurations to get a pointer to a datum.  
	// Once Filled, this should never change content.
	map<string, dataObject*> allData;

public:
	dataset(){};
	virtual ~dataset() {};// delete and clear

public:
	dataObject* getDataObject(string varName);
	int         setDataObject(string varName, dataObject*);

};

/
#endif // _C_DATASET_H
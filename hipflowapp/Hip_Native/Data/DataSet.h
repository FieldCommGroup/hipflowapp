/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
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
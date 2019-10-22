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
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
 * Description:
 *		3/16/18	pdu	created to deal with multiple files and permissions
 *	
 * #include "Files.h"
 */
#pragma once

#ifndef _NATIVEFILES_H
#define _NATIVEFILES_H
#ifdef INC_DEBUG
#pragma message("In Filess.h") 
#endif

#include <string>
#include <stdio.h>

#ifdef INC_DEBUG
#pragma message("    Finished Includes::Filess.h") 
#endif

using namespace std::__cxx11;

extern void nop();

#ifdef _DEBUG
	#define bcmLOGIT( C... )   printf( C )
#else
	#define bcmLOGIT( C... )   nop()
#endif

#define  NATIVE_RANGES_FILE   "/var/data/NativeDevice/ranges"
#define  PI_MEMORY_PATH       "/dev/mem"

FILE* openSystemFile( string& dataFileSpec, string& systemFileSpec );




#endif  // _NATIVEFILES_H
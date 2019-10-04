/*************************************************************************************************
 *
 * $Workfile: Files.h $
 * 16Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
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
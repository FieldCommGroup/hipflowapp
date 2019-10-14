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
 *************************************************************************************************
 *
 * Description: This holds the common data for PV, SV, TV, QV
 *	
 *	
 * #include "cSerialFile.h"
*/
#pragma once

#ifndef _C_SERIALFILE_H
#define _C_SERIALFILE_H
#ifdef INC_DEBUG
#pragma message("In cSerialFile.h") 
#endif

#include "datatypes.h"
#include "errval.h"
#include <sys/stat.h>
#include <stdio.h>
#include <semaphore.h>
#include <string>

using namespace std;

#ifdef INC_DEBUG
#pragma message("    Finished Includes::cSerialFile.h") 
#endif


// ~/.my-app/...			expands to /home/username/.my-app/…			

// we only have one file, let's not complicate things..#define CONFIG_DIRECTORY  "~/nativedev"
// this apparently cannot use use the '~' capability
//#define CONFIG_FILENAME   "~/projects/nativedev/nativedevice.conf"
//#define CONFIG_FILENAME   "/root/projects/nativedev/nativedevice.conf" /* this works */
#ifdef _FLOWDEVICE
 #define CONFIG_FILENAME      "/etc/nativedev/nativeFlow.conf"
#else
  #ifdef _PRESSUREDEVICE
   #define CONFIG_FILENAME   "/etc/nativedev/nativePres.conf"
  #else
   #define CONFIG_FILENAME   "/etc/nativedev/nativeUnkn.conf"
  #endif
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*	serializationFile class
*
*	Designed to be a static base class for all the data files.
*/
class serializationFile
{
	static sem_t  fileSemaphore;// only one file gets to write at a time

	FILE * pFD; // file Descriptor - Use Close_File() to close and NULL this
	string sFilespec;
	
public: // ctor/dtor
	serializationFile();
	~serializationFile();

public:
int Open_File(char* filespec); //  returns 0 on existing file ready
							   //          1 on didn't exist, we made it, it's empty but open
							   //         -1 on doesn't exist and we failed to make it
void Close_File(void);
bool CheckFile(char* filespec); // true on exists
errVal_t getData(void);// non volatile only
errVal_t putData(void);

protected: // helpers
	int make_file(char* filespec );
};


#endif //_C_SERIALFILE_H

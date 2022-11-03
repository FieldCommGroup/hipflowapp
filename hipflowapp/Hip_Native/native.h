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
 *****************************************************************/

#ifndef NATIVE_H_
#define NATIVE_H_

#include "errval.h"
#include "cDatabase.h"
#include "cSerialFile.h"



class NativeData : public serializationFile
{
	// these used only by send_recv()
	/*DEVDATA vDeviceData;
	BYTE vacCommand[ HART_MAX_MSG_LEN];
	BYTE vacResponse[ HART_MAX_MSG_LEN];*/

	//int poll_x_Address;// use dataItem
	uint8_t myAddr[5];

public:
	NativeData();
	virtual ~NativeData();

	//void setPort(const char *p);

	errVal_t commandline(int argc, char *argv[]);  // -p <port>

	// read a file and/or set static data items
	errVal_t configure();

	// programatically set data, semaphores, spin threads, etc
	errVal_t initialize();

	errVal_t fileFillDefault();// fill all non volatile with default values
	
/***
	// poll bus 0..n to find first device
	errVal_t findDevice(void);

	errVal_t identifyDevice(void);
	errVal_t send_receive(uint8_t cCmd);
	errVal_t sendRequest(uint8_t* pcId, uint8_t* request, uint32_t wLength );
	errVal_t readResponse(uint8_t cId, uint8_t* response );
	errVal_t readBurst(uint8_t* response );	// poll for most recent
***/
	// stop threads, delete semaphores and allocated memory
	static errVal_t cleanup();
	void GenerateMyAddress();
	void handleDataDependencies();
	uint8_t *getMyAddr() { return &(myAddr[0]); };// length 5
	void setUniqueID(uint8_t *pLowMACaddr);
};

#endif /* NATIVE_H_ */

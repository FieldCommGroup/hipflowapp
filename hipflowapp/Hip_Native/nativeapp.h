/*************************************************************************************************
 *
 * Workfile: app.h
 * 20Apr18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This class encapsulates the operation of the HART-IP device
 *
 * #include "nativeapp.h"
 */

#ifndef NATIVEAPP_H_
#define NATIVEAPP_H_

#include "app.h"
#include "errval.h"
#include "appconnector.h"
#include "native.h"
#include "commandSet.h"
#include <string>

extern AppPdu *pBurstPdu;
extern AppConnector<AppPdu> *pAppConnector;
//extern App *pGlobalApp;
//  declared after the class below ...extern NativeApp *pGlobalApp;

using namespace std;

class NativeApp: public App
{
	string appFullName;
	cmdSet commandList;// all supported commands
	uint32_t deviceBurstCnt = 0;
	bool   deviceISbursting = false;

public:
	NativeApp(const char *name, const char *ver) : App(name, ver, HART_IP)//  native app...
	{
		configured = false;
		appFullName = "";// will have full path
	}
	;
	virtual ~NativeApp()
	{
	}
	;

	NativeData devicedata;

	/*
	 * each HART-IP application will derive a class from App that implements these methods
	 */

	// parse command line args and set class attributes reflecting them
	virtual errVal_t commandline(int argc, char *argv[]);

	// read a file and/or set static data items
	virtual errVal_t configure();

	// programatically set data, semaphores, spin threads, etc
	virtual errVal_t initialize();

	//This will be called when we receive a INIT_APP_CMD from the server.
	//	If it returns false, the interface will discard the INIT CMD and allow the server to time out.
	//	If the application has a serious error where it can never be 'ready', it should call the
	//	abortAPP() function shortly before returning false.
	virtual errVal_t ready();

	//		This will be called when we receive a TERM_APP_CMD from the server.
	//	The return value is disregarded and doesn't matter.  If the abortAPP() function has not been
	//	called by this function, the AppConnector will call it immediately after this function returns.
	//	That means that the application is assumed to be shutdown when this function is returned,
	//	all threads terminated and all memory freed. runInfc will return soon after this function returns.
	virtual errVal_t stop();

	//		This will be called when we receive a HART_APP_CMD from the server.
	//	This will evaluate the message packet and, if not addressed to this device, will return NOT_OURS.
	//  When it is addressed to this device, the msgFunc will handle the message, fill the AppPdu with
	//  reply data and address, and return a value of ALL_IS_WELL.  This class will then send the msg
	//	packet to the server.  It is up to the msgFunc to handle errors and form the correct error
	//  response packet, returning ALL_IS_WELL to this class to enable the error packet to be sent.
	virtual int handleMessage(AppPdu *pPDU);

	// stop threads, delete semaphores and allocated memory
	virtual errVal_t cleanup();

	bool configured; // true for successful configuration
	int  handle_device_message(AppPdu *pPDU);
	const char* getFullName(){ return appFullName.c_str(); };
	errVal_t start_system(void); // starts up the data acquasition system
	cmd_base * cmd(uint16_t cNum){ return commandList.getCmd(cNum); };// NULL on bad #
	int burstIt(dataItem indexList[], AppPdu *pPDU);// pdu must have command # and master address
	errVal_t getLowMAC(uint8_t *pArr);
	void incBurstCnt(){ deviceBurstCnt ++; };
	bool isBurstCntOdd() { return ( (deviceBurstCnt & 0x00001) != 0 );  };
	void setBursting(bool is_Bursting){ deviceISbursting  = is_Bursting;};
	bool weAreBursting() { return deviceISbursting;};
};


//extern App *pGlobalApp;
extern NativeApp *pGlobalApp;

#endif /* NATIVEAPP_H_ */

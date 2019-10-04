/*************************************************************************************************
 *
 * Workfile: commandSet.h
 * 11Dec18 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the FieldComm Group
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds all the commands
 *
 * #include "commandSet.h"
 */

#ifndef COMMANDSET_H_
#define COMMANDSET_H_

#include "command.h"

//extern AppPdu *pBurstPdu;
//extern AppConnector<AppPdu> *pAppConnector;
//extern App *pGlobalApp;

using namespace std;

class cmdSet
{
	static cmd_base *commandList[];
	int cmdCount;

public: // c.dtor
	cmdSet();
	~cmdSet();

public: // work
	int count() { return cmdCount; };// unused

	int handleCmd(AppPdu *pPDU);

	cmd_base *getCmd( uint16_t cmdNum );

protected: // helper functions
	int cmdSupported(int cmdNum);
};




#endif // COMMANDSET_H_
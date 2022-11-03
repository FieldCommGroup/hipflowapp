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
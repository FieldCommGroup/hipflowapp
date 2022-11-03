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
 * Description:
 *		This is the start of everything.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "toolsigs.h"
#include "appconnector.h"
#include "errval.h"
#include "apppdu.h"
#include "nativeapp.h"
#include "tooldef.h"
#include "debug.h"
#include "factory_reset.h"

/******************  statics ****************************/
template<> bool AppConnector<AppPdu>::time2stop = false;

/******************  globals ****************************/

// we have a single interface so a global works
AppConnector<AppPdu> *pAppConnector = NULL;
//App *pGlobalApp = NULL;
NativeApp *pGlobalApp = NULL;


/* Signal Handler for SIGINT */
void handle_sigint(int32_t sigNum)
{
	/*
	 * Reset handler to catch SIGINT next time.
	 * Refer http://en.cppreference.com/w/c/program/signal
	 */
	pthread_sigmask(SIG_BLOCK, &newSet, &oldSet);

	fprintf(stderr, "Further SIGINT blocked\n");
	fprintf(stderr, "Got SIGINT\n");

	pAppConnector->abortApp();
	pGlobalApp->cleanup();

	exit(0);
}


int main(int argc, char *argv[])
{
	errVal_t errval = NO_ERROR;

	NativeApp app(TOOL_NAME, TOOL_VERS);
	pGlobalApp = &app;

	AppConnector<AppPdu> globalAppConnector; // ctor sets config, incl address
	pAppConnector = &globalAppConnector;

// #135
#if !defined(__x86_64__)
	  // Initialize GPIO for reset and write_protect (GPIO 2 and GPIO3) both pins are pulled high on PI3B+ board
	  if (gpioInitialise() < 0)
	  {
		printf("WARNING: gpio pin initialization failed. factory_reset or write_protect state may not be correct!\n");
	  }
#endif
	do
	{
		if ((errval = app.commandline(argc, argv)))
		{ // error returned
			printf("Command line processing for %s failed. Exiting!\n",
					app.GetName());
			break;
		}

		// read/set static configuration data
		if ((errval = app.configure()))
		{ // error returned
			printf("Configure %s failed. Exiting!\n", app.GetName());
			break;
		}
		
		if ((errval = app.initialize()))
		{ // error returned
			printf("%s failed to initialize. Exiting!\n", app.GetName());
			break;
		}

		// start working, infinite loop
		printf("hello from %s!\n", app.getFullName());
		pAppConnector->run(&app); // ends on abortApp
		printf("%s is finished. Exiting!\n", app.GetName());
		sleep(5);// seconds
		
	} while (false);

	app.cleanup();
	printf("goodbye from %s!    %d\n", app.GetName(), errval);
	return errval;
}


/*************************************************************************************************
 *
 * Workfile: main.cpp
 * 27Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
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
fprintf(stderr, "-->> Main Entry as version %d\n", SOFTWAREVERSION); fflush(stderr);

	NativeApp app(TOOL_NAME, TOOL_VERS);
	pGlobalApp = &app;
printf("-->> App Instantiated\n");

	AppConnector<AppPdu> globalAppConnector; // ctor sets config, incl address
	pAppConnector = &globalAppConnector;
printf("-->> Appconnector generated\n");

	do
	{
		if ((errval = app.commandline(argc, argv)))
		{ // error returned
			printf("Command line processing for %s failed. Exiting!\n",
					app.GetName());
			break;
		}
printf("-->> Command Line Handled\n");
if (NONvolatileData.devVars[0].updatePeriod > 0xc80)printf("Value = %d pre -config.\n", NONvolatileData.devVars[0].updatePeriod);

		// read/set static configuration data
		if ((errval = app.configure()))
		{ // error returned
			printf("Configure %s failed. Exiting!\n", app.GetName());
			break;
		}
printf("-->> App Configured\n");
if (NONvolatileData.devVars[0].updatePeriod > 0xc80)printf("Value = %d post-config.\n",NONvolatileData.devVars[0].updatePeriod);

		if ((errval = app.initialize()))
		{ // error returned
			printf("%s failed to initialize. Exiting!\n", app.GetName());
			break;
		}
printf("-->> App Initialized\n");
if (NONvolatileData.devVars[0].updatePeriod > 0xc80)printf("Value = %d post-init.\n", NONvolatileData.devVars[0].updatePeriod);

		// start working, infinite loop
		printf("hello from %s!\n", app.getFullName());
		pAppConnector->run(&app); // ends on abortApp
		printf("%s is finished. Exiting!\n", app.GetName());
		sleep(5);// supposed to be seconds
	} while (false);

	app.cleanup();
	printf("goodbye from %s!    %d\n", app.GetName(), errval);
	return errval;
}


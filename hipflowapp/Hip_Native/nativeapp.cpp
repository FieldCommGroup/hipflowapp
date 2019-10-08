/*************************************************************************************************
 *
 * Workfile: app.cpp
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
 */

#include "debug.h"
#include "nativeapp.h"

#include "errval.h"
#include <stddef.h>
#include "burst.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory_glbl.h"
#include <math.h>

extern int kill_burst(void);

// this is in memory_gbl.cpp bool isBIGendian = false; // arm pi

extern int init_phy(void);// in physical.cpp
extern int init_burst(uint8_t myAddr[]); // in burst.cpp
extern int initBurstStack();
extern int kill_phy(void);// in physical.cpp


/***************************************
 *  Private Function Implementations
 ***************************************/

/***************************************
 *  Public virtual App method Implementations
 ***************************************/

// parse command line args and set class attributes reflecting them
errVal_t NativeApp::commandline(int argc, char *argv[])
{
errVal_t errval = NO_ERROR;

// If command line options are present, errval is initialized
// to an error value. It is then set to NO_ERROR only if all
// command line options are found to be valid.
if (argc > 1)
{
	errval = INVALID_INPUT_ERROR;
}
else
{
	appFullName = argv[0];
}
#ifdef _NATIVE_HAS_NO_OPTIONS  /* these are left for an example if options are required later */
for (uint8_t i = 1; i < argc; i++)
{
	char c0 = argv[i][0];

	if (c0 == '-')
	{
		/* Flag error if parameter value after the command line
		 * option is missing or if there is no space between the
		 * option and its value.
		 */
		if (strlen(argv[i]) != 2)
		{
			fprintf(stderr, "\nERROR:\n");
			fprintf(stderr,
				" Invalid command format (missing space or value)\n");
			break;
		}

		char c1 = argv[i][1];

		switch (c1)
		{
		case 'h':
			// -h  Print command usage information
			/* Do nothing for -h option.
			 * This function returns an error value unless specifically
			 * set to NO_ERROR. The calling function (main()) prints the
			 * help menu whenever this function returns a value other
			 * than NO_ERROR.
			 */
			fprintf(stderr, "%s\n\n", GetName());
			fprintf(stderr, "Usage:\n");
			fprintf(stderr, " %s [Option(s)]\n\n", GetName());
			fprintf(stderr, "Options:\n");
			fprintf(stderr, " -h Print command usage information and quit.\n");
			fprintf(stderr, " -v Print version number and quit.\n");
			fprintf(stderr, " -p <path> The default is /dev/ttyS0.\n");
			exit(0);
			break;

		case 'v':

			// app name and version
			printf("%s %s\n", GetName(), GetVersion());
			exit(0);
			break;

		case 'p':
			// -p <path>
			if (++i < argc)
			{
				//strcpy(serialPort, argv[i]);
				//master.setPort(serialPort);
				errval = NO_ERROR;
			}
			break;

		default:
			// Invalid command line option
			fprintf(stderr, "\nERROR:\n");
			fprintf(stderr, " Invalid command line option\n");
			break;
		} // switch (c1)
	} // if (c0 == '-')
	else
	{
		fprintf(stderr, "\nERROR:\n");
		fprintf(stderr, " Invalid command usage\n");
	}

	if (errval != NO_ERROR)
	{
		break;
	}
} // for (uint8_t i = 1; i < argc; i++)
#endif
return (errval);
}

// read a file and/or set static data items
errVal_t NativeApp::configure()
{
	printf("- - - - Native App in Configure.\n");
	// doesn't appear to work ... const float    constant_nan = NaN_value;
	float *pFlt    = const_cast<float*>(&constant_nan);
	uint8_t *a_flt = (uint8_t*)(pFlt);
	float *pFLT    = const_cast<float*>(&(NaN_value));
	uint8_t *a_FLT = (uint8_t*)(pFLT);
	a_flt[0] = a_FLT[0];
	a_flt[1] = a_FLT[1];
	a_flt[2] = a_FLT[2];
	a_flt[3] = a_FLT[3];
	uint8_t  lowMACaddr[4] = { 0,0,0,0 };

	errVal_t retval = devicedata.configure();// fills from file or default data
	if (retval == NO_ERROR)
	{
		retval = getLowMAC(lowMACaddr);
		if (retval == NO_ERROR)
		{
			devicedata.setUniqueID(lowMACaddr);
			devicedata.handleDataDependencies();
		}
		else
		{
			printf("**** MAC address failed::>  Device at default address.\n");
		}
	}
	uint8_t *pAdd = devicedata.getMyAddr();// length 5
	printf("Native Device Address configured as: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		pAdd[0], pAdd[1], pAdd[2], pAdd[3], pAdd[4]);

	return retval;
}

// programatically set data, semaphores, spin threads, etc
errVal_t NativeApp::initialize()
{
	printf("\n\n");
	printf("****** Starting %s %s ******\n\n", appname, appversion);

// BURSTOFF	pBurstPdu = pAppConnector->getNewPDU();		// supposed to copy address
// BURSTOFF	pBurstPdu->processPdu(); // execute stuff like we just received the pdu

	errVal_t errval = devicedata.initialize();// does nothing now

	if (errval == NO_ERROR)
	{	// we start the physical app at startup so it runs regardless of the comm 		
	
		errval = start_system(); // any error means one or more threads would not start
		if ( errval )
		{// the measurement application doesn't exist, no reason for us to.
			fprintf(stderr, "Failed to start the physical i/o\n");
			pAppConnector->abortApp();
			//return sytem's error;
		}
		//else,  ok return no error
	}
	return errval;
}

//This will be called when we receive a INIT_APP_CMD from the server.
//	If it returns false, the interface will discard the INIT CMD and allow the server to time out.
//	If the application has a serious error where it can never be 'ready', it should call the
//	abortAPP() function shortly before returning false.
errVal_t NativeApp::ready()
{// 
	errVal_t errval = NO_ERROR;
	
	// we now start the physical layer data acquisition in initialize()
	// we have nothing else to do at connection time
	// future:
	// we need to verify all is functional and return NO_ERROR or, 
	//                   if non functional, return an error
	
	return errval;
}

//		This will be called when we receive a TERM_APP_CMD from the server.
//	The return value is disregarded and doesn't matter.  If the abortAPP() function has not been
//	called by this function, the AppConnector will call it immediately after this function returns.
//	That means that the application is assumed to be shutdown when this function is returned,
//	all threads terminated and all memory freed. runInfc will return soon after this function returns.
errVal_t NativeApp::stop()
{
	errVal_t errval = NO_ERROR;

	return errval;
}

//		This will be called when we receive a HART_APP_CMD from the server.
//	This will evaluate the message packet and, if not addressed to this device, will return NOT_OURS.
//  When it is addressed to this device, the msgFunc will handle the message, fill the AppPdu with
//  reply data and address, and return a value of ALL_IS_WELL.  This class will then send the msg
//	packet to the server.  It is up to the msgFunc to handle errors and form the correct error
//  response packet, returning ALL_IS_WELL to this class to enable the error packet to be sent.
int NativeApp::handleMessage(AppPdu *pPDU)
{
	int r = 0;
	r = pPDU->processPdu( ); // internal DriverMsg to MSG data transfer
	r = pPDU->evalMsg();     // FOR_US || NOT_OURS...doesn't check checksum
#ifdef _DEBUG

	if (pPDU->CmdNum() == 109)
	{
		unsigned U = GetTickCount(); U &= 0xFFFFFF;
		fprintf(stderr, "RECEIVE-%10u|", U); fflush(stderr);


		fprintf(stderr, "RECEIVE-%10.3f|", fmod( get_elapsed_time(),100.0) ); fflush(stderr);
	}
	else
	{
		fprintf(stderr, "RECEIVE-----------|"); fflush(stderr);
	}
	pPDU->printMsg();
#endif
	if (pPDU->IsSTX()) pAppConnector->incStx();
	do
	{
		if ( r == FOR_US || pPDU->CmdNum() == 120)  // it is addressed to us so process the message 
		{// handle
//			r = slave_app(pPDU->nMSG());// loads data base with request, refills pkt w/ resp
			r = handle_device_message(pPDU);// -1 at do not reply;  0 with BC/RC/DS all set (no other return values)
			if ( r == NOT_OURS )// a do-not-respond
			{
				pPDU->clear();
				dbgp_log("-------- NotOurs via slave.\n",0);
				// TJ 20may2019 - caller only supports NO_ERROR & FATAL_ERROR, all else is discarded
				return r;// return not ours so skip the send_message   - ie discard
			}
			else// must be 0 - no error
			{
				pPDU->InsertCheckSum();
				if (pPDU->CmdNum() == 0 )
				{
					pPDU->learnAddress();// from the response packet
				}
			}// fall thru to return & send_message
			// TJ 20may2019 - caller only supports NO_ERROR & FATAL_ERROR, all else is discarded
			// this must be no-error 
		}
		else
		//evalMsg only returns good ie FOR_US (0) or bad ie NOT_OURS (-1): all ok, just not our address
		// TJ 20may2019 - caller only supports NO_ERROR & FATAL_ERROR, all else is discarded like -1
		{
		// timj 30jul2019 - hip_server clients must respond to wrong address
			//pPDU->clear();
			//dbgp_log("-------- NotOurs via address\n",0);
			//return r;// return not ours to skip the send_message
			uint8_t commerr = 0x84;             // Comm Error 0x80 + Communication Failure 0x04
			TpPdu tppdu(pPDU->GetPduBuffer());
			tppdu.ProcessErrResponse(0x84);    // forms error response in place..using resrved comm err
			return NO_ERROR;
		}
		/* evalMsg only returns good or bad  
		else
		if ( r == ERROR_OCCURRED )// in eval
		{				
			pPDU->setErrorPkt(); // - always returns 0
		}// fall thru to return & send_message
		*/
	}
	while(false);// execute once

	return 0;
}

// stop threads, delete semaphores and allocated memory
// virtual, not static...
errVal_t NativeApp::cleanup()
{
	/* Do a graceful cleanup of the program like: free memory/resources/etc and exit */
	devicedata.putData();
	devicedata.cleanup();

	// close the queues and leave+
	if (pAppConnector->queuesAreOpen())
	{
		pAppConnector->sendTerm();
		pAppConnector->closeMQs();
	}
	// else they were never opened (or previously closed)

	close_toolLog();
	return NO_ERROR;
}


// this does the slave_app functionality...breaks up handleMessage()
// return a NOT_OURS for a no-response, generate an error pkt on error return
int NativeApp::handle_device_message(AppPdu *pPDU)
{
	uint8_t *pRC = (uint8_t *)responseCode.pRaw;
	uint8_t *pDS = (uint8_t *)deviceStatus.pRaw;
	// note about threading:
	//  The data write is on this thread so no more commands can be handled
	//  till the file write is complete, effectivly locking the nonVolatile data.

	int i     = commandList.handleCmd( pPDU ); // actually deal with command
	//-1@not supported
	//0x80@do not send, 
	//0x00@ success, 
	// response code at warning or error

	// did we find a command we support ?
	if ( i == 0x80 )// hi bit set, do not respond 
	{
		return NOT_OURS;
		pPDU->clear();
	}
	else
    if ( i == -1 )   // not supported
	{
        *pRC = 64;   // command not implemented error packet
        pPDU->SetByteCount( 2 );
    }
    else // we have a response code
	{
        *pRC = i;
    }

	if (writeNeeded)
	{
		devicedata.putData();
	} 

	bool msa = (pPDU->isPrimary()) ? Pri_MSA () : Sec_MSA();
	bool ccb = (pPDU->isPrimary()) ? configChangedBit_Pri : configChangedBit_Sec;
	if (! pPDU->isPrimary())
	{
		fprintf(stderr,"        Secondary Master: ccb = %s\n",ccb?"Set":"Clr"); fflush(stderr);
	}
	if (ccb)   // this host's ccb is set
		*pDS |= 0x40;// set the config changed bit
	else
		*pDS &= 0xBF;// clr the config changed bit

	if (msa)   // this host's msa is set
		*pDS |= 0x10;// set the more status bit
	else
		*pDS &= 0xEF;// clr the more status bit
			
	pPDU->SetRCStatus(*pRC, *pDS);

	return 0; // caller calculates checksum and sends reply
}


// this assumes pdu has command # and master address & delim set correctly
int NativeApp::burstIt(dataItem indexList[], AppPdu *pPDU)
{
	int retVal = 0;

	uint16_t cmdNum = pPDU->CmdNum();// NOT:   pPDU->command;
	cmd_base * pCmd = pGlobalApp->cmd(cmdNum);
	if (pCmd == NULL)
	{// command not supported
		printf("Burst command %d is not implemented.\n", cmdNum); 
	fprintf(stderr, "                    Burst command %d is not implemented.\n", cmdNum);
		return CMD_ERROR;
	}
	// else...we got one
	//we need an address
	if (pCmd->number() == 9)// the only burst cmd w/ indexes
	{
		/*cmd_09* pNine = dynamic_cast<cmd_09*>(pCmd);
		if (pNine == NULL)
		{
			printf("Burst command pointer did not cast to command.\n ");
		}
		else
		{
			pNine->setIndexes(indexList);
		}*/
		pCmd->setIndexes(indexList);
	}
	
	pCmd->burstThisCmd(indexList, pPDU);// fills the data

	uint8_t *pRC = (uint8_t *)responseCode.pRaw;
	uint8_t *pDS = (uint8_t *)deviceStatus.pRaw;

	// a burst pdu can have either master address
	bool msa = (pPDU->isPrimary()) ? Pri_MSA() : Sec_MSA();
	bool ccb = (pPDU->isPrimary()) ? configChangedBit_Pri : configChangedBit_Sec;

	if (ccb)   // this host's ccb is set
		*pDS |= 0x40;// set the config changed bit
	else
		*pDS &= 0xBF;// clr the config changed bit

	if (msa)   // this host's msa is set
		*pDS |= 0x10;// set the more status bit
	else
		*pDS &= 0xEF;// clr the more status bit

	pPDU->SetRCStatus(*pRC, *pDS);

	pPDU->InsertCheckSum();

	pPDU->command = HART_APP_CMD; // this is actually a control command..
	pAppConnector->sendBurstMsg(pPDU); // send same message 

	return retVal;
}

errVal_t NativeApp::start_system(void)
{
	errVal_t ret = UNKNOWN_ERROR;
	// this occurs at the init message from the server
	// starts the physical interface
	int r = init_phy();
	if ( ! r )// success
	{
		// start the burst thread that updates the timer & deals with burst as required
		r = init_burst(devicedata.getMyAddr());
		if ( r ) // an error
		{
			kill_phy();
			// leave ret 
		}
		//else // all-is-well
		//{
		//	// and anything else that is required to pump messages
			ret = NO_ERROR;
		//}
	}
	// else return the phy error 
	return ret;
}


#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>

// this requires at least 3 bytes in the array
errVal_t NativeApp::getLowMAC(uint8_t *pArr)
{
	errVal_t Ret = NO_ERROR;
	struct ifaddrs *ifaddr, *ifa;
	int sk, n;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return (LINUX_ERROR);
	}

	// Walk through linked list, maintaining head pointer so we
	//  can free list later 

	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			struct ifreq ifr;
			int s;
			if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				perror("socket");
				return LINUX_ERROR;
			}

			strcpy(ifr.ifr_name, ifa->ifa_name);
			if (ioctl(s, SIOCGIFHWADDR, &ifr) < 0) {
				perror("ioctl");
				return LINUX_ERROR;
			}

			unsigned char *hwaddr = (unsigned char *)ifr.ifr_hwaddr.sa_data;
			if (hwaddr[1] != 0)
			{
			//  for debug only
			//	printf("\n**| %s :   %02X:%02X:%02X:%02X:%02X:%02X\n", ifa->ifa_name, hwaddr[0], hwaddr[1],
			//		hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);

				if (pArr == NULL)
				{
					Ret = PARAM_ERROR;
				}
				else
				{
					pArr[0] = hwaddr[3];
					pArr[1] = hwaddr[4];
					pArr[2] = hwaddr[5];
					Ret = NO_ERROR;
				}

				close(s);
				break;// out of for loop
			}
			close(s);
		}// all of AF_NET
	}// next

	freeifaddrs(ifaddr);

	return Ret;
}

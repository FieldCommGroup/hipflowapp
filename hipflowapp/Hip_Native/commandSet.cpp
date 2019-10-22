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
 *		This is the command class, there will be one for each command.
 *
 */

 #include "commandSet.h"
 #include "nativeapp.h"

 #include "cmd_00.h" // individual commands should only be included once...here only
 #include "cmd_01.h" 
 #include "cmd_02.h" 
 #include "cmd_03.h" 
 #include "cmd_06.h" 
 #include "cmd_07.h" 
 #include "cmd_08.h" 
 #include "cmd_09.h" 
 #include "cmd_11.h" 
 #include "cmd_12.h"  
 #include "cmd_13.h"  
 #include "cmd_14.h"  
 #include "cmd_15.h"  
 #include "cmd_16.h"  
 #include "cmd_17.h"  
 #include "cmd_18.h"  
 #include "cmd_19.h" 
 #include "cmd_20.h"  
 #include "cmd_21.h" 
 #include "cmd_22.h"  
 #include "cmd_35.h"
 #include "cmd_38.h"  
 #include "cmd_44.h"  
 #include "cmd_48.h"
 #include "cmd_50.h"
 //#include "cmd_51.h"
 #include "cmd_52.h"
 #include "cmd_53.h"
 #include "cmd_54.h"
 #include "cmd_79.h"
 #include "cmd_95.h"
 #include "cmd_c103.h"
 #include "cmd_c104.h"
 #include "cmd_c105.h"
 #include "cmd_c107.h"
 #include "cmd_c108.h"
 #include "cmd_c109.h"
 #include "cmd_c120.h"
 #include "cmd_c534.h"
 #include "cmd_k2560.h"
 #include "cmd_k2561.h"
 #include "cmd_k2562.h"
 #include "cmd_k2563.h"
 #include "cmd_k2688.h"
 #include "cmd_k2689.h"
 #include "cmd_k2690.h"
 #include "cmd_k2692.h"
 #include "cmd_k2693.h"
 #include "cmd_k2864.h"
 

 class cmd_end : public cmd_base
{
public: // c.dtor
	cmd_end():cmd_base(-1) {};
	~cmd_end(){};

public: // work
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData){return -1;};
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData){return -1;};

};


// allocate the memory
static cmd_00 c_zero;
static cmd_01 c_one;
static cmd_02 c_two;
static cmd_03 c_three;
static cmd_06 c_six;
static cmd_07 c_seven;
static cmd_08 c_eight;
static cmd_09 c_nine;
static cmd_11 c_eleven;
static cmd_12 c_twelve;
static cmd_13 c_thirteen;
static cmd_14 c_forteen;
static cmd_15 c_fifteen;
static cmd_16 c_sixteen;
static cmd_17 c_seventeen;
static cmd_18 c_eighteen;
static cmd_19 c_nineteen;
static cmd_20 c_twenty;
static cmd_21 c_twentyOne;
static cmd_22 c_twentyTwo;
static cmd_35 c_thirtyFive;
static cmd_38 c_thirtyEight;
static cmd_44 c_fortyFour;
static cmd_48 c_fortyEight;
static cmd_50 c_fifty;
// no mappable PV...static cmd_51 c_fiftyOne;
static cmd_52 c_fiftyTwo;
static cmd_53 c_fiftyThree;
static cmd_54 c_fiftyFour;
static cmd_79 c_seventyNine;
static cmd_95 c_nintyfive;
static cmd_103 c_oneOhThree;
static cmd_104 c_oneOhFour;
static cmd_105 c_oneOhFive;
static cmd_107 c_oneOhSeven;
static cmd_108 c_oneOhEight;
static cmd_109 c_oneOhNine;
static cmd_120 c_oneTwenty;// the test command 
static cmd_534 c_fiveThirtyFour;
static cmd_2560	c_twentyfiveSixty;
static cmd_2561 c_twentyfiveSixtyone;
static cmd_2562 c_twentyfiveSixtytwo;
static cmd_2563 c_twentyfiveSixtythree;
static cmd_2688 c_twentysixEightyeight;
static cmd_2689 c_twentysixEightynine;
static cmd_2690 c_twentysixNinety;
static cmd_2692 c_twentysixNinetytwo;
static cmd_2693 c_twentysixNinetythree;
static cmd_2864 c_twentyeightSixtyfour;
static cmd_end c_end;

cmd_base * cmdSet::commandList[] = 
 {// these must be in command number order
	&c_zero,	// required to be index 0
&c_one,
&c_two,
&c_three,
&c_six,
&c_seven,
&c_eight,
&c_nine,
&c_eleven,
&c_twelve,
&c_thirteen,
&c_forteen,
&c_fifteen,
&c_sixteen,
&c_seventeen,
&c_eighteen,
&c_nineteen,
&c_twenty,
&c_twentyOne,
&c_twentyTwo,
&c_thirtyFive,
&c_thirtyEight,
&c_fortyFour,
&c_fortyEight,
&c_fifty,
// no mappable PV...&c_fiftyOne,
// disable zero pv..&c_fiftyTwo,
&c_fiftyThree,
&c_fiftyFour,
&c_seventyNine,
&c_nintyfive,
&c_oneOhThree,
&c_oneOhFour,
&c_oneOhFive,
&c_oneOhSeven,
&c_oneOhEight,
&c_oneOhNine,
&c_oneTwenty,
&c_fiveThirtyFour,
&c_twentyfiveSixty,
&c_twentyfiveSixtyone,
&c_twentyfiveSixtytwo,
&c_twentyfiveSixtythree,
&c_twentysixEightyeight,
&c_twentysixEightynine,
&c_twentysixNinety,
&c_twentysixNinetytwo,
&c_twentysixNinetythree,
&c_twentyeightSixtyfour,
&c_end
 };
 
cmdSet::cmdSet( )
{
	for (cmdCount = 0; commandList[cmdCount]->number() >= 0; cmdCount++);
}


cmdSet::~cmdSet()
{
}

// returns the matching list Index or -1 on not supported
int  cmdSet::cmdSupported(int cmdNum)
{
	for ( int i = 0; i < cmdCount; i++)
	{
		if (commandList[i]->number() == cmdNum )
			return i;
		if (commandList[i]->number() >  cmdNum )
			return -1;
	}
	return -1;// target is larger than our highest supported number
}

// return 
// response code otherwise
//     return RC with bit 7 SET if no response is needed
int cmdSet::handleCmd(AppPdu *pPDU)
{
	int ret = RC_SUCCESS;
	uint16_t actualCmdNumber = pPDU->CmdNum();// separate for debugging
	if (actualCmdNumber == 120 && pPDU->ByteCount() < 12)
	{
		printf("Rcv'd command 120.\n");
	}
	int index = cmdSupported( actualCmdNumber );
	if ( index < 0 )
	{// command not supported
		ret = -1;// not supported
	}
	else// we got one
	{
		ret = commandList[index]->handleCmd(pPDU);
		if (actualCmdNumber == 120 )
			ret = -1;// treat as unsupported
	}
	return ret;//-1@not supported, x80@do not send, 0 success, or response code
}

// returns NULL on bad command number
cmd_base * cmdSet::getCmd(uint16_t cmdNum)
{
	int index = cmdSupported(cmdNum);
	if (index < 0)
	{// command not supported
		return NULL;// not supported
	}
	else// we got one
	{
		return  commandList[index];
	}
	return NULL; // to make the complier happy
}
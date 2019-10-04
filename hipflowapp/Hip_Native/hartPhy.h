/*************************************************************************************************
 *
 * Workfile: hartPhy.h 
 * 06Dec18 - stevev
 *
 *************************************************************************************************
* The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description: This holds various definitions used by HART and the phy interface
 *	
 *		
 *	
 * #include "hartPhy.h"
 */
#pragma once

#ifndef _HART_PHY_H
#define _HART_PHY_H
#ifdef INC_DEBUG
#pragma message("In hartPhy.h") 
#endif

#include "hartdefs.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::hartPhy.h") 
#endif



#define NO_CLASS     0		// 'Not Classified' 
#define UNITS_PSI     6
#define UNITS_F      0x21

#define UNITS_PERCNT	 57		// %
#define UNITSNAME_PERCNT "%"
#define UNITSMULTI_PERCNT 1

#define UNITS_MA		39		// mA
#define UNITSNAME_MA	"mA"
#define UNITSMULT_MA		 1

#define UNITS_KG		61		// kg
#define UNITSNAME_KG	"kg"
#define UNITSMULT_KG		1

#define UNITS_POUND		63
#define UNITSNAME_POUND	"lb"
#define UNITSMULT_POUND	0.45359237

#define UNITS_TON		64
#define UNITSNAME_TON	"ton"
#define UNITSMULT_TON	907.18474

/* * * * * * * FLOW * * * * * * */
#define UNITS_KGpH		75		// kg/h
#define UNITSNAME_KGpH	"kg/h"
#define UNITSMULT_KGpH	 1

#define UNITS_KGpM		74		
#define UNITSNAME_KGpM	"kg/min"
#define UNITSMUTL_KGpM	0.0166666667

#define UNITS_LBpM		81		
#define UNITSNAME_LBpM	"lb/min"
#define UNITSMUTL_LBpM	0.0075598728

#define UNITS_LBpH		82		
#define UNITSNAME_LBpH	"lb/h"
#define UNITSMUTL_LBpH	0.45359237

#define UNITS_TpM		84		
#define UNITSNAME_TpM	"ton/min"
#define UNITSMUTL_TpM	15.11974566667

#define UNITS_TpH		85		
#define UNITSNAME_TpH	"ton/h"
#define UNITSMUTL_TpH	907.184740

#define UNITS_NONE		HART_UNITCODE_NOT_USED		// unused
#define UNITSNAME_NONE	"Unused"		// unused
#define UNITSMUTL_NONE	0.0

#define NO_STATUS      0
#define UNUSED_STATUS    HART_DEVICESTAT_NOT_USED
#define DV_STATUS_GOOD   0xc0	/* bit 7 & 6 */
#define DV_STATUS_POOR   0x40	/* bit  6 */

#define MA_ALARM       100.0  /* alarms above this value  */

#define PHY_UPDATE_PERIOD	3200	/* assume 100mS for now */

#define NOT_USED       250

/*************** Externs ***************************************************************/
extern bool isBIGendian;// false on ARM Pi...ie it's little endian



#endif //_HART_PHY_H
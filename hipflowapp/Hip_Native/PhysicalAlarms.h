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
 * Description: This describes the alarms
 *
 * #include "PhysicalAlarms.h"
 */

#pragma once

#ifndef PHY_ALARMS_H_
#define PHY_ALARMS_H_

// checks and possibly sets alarms for the device variable
extern void pv_alarm(double newValue);
extern void sv_alarm(double newValue);

// device specific, Command 48 alarm set
// bit enumerated - these are the same on PV & SV
enum DeviceVariableAlarms
{
	dv_ZeroValue = 0x01,
	dv_LoAlarm   = 0x02,
	dv_HiAlarm   = 0x04,
	dv_OverRange = 0x08
};

// Extended Field Device Status 
// bit enumerated
enum ExtendedDeviceStati
{
	eds_MaintReqd   = 0x01,
	eds_DevVarAlert = 0x02,
	eds_PowerFail       = 0x04,// not used
	eds_CondStatFailure = 0x08,// condensed status not supported
	eds_CondStatOutSpec = 0x10,// condensed status not supported
	eds_CondStatFuncChk = 0x20 // condensed status not supported
};

// Device Variable Status
enum ProcessDataQuality
{
	pdq_Bad   = 0x00,
	pdq_Poor  = 0x40,
	pdq_Fixed = 0x80,
	pdq_Good  = 0xC0
};
#define NO_PDQ  0x3F  /*aka Device Variable Status BAD */
enum LimitStatus
{
	ls_Not  = 0x00, // Not Limited
	ls_Low  = 0x10, // Low Limited
	ls_Hi   = 0x20, // High Limited
	ls_Const= 0x30  // Held Constant
};
#define NO_LIMIT_STATUS  0xCF	/* aka Limit Status NOT LIMITED */
// Device Family status not supported

// Field Device Status (comes right after the response code)
// bit enumerated
enum DeviceStatus
{
	ds_PV_OutOfLimit    = 0x01,
	ds_NonPV_OutOfLimit = 0x02,
	ds_PV_Saturated     = 0x04,
	ds_PV_FixedOutput   = 0x08,
	ds_MoreStatusAvail  = 0x10,
	ds_Cold_Start       = 0x20,
	ds_Config_Changed   = 0x40,
	ds_DeviceMalfunction= 0x80
};
enum DeviceStatus_CLEAR
{
	dsc_DeviceMalfunction= 0x7F,
	dsc_Config_Changed   = 0xBF,
	dsc_Cold_Start       = 0xDF,
	dsc_MoreStatusAvail  = 0xEF,
	dsc_PV_FixedOutput   = 0xF7,
	dsc_PV_Saturated     = 0xFB,
	dsc_PV_OutOfLimit    = 0xFD,
	dsc_NonPV_OutOfLimit = 0xFE
};
#define NO_DEVICE_STATUS   0x00  
/* eg DS = ( DS & NO_DEVICE_STATUS )*/

#define PV_INDEX  (NONvolatileData.devVar_Map[0])
#define SV_INDEX  (NONvolatileData.devVar_Map[1])

#define DV_FLOW   0  /* Device Variable Flow Index */
#define DV_TUBEI  1	 /* Device Variable Tube Current index*/
#define DV_FLWTOT 2	 /* Device Variable Flow Total*/


#endif // PHY_ALARMS_H_


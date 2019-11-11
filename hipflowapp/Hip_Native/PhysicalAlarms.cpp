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
 *		This handles the alarms for the physical interface.
 *
 */

#include <stdio.h>
#include <mutex>
#include "PhysicalAlarms.h"
#include "cDataRaw.h"


std::mutex al_mutex;

extern bool    configChangedBit_Pri;
extern bool	configChangedBit_Sec;

// clr has the bits you want cleared, 0 the bits in clr that you want to leave
void clr_DeviceStatus(unsigned char clr)// verifies set bits are clear in DS
{// these two cannot be cleared here (it's more complex than that)
	unsigned char usableMask = clr & (~(ds_MoreStatusAvail| ds_Config_Changed));

	volatileData.deviceStatus &= ~usableMask;

}
void set_DeviceStatus(unsigned char set)
{
	//if (set & ds_MoreStatusAvail)//MSA bit is set
	//{
	//	if (Pri_MSA())// is set
	//	{
	//		if (PV_cmd48IsSame)// compare PV last sent cmd 48 to main cmd48
	//		{// we have cleared cmd 48 to the last state it has sent us
	//			Pri_MSA( false );
	//		}// else, leave it set
	//	}
	//	
	//	if (Sec_MSA())// is set
	//	{
	//		if (SV_cmd48IsSame)// compare SV last sent cmd 48 to main cmd48
	//		{// we have cleared cmd 48 to the last state it has sent us
	//			Sec_MSA( false );
	//		}// else, leave it set
	//	}
	//}
	if (set & ds_Config_Changed)
	{	
		configChangedBit_Pri =
		configChangedBit_Sec = true;
#ifdef _DEBUG
fprintf(stderr, "        SecCCB set...\n");
#endif
	}
	volatileData.deviceStatus |= set;
}

bool newlySet(unsigned char New, unsigned char Old)
{
	return (( New | Old ) ^ Old ) != 0;
}

// This function assumes you are setting either primary or secondary, never both
// returns value-changed DOES NOT MESS WITH DEVICE STATUS
bool setDeviceSpecific(unsigned char set)
{// will set more status available
	bool data_changed = false;
	//if hi(secondary), keep the low, else keep the hi
	unsigned char previous = 0;
	if (set > 0x0f) 
	{// secondary, hi nibble
		previous = volatileData.cmd48Data[0] & 0xf0; //save previous hi value
		//if (set != previous)
		//	msa = true;		// new != old
		// stevev 05nov2019 - change to only be true if a new bit is set
		data_changed = newlySet(set, previous);
		volatileData.cmd48Data[0] &= 0x0f;// clear all 4 bits(save the lo)
	}
	else
	{// primary, lo nibble
		previous = volatileData.cmd48Data[0] & 0x0f; //save previous lo value
		//if (set != previous)
		//	msa = true;		// new != old
		data_changed = newlySet(set, previous);
		volatileData.cmd48Data[0] &= 0xf0;// clear all 4 bits(save the hi)
	}
	volatileData.cmd48Data[0] |= set;

	//if (msa)// any bit newly set
	//{
	//	set_DeviceStatus(ds_MoreStatusAvail);
	//	return true;
	//}
	//return false; // no change detected
	return data_changed;
}

// returns extended device status changed - does not modify Device Status
bool set_ExtendedFieldDeviceStatus(unsigned char set)
{// exists in command 48
	set &= 0x03;// we only use the bottom 2 bits
	unsigned char previous = volatileData.extended_fld_dev_status & 0x03;
	volatileData.extended_fld_dev_status  = 0x00;
	volatileData.extended_fld_dev_status |= set;

	return (set != previous);
}
void clr_ExtendedFieldDeviceStatus()
{
	if( volatileData.devVars[PV_INDEX].devVar_status == (pdq_Good | ls_Not) &&
		volatileData.devVars[SV_INDEX].devVar_status == 0  )
	{
		volatileData.extended_fld_dev_status = 0;// clear it all
	}
}

// this assumes you are setting all 4 bits (QUality and Limit) at the same time!
// this can be used to clear 'em too
// returns true if Extended Device Status changes
void set_PV_DeviceVariableStatus(unsigned char set)
{//will set_ExtendedFieldDeviceStatus-which will set more status available
	
	volatileData.devVars[PV_INDEX].devVar_status = 0;// we don't use the bottom 4 bits
	volatileData.devVars[PV_INDEX].devVar_status |= set;

	//           bad!fixed       or     any limits
	if (  ((set & pdq_Poor)== 0) || ((set & ls_Const)!= 0)  )
	{
		volatileData.devVars[PV_INDEX].boolDevVarAlert =  set;// set extended has to roll these up
	}
	else
	{
		volatileData.devVars[PV_INDEX].boolDevVarAlert = 0; // no alert here /false
	}
}
// this assumes you are setting all 4 bits (QUality and Limit) at the same time!
// returns true if Extended Device Status changes
void set_SV_DeviceVariableStatus(unsigned char set)
{ //will set_ExtendedFieldDeviceStatus-which will set more status available
	
	volatileData.devVars[SV_INDEX].devVar_status = 0;// we don't use the bottom 4 bits
	volatileData.devVars[SV_INDEX].devVar_status |= set;

	//           bad!fixed      or     any limits
	if (((set & pdq_Poor) == 0) | ((set & ls_Const) != 0))
	{
		volatileData.devVars[SV_INDEX].boolDevVarAlert = set;// set extended has to roll these up
	}
	else
	{
		volatileData.devVars[SV_INDEX].boolDevVarAlert = 0; // no alert here /false
	}	
}

/* this clears everything, then handle_alarms will set what's required */
void clearAlarm(unsigned DeviceVarIdx)
{
	if (DeviceVarIdx == PV_INDEX)
	{// lo nibble clear
		volatileData.cmd48Data[0] &= 0xf0;// clear all 4 bits(save the hi)
		set_PV_DeviceVariableStatus(pdq_Good);
		//clr_DeviceStatus(0xfd);// clear all but non-pv out of limits
		volatileData.devVars[PV_INDEX].boolDevVarAlert = 0;
	}
	else// secondary
	if (DeviceVarIdx == SV_INDEX)
	{// hi nibble clear
		volatileData.cmd48Data[0] &= 0x0f;// clear all 4 bits(save the lo)
		set_SV_DeviceVariableStatus(pdq_Good);
		//clr_DeviceStatus(0xff);// all
		set_ExtendedFieldDeviceStatus(0);// only set for SV so we'll only clear it here
		volatileData.devVars[SV_INDEX].boolDevVarAlert = 0;
	}
	else
	{
		// error
	}
}

/* stevev 05nov2019 - changed the handling where setalarm merely sets the appropriate bits
 * and the Extended Device Status will be rolled up from DevVarStati - modifying cmd48,  
 * the Device-Status More Status Avail will be set by PRi/SEc cmd48 previous write
 * (ie on demand processing).
 */
void setAlarm(DeviceVariableAlarms enumVal, unsigned DeviceVar)
{
	unsigned char theSet = enumVal;

	if (DeviceVar == SV_INDEX)
	{// sv is in the hi nibble
		theSet = (unsigned char)(enumVal << 4);
	}

	switch(enumVal)
	{
	case dv_ZeroValue:
	{	
		setDeviceSpecific(theSet);//sets command 48 - more status has to be set on command demand

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			set_PV_DeviceVariableStatus(pdq_Bad|ls_Low);//sets device Var status, & extended device status
			set_DeviceStatus(ds_PV_OutOfLimit);// sets bit in device status
		}
		else // SV_INDEX
		{
			set_SV_DeviceVariableStatus(pdq_Bad | ls_Low);//sets device Var status, & extended device status

			set_DeviceStatus(ds_NonPV_OutOfLimit);// sets bit in device status

			set_ExtendedFieldDeviceStatus(eds_MaintReqd);// sets bit in extended device status
			// this is basically a power supply failure
			set_DeviceStatus(ds_DeviceMalfunction);// sets bit in device status
		}
	}
	break;
	case dv_LoAlarm:
	{
		setDeviceSpecific(theSet);

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			set_PV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			set_SV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_ExtendedFieldDeviceStatus();// clears it all
			clr_DeviceStatus(ds_DeviceMalfunction | ds_NonPV_OutOfLimit);// in case any set
		}
	}
	break;
	case dv_HiAlarm:
	{
		setDeviceSpecific(theSet);

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			set_PV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			set_SV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_ExtendedFieldDeviceStatus();// clears it all
			clr_DeviceStatus(ds_DeviceMalfunction | ds_NonPV_OutOfLimit);// in case any set
			set_ExtendedFieldDeviceStatus(eds_MaintReqd);// part of requirements
		}
	}
	break;
	case dv_OverRange:
	{
		setDeviceSpecific(theSet);
		
		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			set_PV_DeviceVariableStatus(pdq_Bad | ls_Hi);
			set_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			set_SV_DeviceVariableStatus(pdq_Bad | ls_Hi);
			clr_ExtendedFieldDeviceStatus();// clears it all
			set_ExtendedFieldDeviceStatus(eds_MaintReqd);
			// this is basically a power supply failure
			set_DeviceStatus(ds_NonPV_OutOfLimit);
			clr_DeviceStatus( ds_DeviceMalfunction);
		}
	}
	break;
	default:
	{
		set_DeviceStatus(ds_DeviceMalfunction);// something seriously screwed up
	}
	break;
	}// endswitch


}

/* the setAlarm function deals with the device-variable alarms and the device status
  and the command 48 device-specific alarms.   It does NOT unmap the PV/SV indexes to
  set the correct command 48 device-specific alarm and the device-variable status. It
  merely assumes Zero PV is 0 device-variable & One SV is 1 device-variable        */
void handle_alarms(double newValue, int index)// index is PV_INDEX or SV_INDEX
{
	bool overrange = false;
	//all the setting and clearing goes on in this routine and its subroutines
	std::lock_guard<std::mutex> lock(al_mutex); //auto unlock at return

	/* clear all for this index, then set as needed */
	clearAlarm(index);

	if (newValue >= NONvolatileData.devVars[index].high_alarm)
	{
		setAlarm(dv_HiAlarm, index);
	}
	//else less than hi alarm, could be a good value

	if (newValue >= NONvolatileData.devVars[index].upperSensorLimit)
	{
		setAlarm(dv_OverRange, index);
		overrange = true;
	}
	// else less than hihi

	if (newValue <= NONvolatileData.devVars[index].low_alarm)
	{
		setAlarm(dv_LoAlarm, index);
	}
	// else greater than lo alarm, could be a good value

	if (newValue <= NONvolatileData.devVars[index].lowerSensorLimit)// lowest
	{
		setAlarm(dv_ZeroValue, index);
		overrange = true;
	}
	// else greater than lolo

	if ( ! overrange )
	{// clear pv/sv out of limits
		if (index)//not pv
		{
			clr_DeviceStatus(0x01);// nonPV OOL
		}
		else // pv
		{
			clr_DeviceStatus(0x02);//    PV OOL
		}
	}
}

void pv_alarm(double newValue)
{
	handle_alarms(newValue, PV_INDEX);
}

void sv_alarm(double newValue)
{
	handle_alarms(newValue, SV_INDEX);
}


uint8_t getDeviceStatus(void)
{
	std::lock_guard<std::mutex> lock(al_mutex); //auto unlock at return

	uint8_t DS = volatileData.deviceStatus;

	return DS;
}

// returns true on error - always false....
bool getCmd48Bytes(uint8_t &Byte0, uint8_t &ExDevSt)
{
	std::lock_guard<std::mutex> lock(al_mutex); //auto unlock at return

	uint8_t DS = volatileData.extended_fld_dev_status & 0x01;// get the lsb only
	
	if (volatileData.devVars[PV_INDEX].boolDevVarAlert ||
		volatileData.devVars[SV_INDEX].boolDevVarAlert)
	{
		DS |= 0x02;
	}
	ExDevSt = DS;
	Byte0   = volatileData.cmd48Data[0];

	return false;
}
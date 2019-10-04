/*************************************************************************************************
 *
 * Workfile: PhysicalAlarms.cpp
 * 15jul19 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		This handles the alarms for the physical interface.
 *
 */

#include <stdio.h>
#include "PhysicalAlarms.h"
#include "cDataRaw.h"

extern bool    configChangedBit_Pri;
extern bool	configChangedBit_Sec;

void clr_DeviceStatus(unsigned char clr)// verifies set bits are clear in DS
{// these two cannot be cleared here (it's more complex than that)
	unsigned char usableMask = clr & (~(ds_MoreStatusAvail| ds_Config_Changed));

	volatileData.deviceStatus &= ~usableMask;

}
void set_DeviceStatus(unsigned char set)
{
	if (set & ds_MoreStatusAvail)//MSA bit is set
	{
		if (Pri_MSA())// is set
		{
			if (PV_cmd48IsSame)// compare PV last sent cmd 48 to main cmd48
			{// we have cleared cmd 48 to the last state it has sent us
				Pri_MSA( false );
			}// else, leave it set
		}
		else
		if (Sec_MSA())// is set
		{
			if (SV_cmd48IsSame)// compare SV last sent cmd 48 to main cmd48
			{// we have cleared cmd 48 to the last state it has sent us
				Sec_MSA( false );
			}// else, leave it set
		}
	}
	if (set & ds_Config_Changed)
	{	
		configChangedBit_Pri =
		configChangedBit_Sec = true;
fprintf(stderr, "        SecCCB set...\n");
	}
	volatileData.deviceStatus |= set;// this isn't used - PV/SV_MSA is used for this bit
}

// This function assumes you are setting either primary or secondary, never both
// returns value-changed
bool setDeviceSpecific(unsigned char set)
{// will set more status available
	bool msa = false;
	//if hi(secondary), keep the low, else keep the hi
	unsigned char previous = 0;
	if (set > 0x0f) 
	{// secondary, hi nibble
		previous = volatileData.cmd48Data[0] & 0xf0; //save previous hi value
		volatileData.cmd48Data[0] &= 0x0f;// clear all 4 bits(save the lo)
		if (set != previous)
			msa = true;
	}
	else
	{// primary, lo nibble
		previous = volatileData.cmd48Data[0] & 0x0f; //save previous lo value
		volatileData.cmd48Data[0] &= 0xf0;// clear all 4 bits(save the hi)
		if (set != previous)
			msa = true;
	}
	volatileData.cmd48Data[0] = set;

	if (msa)// any bit newly set
	{
		set_DeviceStatus(ds_MoreStatusAvail);
		return true;
	}
	return false; // no change detected
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
bool set_PV_DeviceVariableStatus(unsigned char set)
{//will set_ExtendedFieldDeviceStatus-which will set more status available
	
	volatileData.devVars[PV_INDEX].devVar_status = 0;// we don't use the bottom 4 bits
	volatileData.devVars[PV_INDEX].devVar_status |= set;

	//           bad!fixed      or     any limits
	if (  ((set & pdq_Poor)== 0) | ((set & ls_Const)!= 0)  )
	{
		return set_ExtendedFieldDeviceStatus(eds_DevVarAlert);
	}// else, just
	return false;// we didn't change it
}
// this assumes you are setting all 4 bits (QUality and Limit) at the same time!
// returns true if Extended Device Status changes
bool set_SV_DeviceVariableStatus(unsigned char set)
{ //will set_ExtendedFieldDeviceStatus-which will set more status available
	
	volatileData.devVars[SV_INDEX].devVar_status = 0;// we don't use the bottom 4 bits
	volatileData.devVars[SV_INDEX].devVar_status |= set;

	//           bad!fixed      or     any limits
	if (((set & pdq_Poor) == 0) | ((set & ls_Const) != 0))
	{
		return set_ExtendedFieldDeviceStatus(eds_DevVarAlert);
	}// else, just
	return false;// we didn't change it
}

void clearAlarm(unsigned index)
{
	bool msa = false;
	unsigned char previous = 0;
	if (index == PV_INDEX)
	{// lo nibble clear
		previous = volatileData.cmd48Data[0] & 0x0f; //save previous lo value
		volatileData.cmd48Data[0] &= 0xf0;// clear all 4 bits(save the hi)
		if (0 != previous)
			msa = true;
		set_PV_DeviceVariableStatus(0);
		clr_DeviceStatus(ds_PV_OutOfLimit);
	}
	else// secondary
	{// hi nibble clear
		previous = volatileData.cmd48Data[0] & 0xf0; //save previous hi value
		volatileData.cmd48Data[0] &= 0x0f;// clear all 4 bits(save the lo)
		if (0 != previous)
		set_SV_DeviceVariableStatus(0);
			msa = true;
		set_SV_DeviceVariableStatus(0);
		clr_DeviceStatus(ds_NonPV_OutOfLimit | ds_DeviceMalfunction);
	}
	if (msa)// any bit newly set
	{
		set_DeviceStatus(ds_MoreStatusAvail);// something changed in cmd 48...set msa
	}
	clr_ExtendedFieldDeviceStatus();// if both clear


}

void setAlarm(DeviceVariableAlarms enumVal, unsigned DeviceVar)
{
	unsigned char theSet;
	bool bitsChanged = false;

	switch(enumVal)
	{
	case dv_ZeroValue:
	{	
		theSet = enumVal;//< -DeviceSpecific - No Drive Current
		if (DeviceVar == SV_INDEX)
		{// sv is in the hi nibble
			theSet = (unsigned char)(enumVal << 4);
		}
		bitsChanged = setDeviceSpecific(theSet);

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			bitsChanged |= set_PV_DeviceVariableStatus(pdq_Bad|ls_Low);

			set_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			bitsChanged |= set_SV_DeviceVariableStatus(pdq_Bad | ls_Low);

			set_DeviceStatus(ds_NonPV_OutOfLimit);

			set_ExtendedFieldDeviceStatus(eds_MaintReqd);
			// this is basically a power supply failure
			set_DeviceStatus(ds_DeviceMalfunction);
		}
		if (bitsChanged)
		{
			set_DeviceStatus(ds_MoreStatusAvail);
		}
	}
	break;
	case dv_LoAlarm:
	{
		theSet = enumVal;//< -DeviceSpecific 
		if (DeviceVar == SV_INDEX)
		{// sv is in the hi nibble
			theSet = (unsigned char)( enumVal << 4);
		}
		bitsChanged = setDeviceSpecific(theSet);

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			bitsChanged |= set_PV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			bitsChanged |= set_SV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_DeviceMalfunction | ds_NonPV_OutOfLimit);// in case any set
		}
		if (bitsChanged)
		{
			set_DeviceStatus(ds_MoreStatusAvail);
		}
	}
	break;
	case dv_HiAlarm:
	{
		theSet = enumVal;//< -DeviceSpecific 
		if (DeviceVar == SV_INDEX)
		{// sv is in the hi nibble
			theSet = (unsigned char)( enumVal << 4 );
		}
		bitsChanged = setDeviceSpecific(theSet);

		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			bitsChanged |= set_PV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			bitsChanged |= set_SV_DeviceVariableStatus(pdq_Poor);// not limited
			clr_DeviceStatus(ds_DeviceMalfunction | ds_NonPV_OutOfLimit);// in case any set
			set_ExtendedFieldDeviceStatus(eds_MaintReqd);// part of requirements
		}
		if (bitsChanged)
		{
			set_DeviceStatus(ds_MoreStatusAvail);
		}
	}
	break;
	case dv_OverRange:
	{
		theSet = enumVal;//< -DeviceSpecific
		if (DeviceVar == SV_INDEX)
		{// sv is in the hi nibble
			theSet = (unsigned char)(enumVal << 4);
		}
		bitsChanged = setDeviceSpecific(theSet);

		
		if (DeviceVar == PV_INDEX)// < -DeviceVariableStatus.DataQuality Bad
		{
			bitsChanged |= set_PV_DeviceVariableStatus(pdq_Bad | ls_Hi);

			set_DeviceStatus(ds_PV_OutOfLimit);
		}
		else // SV_INDEX
		{
			bitsChanged |= set_SV_DeviceVariableStatus(pdq_Bad | ls_Hi);

			bitsChanged |= set_ExtendedFieldDeviceStatus(eds_MaintReqd);
			// this is basically a power supply failure
			set_DeviceStatus(ds_NonPV_OutOfLimit);
			clr_DeviceStatus( ds_DeviceMalfunction);
		}
		if (bitsChanged)
		{
			set_DeviceStatus(ds_MoreStatusAvail);
		}
	}
	break;
	default:
	{
		set_DeviceStatus(ds_DeviceMalfunction);
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
	if (newValue >= NONvolatileData.devVars[index].upperSensorLimit)
	{
		setAlarm(dv_OverRange, index);
	}
	else // less than hihi
	if (newValue >= NONvolatileData.devVars[index].high_alarm)
	{
		setAlarm(dv_HiAlarm, index);
	}
	else// less than hi alarm
	if (newValue <= NONvolatileData.devVars[index].lowerSensorLimit)// lowest
	{
		setAlarm(dv_ZeroValue, index);
	}
	else// less than hi alarm && more than lolo
	if (newValue <= NONvolatileData.devVars[index].low_alarm)
	{
		setAlarm(dv_LoAlarm, index);
	}
	else// > lo alarm, < hi alarm - good value
	{
		clearAlarm(index);// clear as required
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




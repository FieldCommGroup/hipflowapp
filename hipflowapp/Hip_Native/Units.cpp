/*************************************************************************************************
 *
 * Workfile: Units.cpp
 * 16Aug19 - stevev
 *
 *************************************************************************************************
 * The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, Fieldcomm Group Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description:
 *		Units functionality.
 *
 */

#include "Units.h"


conversion::conversion()
{
}

conversion::~conversion()
{
}

// Unit's value to standard value
double conversion::convert(double counts)
{
	return ( ((counts + PreAdd) / Multiply) + PostAdd);	// was *
}

// StandardValue 2 Unit's value
double conversion::unconvert(double val)
{
	return ( ((val - PostAdd) * Multiply)-PreAdd );		// was /
}

void conversion::Set(double Pre_Add, double Multi, double Post_Add)
{
	PreAdd   = Pre_Add;
	Multiply = Multi;
	PostAdd  = Post_Add;
}

/********************************************************************************/
Unit::Unit(){};
Unit::~Unit(){};


void Unit::NameUnit(int Unit_Code, string UnitName)
{
	UnitCode = Unit_Code;
	UnitName = UnitName;
}

void Unit::SetCvt(double PreAdd, double Mult, double PostAdd)
{
	Cvt.Set( PreAdd, Mult, PostAdd);
}

/********************************************************************************/
UnitList::UnitList(){};
UnitList::~UnitList(){};


bool UnitList::isaUnit(int V)
{ 
	UnitList::iterator U;
	U = find(V); 
	return (U != end()); 
}

int  UnitList::findUnit(string UnitName) // -1 on NotFound
{
	UnitList::iterator U; Unit target(0,UnitName);
	U = find_if(begin(), end(), value_equals<int,Unit>(target));
	if (U != end())
	{	
		return ( U->first );
	}
	else
	{
		return -1;
	}
};

/********************************************************************************/

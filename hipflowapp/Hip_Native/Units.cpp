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


void Unit::NameUnit(int Unit_Code, string Unit_Name)
{
	UnitCode = Unit_Code;
	UnitName = Unit_Name;
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

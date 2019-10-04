/*************************************************************************************************
 *
 * Workfile: Units.h
 * 16Aug19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds the declarations for the Units class
 *
 *
 * #include "Units.h"
*/
#pragma once

#ifndef _C_UNITS_H
#define _C_UNITS_H
#ifdef INC_DEBUG
#pragma message("In Units.h") 
#endif

#include <map>
#include <string>
#include <algorithm>
#include "datatypes.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::Units.h") 
#endif

using namespace std;

class conversion
{
	double PreAdd   = 0.0;
	double Multiply = 1.0;
	double PostAdd  = 0.0;
public:
	conversion();
	~conversion();

public:
	double convert  ( double baseUnitValue);
	double unconvert( double val);// to base unit value

	void Set(double PreAdd, double Mult, double PostAdd);
};

class Unit
{
	int    UnitCode;
	string UnitName;

	conversion Cvt;

public:		// ctor, dtor
	Unit();
	Unit(int Unit_Code, string UnitName){ NameUnit(Unit_Code, UnitName);};
	~Unit();

public:  // setup
	void NameUnit(int Unit_Code, string UnitName);
	void SetCvt(double PreAdd, double Mult, double PostAdd);

public:  // usage
	string Name() {  return UnitName; };
	double frmStandard(double baseUnitValue) {return(Cvt.unconvert(baseUnitValue));};// from base value
	double to_Standard(double val) { return(Cvt.convert(val)); };// Value in Units to base unit value
};

typedef map<int, Unit> UnitMap;

// Unit K; NameUnit(79, "mA"); Units L; L[79] = K;
class UnitList : public UnitMap
{
public:
	UnitList();
	~UnitList();

public:
	bool isaUnit(int V);
	int  findUnit(string UnitName);
};


template<class K, class V>
class value_equals
{
	V value;
public:
	value_equals(const V& v) : value(v) {};
	bool operator()(pair<const K, V>elem)
	{
		return elem.second.Name() == value.Name();
	}
};
#endif // _C_UNITS_H

/*************************************************************************************************
 *
 * Workfile: insert_Extract.h 
 * 17May2019 stevev
 *
 *************************************************************************************************
* The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description: This is the pdu handler for the native device.
 *	
 *		
 *	
 * #include "insert_Extract.h"
 */
#pragma once

#ifndef _INSERT_EXTRACT_H
#define _INSERT_EXTRACT_H
#ifdef INC_DEBUG
#pragma message("In insert_Extract.h") 
#endif

#include "datatypes.h"
#include "hartdefs.h"   // to get response codes

#ifdef INC_DEBUG
#pragma message("    Finished Includes::insert_Extract.h") 
#endif

/* global system description flags */
extern bool isBIGendian;// false on ARM Pi & intel - HART packets are BIG endian

extern const float hartBENaN;
extern const float hartLENaN;


class c_insert_Extract  /* no data; action only */
{
public:
/* all these  return 0 on success, error or -1 on error */
/* all these  increments pData by sizeof(x), increments dataCnt by the same */
uint8_t insert( uint8_t  x, uint8_t **ppData, uint8_t& dataCnt);  
	uint8_t insert( uint16_t x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t insert24(uint32_t x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t insert( uint32_t x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t insert( uint64_t x, uint8_t **ppData, uint8_t& dataCnt);

uint8_t insert(  char    x, uint8_t **ppData, uint8_t& dataCnt);  
	uint8_t insert(  int16_t x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t insert(  int32_t x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t insert(  int64_t x, uint8_t **ppData, uint8_t& dataCnt);

uint8_t insert( float    x, uint8_t **ppData, uint8_t& dataCnt); // these translate nans
uint8_t insert( double   x, uint8_t **ppData, uint8_t& dataCnt); 


/* all these  increments pData by sizeof(x), decrements dataCnt by the same */
uint8_t extract( uint8_t & x, uint8_t **ppData, uint8_t& dataCnt);  
	uint8_t extract( uint16_t& x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t extract24(uint32_t& x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t extract( uint32_t& x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t extract( uint64_t& x, uint8_t **ppData, uint8_t& dataCnt);

	uint8_t extract(  char   & x, uint8_t **ppData, uint8_t& dataCnt);  
	uint8_t extract(  int16_t& x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t extract(  int32_t& x, uint8_t **ppData, uint8_t& dataCnt);
	uint8_t extract(  int64_t& x, uint8_t **ppData, uint8_t& dataCnt);

uint8_t extract( float   & x, uint8_t **ppData, uint8_t& dataCnt); // these translate nans
	uint8_t extract( double  & x, uint8_t **ppData, uint8_t& dataCnt); 
};

#endif // _INSERT_EXTRACT_H
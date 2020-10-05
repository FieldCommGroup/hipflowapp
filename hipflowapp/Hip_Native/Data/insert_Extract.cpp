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
 *		This handles the endian conversion to add/subtract from a packet.
 *
 */

 #include <string.h>
#include "safe_lib.h"

#include "insert_Extract.h"
 
/* all these  return 0 on success, error or -1 on error */
/* all these  increments pData by sizeof(x), increments dataCnt by the same */

// common code -  add expectedCnt bytes from d to pData, incrementing dataCnt & pData
static uint8_t Add_It(uint8_t& dataCnt, uint8_t **ppData, uint8_t* d, uint8_t expectedCnt)
{	memcpy_s(*ppData, expectedCnt, d, expectedCnt);
    dataCnt += expectedCnt;
    *ppData += expectedCnt;
    return 0;
}

// common code -  extract expectedCnt bytes from pData to d, decrementing dataCnt & incrementing pData
static uint8_t extract_It(uint8_t& dataCnt, uint8_t **ppData, uint8_t* d, uint8_t expectedCnt)
{    memcpy_s(d, expectedCnt, *ppData, expectedCnt);
    dataCnt -= expectedCnt;
    *ppData += expectedCnt;
    return 0;
}

///////////////// end common code //////////////////////////////////////////////////////


uint8_t c_insert_Extract::insert( uint8_t  x, uint8_t **ppData, uint8_t& dataCnt)
{
	return Add_It(dataCnt, ppData, (uint8_t*) &x, 1);
}

uint8_t c_insert_Extract::insert( uint16_t x, uint8_t **ppData, uint8_t& dataCnt)
{
	x = REVERSE_S(x);
	memcpy_s((*ppData), sizeof(uint16_t), &x, sizeof(uint16_t));
	dataCnt += (uint8_t)sizeof(uint16_t);
	(*ppData) += sizeof(uint16_t);

	return 0; //SUCCESS
}


uint8_t c_insert_Extract::insert24(uint32_t x, uint8_t **ppData, uint8_t& dataCnt)
{// in situ reversal didn't work
	uint16_t g = (uint16_t)((x >> 16) & 0xffff);// hi word
	uint16_t h = (uint16_t)(x & 0xffff);        // lo word
	x = (((uint32_t)REVERSE_S(h)) << 16) | (uint32_t)REVERSE_S(g);
	*(*ppData) = (uint8_t)(g & 0xff);
	(*ppData)++; dataCnt++;
	*(*ppData) = (uint8_t)((h >>8 ) & 0xff);
	(*ppData)++; dataCnt++;
	*(*ppData) = (uint8_t)(( h ) & 0xff);
	(*ppData)++; dataCnt++;

	return 0; //SUCCESS
}

uint8_t c_insert_Extract::insert( uint32_t x, uint8_t **ppData, uint8_t& dataCnt)
{// in situ reversal didn't work
	uint16_t g = (uint16_t)((x >> 16) & 0xffff);// hi word
	uint16_t h = (uint16_t)(x & 0xffff);
	x = (((uint32_t)REVERSE_S(h)) << 16) | (uint32_t)REVERSE_S(g);
	memcpy_s( (*ppData),sizeof(uint32_t), &x, sizeof(uint32_t) );
	dataCnt += (uint8_t)sizeof(uint32_t);
	(*ppData) += sizeof(uint32_t);

	return 0; //SUCCESS
}
uint8_t c_insert_Extract::insert( uint64_t x, uint8_t **ppData, uint8_t& dataCnt)
{
	return 0; //SUCCESS
}

uint8_t c_insert_Extract::insert(  char    x, uint8_t **ppData, uint8_t& dataCnt)
{
	return Add_It(dataCnt, ppData, (uint8_t*) &x, 1);
}
uint8_t c_insert_Extract::insert(  int16_t x, uint8_t **ppData, uint8_t& dataCnt)
{
	uint16_t X = (uint16_t)x;
	uint16_t rev = REVERSE_S(X);
	memcpy_s( (*ppData), sizeof(uint16_t), &rev, sizeof(uint16_t) );
	dataCnt += (uint8_t)sizeof(uint16_t);
	return 0; //SUCCESS
}
uint8_t c_insert_Extract::insert(  int32_t x, uint8_t **ppData, uint8_t& dataCnt)
{
	return 0; //SUCCESS
}
uint8_t c_insert_Extract::insert(  int64_t x, uint8_t **ppData, uint8_t& dataCnt)
{
	return 0; //SUCCESS
}

uint8_t c_insert_Extract::insert( float    x, uint8_t **ppData, uint8_t& dataCnt) // these translate nans
{
	uint32_t t = REVERSE_L( *((uint32_t*)(&x)) );// this one works...
	if ( x != x )// is it a NaN
	{// it is			
         t = 0x0000a07f; // a reversed HART NaN	
	}
	return Add_It(dataCnt, ppData, (uint8_t*) &t, 4);
}
uint8_t c_insert_Extract::insert( double   x, uint8_t **ppData, uint8_t& dataCnt)
{	
	if ( x != x )// is it a NaN
	{// it is; force a HART NaN		- try this...
		float f;  *((uint32_t *)&f) = 0x0000a07f;
		x = f; // x is local
	}
	uint64_t t = REVERSE_H( *((uint64_t*)(&x)) );				
    return Add_It(dataCnt, ppData, (uint8_t*) &t, 8);
}





uint8_t c_insert_Extract::extract( uint8_t & x, uint8_t **ppData, uint8_t& dataCnt)
{
	uint8_t* pD = *ppData;
	if (dataCnt < 1)
	{
		return RC_TOO_FEW;
	}
	x = *pD;
	pD ++;
	*ppData = pD;
	dataCnt --;
	return 0; //SUCCESS
}
uint8_t c_insert_Extract::extract( uint16_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	uint8_t* pD = *ppData;
	if (dataCnt < 2)
	{
		return RC_TOO_FEW;
	}

	uint16_t rev = *pD;
	x = (uint16_t)REVERSE_S(rev);

	pD+=2;
	*ppData = pD;
	dataCnt-= 2;
	return 0; //SUCCESS
}
uint8_t c_insert_Extract::extract( uint32_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	uint8_t* pD = *ppData;
	if (dataCnt < 4)
	{
		return RC_TOO_FEW;
	}

	uint32_t rev = *pD;
	x = REVERSE_L(rev);

	pD += 4;
	*ppData = pD;
	dataCnt -= 4;
	return 0; //SUCCESS
}
uint8_t c_insert_Extract::extract( uint64_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	return -1; //FAILURE
}

uint8_t c_insert_Extract::extract(  char   & x, uint8_t **ppData, uint8_t& dataCnt)
{
	uint8_t* pD = *ppData;

	if (dataCnt < 1)
	{
		return RC_TOO_FEW;
	}

	x = (char)(*pD);
	pD += 1;
	*ppData = pD;
	dataCnt --;
	return 0; //SUCCESS
} 
uint8_t c_insert_Extract::extract(  int16_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	return -1; //FAILURE
}
uint8_t c_insert_Extract::extract(  int32_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	return -1; //FAILURE
}
uint8_t c_insert_Extract::extract(  int64_t& x, uint8_t **ppData, uint8_t& dataCnt)
{
	return -1; //FAILURE
}


uint8_t c_insert_Extract::extract( float   & x, uint8_t **ppData, uint8_t& dataCnt) // these translate nans
{
	const uint8_t len = (uint8_t)sizeof(float);
	int32_t  t;
	uint8_t ret = 0,  dest[sizeof(float)];

	if ( dataCnt < len )
	{
		return RC_TOO_FEW;
	}

	ret = extract_It(dataCnt, ppData, dest, len);

	memcpy_s( &t, len, &(dest[0]), len);

	t = REVERSE_L( t );

	x = *((float*) &t);

	dataCnt += len;

	return ret; //SUCCESS
}
uint8_t c_insert_Extract::extract( double  & x, uint8_t **ppData, uint8_t& dataCnt)
{
	return -1; //FAILURE
} 

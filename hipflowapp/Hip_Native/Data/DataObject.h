/*************************************************************************************************
 *
 * Workfile: DataObject.h
 * 17May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This holds the common data object data and code
 *
 *
 * #include "DataObject.h"
*/
#pragma once

#ifndef _C_DATAOBJECT_H
#define _C_DATAOBJECT_H
#ifdef INC_DEBUG
#pragma message("In DataObject.h") 
#endif

#include <string.h> /* for memcpy */
#include <assert.h>

#include "DataObjectBase.h"
#include "DataObjectConfig.h"
#include "insert_Extract.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::DataObject.h") 
#endif

template<hartTypes_t HART_TYPE, typename RAW_TYPE, int LEN>
class dataObject : public dataObject_Base, public dataObjectConfig,  private c_insert_Extract
{
protected:
	hartTypes_t h_type  = HART_TYPE;
	uint8_t     h_size  = LEN;
	bool        isConfig;
	uint8_t     arrLen;

	RAW_TYPE    theData;
	RAW_TYPE    *pTheArray;

public: // ctor / dtor
	dataObject():isConfig(false), pTheArray(NULL), arrLen(0){};
	virtual ~dataObject(){if (pTheArray) delete[] pTheArray; 	};
	
	// configuration interface
	void do_Configure(bool is_Config ) { isConfig = is_Config; };
	void do_ConfigureArray(bool is_Config, uint8_t datasize); 

	// object interface
	virtual uint8_t			
	extractSelf(uint8_t **ppData, uint8_t& dataCnt)
	{
		uint8_t dC = dataCnt;
		if (h_type == ht_int24)//handle partial type
		{
			uint32_t localD = theData;
			return extract24(localD, ppData, dC);
		}
		else
			return extract(theData, ppData, dataCnt);
	};

	virtual uint8_t			
	insertSelf(uint8_t **ppData, uint8_t& dataCnt)
	{
		if (h_type == ht_int24)//handle partial type
			return insert24(theData, ppData, dataCnt);
		else
			return insert(theData, ppData, dataCnt);
	};

	virtual std::tuple<uint8_t*, int> //  returns tuple of data ptr, length -
	saveSelfInfo()
	{// we save the raw data
		if (arrLen != 0)
			return std::make_tuple(static_cast<uint8_t*>(pTheArray), (sizeof(RAW_TYPE) * arrLen));
		else
			return std::make_tuple((uint8_t*)&theData, sizeof(RAW_TYPE) );
	};

	virtual void            
	restoreSelf(uint8_t* pD, int L)
	{
		if (pTheArray != 0)
			memcpy(pTheArray, pD, L);
		else
			theData = *((RAW_TYPE*)pD);
	};

	virtual void			
	setSelfValue(CValueVarient& newVal)
	{
		theData = (RAW_TYPE)newVal;
	};
	virtual void
	setSelfValue(RAW_TYPE* newArr, uint8_t newLen)
	{
		memcpy(pTheArray, newArr, newLen);
	};

	virtual CValueVarient	
	getSelfValue(void)
	{
		CValueVarient rv; rv = theData;
		return rv;
	};

	virtual bool            getIsConfig(void) { return isConfig; };
	virtual hartTypes_t		getSelfType(void) { return h_type;   };
};

template<hartTypes_t HART_TYPE, typename RAW_TYPE, int LEN>
void dataObject<HART_TYPE, RAW_TYPE, LEN>   ::
do_ConfigureArray(bool is_Config, uint8_t datasize)
{
	do_Configure(is_Config);
	pTheArray = new RAW_TYPE[datasize];
	assert(pTheArray != NULL);
};

#endif // _C_DATAOBJECT_H
/*************************************************************************************************
 *
 * Workfile: AllDataObjects.h
 * 17May19 - stevev
 *
 *************************************************************************************************
* The content of this file is the
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2019, FieldComm Group, Inc., All Rights Reserved
 *************************************************************************************************
 *
 * Description: This defines all of the primative data classes
 *
 *
 * #include "AllDataObjects.h"
*/
#pragma once

#ifndef _C_ALL_DATA_OBJECTS_H
#define _C_ALL_DATA_OBJECTS_H
#ifdef INC_DEBUG
#pragma message("In AllDataObjects.h") 
#endif

#include "DataObject.h"
#include "DataObjectConfig.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::AllDataObjects.h") 
#endif

// these are the only ones being used in this app, make more as required.

// --- these all rely on the dataObject interfaces ---
// >>>    public dataObject_Base, public dataObjectConfig
//                                HART-type,  Raw-type,  length
class do_int8  : public dataObject< ht_int8,   uint8_t,  1 > {};
class do_int16 : public dataObject< ht_int16,  uint16_t, 2 > {};
class do_int24 : public dataObject< ht_int24,  uint32_t, 3 > {};
class do_int32 : public dataObject< ht_int32,  uint32_t, 4 > {};
class do_float : public dataObject< ht_float,  float,    4 > {};
class do_double: public dataObject< ht_double, double,   8 > {};
class do_ascii : public dataObject< ht_ascii,  uint8_t,  1 > {};
class do_packed: public dataObject< ht_packed, uint8_t,  1 > {};

#endif // _C_ALL_DATA_OBJECTS_H
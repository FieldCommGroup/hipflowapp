/*****************************************************************
 * Copyright (C) 2015-2016 FieldComm Group
 *
 * All Rights Reserved.
 * This software is CONFIDENTIAL and PROPRIETARY INFORMATION of
 * FieldComm Group, Austin, Texas USA, and may not be used either
 * directly or by reference without permission of FieldComm Group.
 *
 * THIS SOFTWARE FILE AND ITS CONTENTS ARE PROVIDED AS IS WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE AND BEING FREE OF DEFECT.
 *
 *****************************************************************/

/**********************************************************
 *
 * File Name:
 *   tooldef.h
 * File Description:
 *   Header file to define the tool being built -
 *    - HTest only, with no Wi-HTest, or 
 *    - Wi-HTest (which includes HTest). 
 *   If Wi-HTest is not being built, only code related 
 *   to HTest will be included in the build.
 *
 **********************************************************/
#ifndef _TOOLDEF_H
#define _TOOLDEF_H

#ifdef _FLOWDEVICE
 #define TOOL_NAME      "Hip_Native-Flow"
 #define TOOL_VERS      "2.0"
#else
  #ifdef _PRESSUREDEVICE
   #define TOOL_NAME   "Hip_Native-Pres"
 #define TOOL_VERS      "1.0"
  #else
   #define TOOL_NAME   "Hip_Native-New"
 #define TOOL_VERS      "0.0"
 #endif
#endif


 #define TOOL_VERS      "2.0"

 
#endif /* _TOOLDEF_H */


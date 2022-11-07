/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
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
 #define TOOL_VERS      "1.1.0"
#else
  #ifdef _PRESSUREDEVICE
   #define TOOL_NAME   "Hip_Native-Pres"
 #define TOOL_VERS      "1.0"
  #else
   #define TOOL_NAME   "Hip_Native-New"
 #define TOOL_VERS      "0.0"
 #endif
#endif


 #define TOOL_VERS      "1.1"

 
#endif /* _TOOLDEF_H */


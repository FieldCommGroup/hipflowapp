#********************************************************************
# Copyright 2019 FieldComm Group, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#********************************************************************

#**********************************************************
#*
#* File Name:
#*   Makefile
#* File Description:
#*   Makefile for hipflowapp
#*   ("Generic" Top level)
#*
#**********************************************************

SHELL=/bin/bash

MAIN_DIR = $(shell pwd)

include Makefile_macros.inc
include Makefile.inc

COMMON_OBJS  = $(HIP_OBJS) $(COMMON_HFILES)

# ===============================================================

# Build everything
EXEC_TOP = all

$(EXEC_TOP): $(COMMON_OBJS)
	echo	
	cd hipserver/hipserver/safestringlib && $(MAKE)
	cd ../../..
	echo Building $(EXEC_TOP)
	echo
	$(LD) $(CFLAGS) -o hipflowapp $(COMMON_OBJS) $(INCL_LIBS) -lpthread -lrt
	echo

		
# ===============================================================

.PHONY: clean
clean:
	$(ECHO)
	$(ECHO) Cleaning $(shell pwd)
	$(RM) -f $(EXEC_TOP)       >  /dev/null 2>&1
	$(RM) -f *.gch             >  /dev/null 2>&1
	$(RM) -fr $(HIP_OBJS)	   >  /dev/null 2>&1
	cd hipserver/hipserver/safestringlib && $(MAKE) clean
	cd ../../..

.PHONY: cleanall
cleanall:
	@$(MAKE) -s clean


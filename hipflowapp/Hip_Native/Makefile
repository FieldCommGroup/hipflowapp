#********************************************************************
#* Copyright (C) 2015-2019 FieldComm Group
#*
#* All Rights Reserved.
#* This software is CONFIDENTIAL and PROPRIETARY INFORMATION of
#* FieldComm Group, Austin, Texas USA, and may not be used either
#* directly or by reference without permission of FieldComm Group.
#*
#* THIS SOFTWARE FILE AND ITS CONTENTS ARE PROVIDED AS IS WITHOUT
#* WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
#* WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR
#* A PARTICULAR PURPOSE AND BEING FREE OF DEFECT.
#*
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


.PHONY: cleanall
cleanall:
	@$(MAKE) -s clean


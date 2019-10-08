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
#*   Makefile.inc
#* File Description:
#*   Include file for Makefile to build the HART-IP APP.
#*
#**********************************************************

#include $(MAIN_DIR)/Makefile_macros.inc

# Header files with definitions that are used in multiple files for the tool
COMMON_HFILES = $(MAIN_DIR)/hipserver/hipserver/Common/common.h      	\
                $(MAIN_DIR)/hipserver/hipserver/Common/datatypes.h   	\
                $(MAIN_DIR)/hipserver/hipserver/Common/errval.h      	\
                $(MAIN_DIR)/hipserver/hipserver/Common/hartdefs.h    	\
                $(MAIN_DIR)/hipserver/hipserver/AppConnect/tpdll.h		\
                $(MAIN_DIR)/hipserver/hipserver/AppConnect/appconnector.h

# List all HART-IP .c files (without extension)
# First list all .c files that have a corresponding .h file
# (1a)
HIP_CFILES =	\
#			$(MAIN_DIR)/hipserver/hipserver/Realtime/toolqueues		\
#			$(MAIN_DIR)/hipserver/hipserver/Realtime/toolutils		\
#			$(MAIN_DIR)/hipserver/hipserver/Realtime/toolsems       \
#			$(MAIN_DIR)/hipserver/hipserver/Realtime/toolthreads    \
#           $(MAIN_DIR)/hipserver/hipserver/Realtime/toolsigs


# Then list all .c files that have no corresponding .h file
# (1b)
HIP_NOH_CFILES =

# ===================================================

# List all HART-IP .cpp files here (without extension)
# First list all .cpp files that have a corresponding .h file
# (1c)
HIP_CPPFILES = $(MAIN_DIR)/hipserver/hipserver/AppConnect/tppdu 		\
               $(MAIN_DIR)/hipserver/hipserver/AppConnect/app						\
               $(MAIN_DIR)/hipserver/hipserver/AppConnect/apppdu					\
               $(MAIN_DIR)/Commands/command					\
               $(MAIN_DIR)/Data/insert_Extract					\
               $(MAIN_DIR)/ads1256_test	\
               $(MAIN_DIR)/bcm2835	\
               $(MAIN_DIR)/burst	\
               $(MAIN_DIR)/cDatabase	\
               $(MAIN_DIR)/commandSet	\
               $(MAIN_DIR)/cSerialFile	\
               $(MAIN_DIR)/Files	\
               $(MAIN_DIR)/FillDefaultValues	\
               $(MAIN_DIR)/InsertExtract	\
               $(MAIN_DIR)/IOthread	\
               $(MAIN_DIR)/memory_glbl	\
               $(MAIN_DIR)/native	\
               $(MAIN_DIR)/nativeapp	\
               $(MAIN_DIR)/physical	\
               $(MAIN_DIR)/PhysicalAlarms	\
               $(MAIN_DIR)/sems	\
               $(MAIN_DIR)/timer	\
               $(MAIN_DIR)/timeStamp	\
               $(MAIN_DIR)/Units	\
               $(MAIN_DIR)/main


# Then list all .cpp files that have no corresponding .h file
# (1d)
HIP_NOH_CPPFILES =

# ===================================================

# List all HART-IP .h files here (without extension)
# (2a + 2b + 2c)
HIP_ALL_HFILES = $(HIP_C_HFILES) $(HIP_CPP_HFILES) $(HIP_NOC_HFILES)

# HIP_C_HFILES are .h files derived from the corresponding .c files
# (2a)
HIP_C_HFILES = $(HIP_CFILES:%=%.h)

# HIP_CPP_HFILES are .h files derived from the corresponding .cpp files
# (2b)
HIP_CPP_HFILES = $(HIP_CPPFILES:%=%.h)

# HIP_NOC_HFILES are .h files that have no corresponding .c/.cpp file
# (2c)
HIP_NOC_HFILES =

# ===================================================

# HART-IP Objects (1a + 1b + 1c + 1d)
HIP_OBJS = $(HIP_CFILES:%=%.o) $(HIP_NOH_CFILES:%=%.o)             \
           $(HIP_CPPFILES:%=%.o) $(HIP_NOH_CPPFILES:%=%.o)

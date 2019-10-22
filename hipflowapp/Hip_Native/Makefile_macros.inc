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
#*   Makefile_macros.inc
#* File Description:
#*   Include file for basic macros used in the Makefile.
#*
#**********************************************************

CC        = g++ -c
CXX        = g++ -c
CFLAGS     = -g -DLINUX -D_FLOWDEVICE -I$(MAIN_DIR)/Commands -I$(MAIN_DIR)/Data -I$(MAIN_DIR)/hipserver/hipserver/Common   -I$(MAIN_DIR)/hipserver/hipserver/AppConnect  -I$(MAIN_DIR)/hipserver/hipserver/Data  -I$(MAIN_DIR)/hipserver/hipserver/Realtime  -I$(MAIN_DIR)  -std=gnu++11 
CXXFLAGS   = -g -DLINUX -D_FLOWDEVICE -I$(MAIN_DIR)/Commands -I$(MAIN_DIR)/Data -I$(MAIN_DIR)/hipserver/hipserver/Common   -I$(MAIN_DIR)/hipserver/hipserver/AppConnect  -I$(MAIN_DIR)/hipserver/hipserver/Data  -I$(MAIN_DIR)/hipserver/hipserver/Realtime  -I$(MAIN_DIR) -std=gnu++11 
ECHO       = @echo
INCL_LIBS  = -lpthread -lrt
LD         = g++ 
RM         = @rm

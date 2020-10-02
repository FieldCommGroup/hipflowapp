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
#*   Makefile_macros.inc
#* File Description:
#*   Include file for basic macros used in the Makefile.
#*
#**********************************************************

CC        = g++ -c
CXX        = g++ -c
CFLAGS     = -g -D_IS_HART7 -DLINUX -D_FLOWDEVICE -I$(MAIN_DIR)/Commands -I$(MAIN_DIR)/Data -I$(MAIN_DIR)/hipserver/hipserver/Common   -I$(MAIN_DIR)/hipserver/hipserver/AppConnect  -I$(MAIN_DIR)/hipserver/hipserver/Data -I$(MAIN_DIR)/hipserver/hipserver/Realtime -I(MAIN_DIR)hipserver/hipserver/safestringlib -I$(MAIN_DIR)  -std=gnu++11 
CXXFLAGS   = -g  -D_IS_HART7 -DLINUX -D_FLOWDEVICE -I$(MAIN_DIR)/Commands -I$(MAIN_DIR)/Data -I$(MAIN_DIR)/hipserver/hipserver/Common   -I$(MAIN_DIR)/hipserver/hipserver/AppConnect  -I$(MAIN_DIR)/hipserver/hipserver/Data  -I$(MAIN_DIR)/hipserver/hipserver/Realtime -I(MAIN_DIR)hipserver/hipserver/safestringlib -I$(MAIN_DIR) -std=gnu++11 
ECHO       = @echo
INCL_LIBS  = -lpthread -lrt hipserver/hipserver/safestringlib/libsafestring.a
LD         = g++ 
RM         = @rm

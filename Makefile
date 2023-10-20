# Project Name
TARGET = AIBoiler

#APP_TYPE = BOOT_NONE

# Sources
CPP_SOURCES = AIBoiler.cpp 

C_SOURCES = $(shell ls App/*.c)

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

C_INCLUDES += \
-I. \
-I./AI/Inc \
-I./App

LDFLAGS += -L./AI/Lib -l:libNetworkRuntime810_CM7_GCC.a

OPT = -O0

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile




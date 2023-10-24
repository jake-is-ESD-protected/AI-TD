# Project Name
TARGET = AIBoiler

#APP_TYPE = BOOT_NONE

# Sources
CPP_SOURCES += src/main.cpp
CPP_SOURCES += src/hal/hal.cpp

C_SOURCES = $(shell ls lib/AI_model/*.c)

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

C_INCLUDES += \
-I./Ai_driver/inc \
-I./AI/Inc \
-I./lib/AI_model\
-I./src/hal

LDFLAGS += -L./Ai_driver/lib -l:libNetworkRuntime810_CM7_GCC.a

OPT = -O0

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
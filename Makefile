# Project Name
TARGET = AITD

#APP_TYPE = BOOT_NONE

# Sources
CPP_SOURCES += src/main.cpp
CPP_SOURCES += src/hal/hal.cpp
CPP_SOURCES += src/transientDSP/transientDSP.cpp
CPP_SOURCES += src/ui/ui.cpp
CPP_SOURCES += src/transientDSP/EnvelopeFollowerPeakHold.cpp
CPP_SOURCES += src/transientDSP/SmootherExponential.cpp
CPP_SOURCES += src/cli/cli.cpp
CPP_SOURCES += src/transientDSP/envDeps/BufferCircular.cpp
CPP_SOURCES += src/ai/ai.cpp
CPP_SOURCES += seedcli/seedcli_src/cli.cpp
CPP_SOURCES += seedcli/seedcli_src/mem.cpp

C_SOURCES = $(shell ls lib/AI_model/*.c)

C_INCLUDES += \
-I./lib/Ai_driver/inc \
-I./lib/AI_model \
-I./src/transientDSP \
-I./src/hal \
-I./src/ui \
-I./src/ai \
-I$(LIBK_DIR) \
-I./seedcli/seedcli_src

LDFLAGS += -L./lib/Ai_driver/lib -l:libNetworkRuntime810_CM7_GCC.a

OPT = -O0

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
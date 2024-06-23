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
CPP_SOURCES += src/transientDSP/envDeps/BufferCircular.cpp
CPP_SOURCES += src/ai/ai.cpp
CPP_SOURCES += seedcli/seedcli_src/cli.cpp
CPP_SOURCES += seedcli/seedcli_src/mem.cpp

C_SOURCES += src/af/af.c
C_SOURCES += $(shell ls src/af/beatDetectionDSP/*.c)
C_SOURCES += $(shell ls src/af/transientDetectionDSP/*.c)
C_SOURCES += $(shell ls src/af/audioFeatureDSP/*.c)
C_SOURCES += $(shell ls src/af/percentileCalculator/*.c)
C_SOURCES += $(shell ls src/af/transientDetectionDSP/envDeps/*.c)

C_INCLUDES += \
-I./src/transientDSP \
-I./src/hal \
-I./src/ui \
-I./src/ai \
-I$(LIBK_DIR) \
-I./seedcli/seedcli_src \
-I./src/af \
-I./RTNeural/RTNeural

OPT = -O2

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

CPPFLAGS += -DRTNEURAL_NAMESPACE=RTNeural -DRTNEURAL_DEFAULT_ALIGNMENT=8 -DRTNEURAL_NO_DEBUG=1
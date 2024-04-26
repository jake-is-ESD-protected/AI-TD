TARGET_EXEC = AFInC.dll



BUILD_DIR = build

SRCS ?= af.c #BEWARE INC FOR CPP IS NOT IMPLEMENTED YET DUE TO LINKING ERROR
SRCS += $(shell ls beatDetectionDSP/*.c)
SRCS += $(shell ls transientDetectionDSP/*.c)
SRCS += $(shell ls audioFeatureDSP/*.c)
SRCS += $(shell ls percentileCalculator/*.c)
SRCS += $(shell ls transientDetectionDSP/envDeps/*.c)

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(info $(OBJS))

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -fPIC

LDFLAGS = -shared

#CPP = C:\msys64\mingw64\bin\g++.exe
CC = gcc
CFLAGS = -fPIC

TARGET_DEVICE_DAISY = 1
CFLAGS += -DTARGET_DEVICE_DAISY

$(TARGET_EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS) 


$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CPP) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -f AFInC.dll

-include $(DEPS)

MKDIR_P ?= mkdir -p
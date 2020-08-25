########################################################################################################################
# Build config
########################################################################################################################

#
# Possible archs:
# 	- amd64
#
ARCH ?= amd64

#
# Possible platforms:
# 	- linux
# 	- windows
#
PLATFORM ?= windows

#
# Enable debug stuff
#
DEBUG ?= 0

########################################################################################################################
# Build constants
########################################################################################################################

CFLAGS := -Wall -Werror
CFLAGS += -O3 -flto -g
CFLAGS += -Isrc -Ideps/openvr/headers

LDFLAGS := -shared

# Game sources
SRCS += $(shell find src -iname '*.cpp')

ifeq ($(DEBUG), 1)
	OUT_DIR := out/$(PLATFORM)/$(ARCH)/DEBUG/
	BUILD_DIR := build/$(PLATFORM)/$(ARCH)/DEBUG

	CFLAGS += -D_DEBUG
	CFLAGS += -D__DEBUG__
#	CFLAGS += -fsanitize=address
#	CFLAGS += -fsanitize=undefined
else
	OUT_DIR := out/$(PLATFORM)/$(ARCH)/RELEASE/
	BUILD_DIR := build/$(PLATFORM)/$(ARCH)/RELEASE

	CFLAGS += -DNDEBUG
endif

include makefiles/$(PLATFORM)-$(ARCH).mk

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

MISC_FILES := $(shell find driver -type f)

########################################################################################################################
# Phony
########################################################################################################################

.PHONY: all clean clean-all

all: $(DRIVER_BINARY)
	cp $(MISC_FILES) $(OUT_DIR)

########################################################################################################################
# Targets
########################################################################################################################

-include $(DEPS)

# Link it
$(DRIVER_BINARY): $(BINS) $(OBJS)
	@echo LD $@
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Build C++ files
$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo CC $@
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -D__FILENAME__="$<" -c $< -o $@

# Clean the build files
clean:
	rm -rf build

# Clean everything, including the output directory
clean-all: clean
	rm -rf out

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
CFLAGS += -Isrc

LDFLAGS := -shared

# Game sources
SRCS += $(shell find src -iname '*.cpp')

ifeq ($(DEBUG), 1)
	BIN_DIR := bin/DEBUG
	BUILD_DIR := build/$(PLATFORM)/$(ARCH)/DEBUG

	CFLAGS += -D_DEBUG
	CFLAGS += -D__DEBUG__
#	CFLAGS += -fsanitize=address
#	CFLAGS += -fsanitize=undefined
else
	BIN_DIR := bin/RELEASE
	BUILD_DIR := build/$(PLATFORM)/$(ARCH)/RELEASE

	CFLAGS += -DNDEBUG
endif

include makefiles/$(PLATFORM)-$(ARCH).mk

########################################################################################################################
# Phony
########################################################################################################################

.PHONY: all clean clean-all

all: $(BIN_DIR)/$(PLATFORM)_$(ARCH)_pmfbt_driver.$(EXTENSION)

########################################################################################################################
# Targets
########################################################################################################################

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

-include $(DEPS)

$(BIN_DIR)/$(PLATFORM)_$(ARCH)_pmfbt_driver.$(EXTENSION): $(BINS) $(OBJS)
	@echo LD $@
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo CC $@
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -MMD -D__FILENAME__="$<" -c $< -o $@

clean:
	rm -rf build

clean-all: clean
	rm -rf bin

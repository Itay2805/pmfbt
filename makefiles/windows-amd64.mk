# Setup the compiler, linker and target
CC := x86_64-w64-mingw32-g++

LD := $(CC)

# OpenVR client library
CFLAGS += -DLINUX -DPOSIX -DLINUX64
LDFLAGS += deps/openvr/lib/win64/openvr_api.lib

# The binary extension
DRIVER_BINARY := $(BIN_DIR)/win64/pmfbt.dll

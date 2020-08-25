# Setup the compiler, linker and target
CC := clang++-10
CFLAGS += -target x86_64-pc-linux

LD := $(CC)
LDFLAGS += -target x86_64-pc-linux
LDFLAGS += -fuse-ld=lld

# OpenVR client library
LDFLAGS += deps/openvr/lib/linux64/openvr_api.so

# The binary extension
DRIVER_BINARY := $(OUT_DIR)/bin/win64/pmfbt.so

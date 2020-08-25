# Setup the compiler, linker and target
CC := clang++-10
CFLAGS += -target x86_64-pc-windows-gnu

LD := $(CC)
LDFLAGS += -target x86_64-pc-windows-gnu
LDFLAGS += -fuse-ld=lld

# OpenVR client library
CFLAGS += -DLINUX -DPOSIX -DLINUX64
LDFLAGS += deps/openvr/lib/win64/openvr_api.lib

# The binary extension
DRIVER_BINARY := $(OUT_DIR)/bin/win64/pmfbt.dll

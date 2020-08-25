# Setup the compiler, linker and target
CC := clang++-10
CFLAGS += -target x86_64-pc-linux

LD := $(CC)
LDFLAGS += -target x86_64-pc-linux
LDFLAGS += -fuse-ld=lld

# The binary extension
EXTENSION := so

# Setup the compiler, linker and target
CC := clang++-10
CFLAGS += -target x86_64-pc-windows-gnu

LD := $(CC)
LDFLAGS += -target x86_64-pc-windows-gnu
LDFLAGS += -fuse-ld=lld

# The binary extension
EXTENSION := dll

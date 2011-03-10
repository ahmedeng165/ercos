CPPFLAGS+=-I $R/os/ercos/hals/m68k/include

SRCS+=$(wildcard $R/os/ercos/hals/m68k/src/*.c)
SRCS+=$(wildcard $R/os/ercos/hals/m68k/src/*.S)
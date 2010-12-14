CPPFLAGS+= -I $R/os/ercos/hals/sparc/include

SRCS+=$(wildcard $R/os/ercos/hals/sparc/src/*.c)
#SRCS+=$(wildcard $R/os/ercos/src/ports/sparc/*.S)

ifeq ($(CONFIG_FAST_MODE), n)
SRCS+=$R/os/ercos/hals/sparc/src/context2.S
SRCS+=$R/os/ercos/hals/sparc/src/isr2.S
SRCS+=$R/os/ercos/hals/sparc/src/syscall2.S
SRCS+=$R/os/ercos/hals/sparc/src/window2.S
else
SRCS+=$R/os/ercos/hals/sparc/src/context.S
SRCS+=$R/os/ercos/hals/sparc/src/isr.S
SRCS+=$R/os/ercos/hals/sparc/src/syscall.S
SRCS+=$R/os/ercos/hals/sparc/src/window.S
endif
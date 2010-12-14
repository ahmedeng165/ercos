CPPFLAGS+= -I $R/os/ercos/hals/sparcpro/include

SRCS+=$(wildcard $R/os/ercos/hals/sparcpro/src/*.c)
#SRCS+=$(wildcard $R/os/ercos/src/ports/sparcpro/*.S)

ifeq ($(CONFIG_FAST_MODE), n)
SRCS+=$R/os/ercos/hals/sparcpro/src/context2.S
SRCS+=$R/os/ercos/hals/sparcpro/src/isr2.S
SRCS+=$R/os/ercos/hals/sparcpro/src/syscall2.S
SRCS+=$R/os/ercos/hals/sparcpro/src/window2.S
else
SRCS+=$R/os/ercos/hals/sparcpro/src/context.S
SRCS+=$R/os/ercos/hals/sparcpro/src/isr.S
SRCS+=$R/os/ercos/hals/sparcpro/src/syscall.S
SRCS+=$R/os/ercos/hals/sparcpro/src/window.S
endif

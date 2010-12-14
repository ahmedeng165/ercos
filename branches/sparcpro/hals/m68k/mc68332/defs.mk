CPPFLAGS+= -I $R/os/ercos/hals/m68k/mc68332/include
	
SRCS+=$(wildcard $R/os/ercos/hals/m68k/mc68332/src/*.c)
SRCS+=$(wildcard $R/os/ercos/hals/m68k/mc68332/src/*.S)
	
ifeq ($(CONFIG_ERCOS_HAL_MC68332_GDBSTUB), y)
SRCS+=$(wildcard $R/os/ercos/hals/m68k/mc68332/src/gdb/*.c)
SRCS+=$(wildcard $R/os/ercos/hals/m68k/mc68332/src/gdb/*.S)
endif

LDSCRIPT_IN:=$R/os/ercos/hals/m68k/mc68332/src/link.in
LDSCRIPT:=$(patsubst %.in,%.ld,$(LDSCRIPT_IN))

LDFLAGS+= -nostdlib -M -T $(LDSCRIPT) 
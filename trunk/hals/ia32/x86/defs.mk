CPPFLAGS+= -I$R/os/ercos/hals/ia32/x86/include
	
SRCS+=$(wildcard $R/os/ercos/hals/ia32/x86/src/*.c)
SRCS+=$(wildcard $R/os/ercos/hals/ia32/x86/src/*.S)
SRCS+=$(wildcard $R/os/ercos/hals/ia32/x86/src/console/*.c)
	
LDSCRIPT_IN:=$R/os/ercos/hals/ia32/x86/src/link.in
LDSCRIPT:=$(patsubst %.in,%.ld,$(LDSCRIPT_IN))

LDFLAGS+= -nostdlib -M -T $(LDSCRIPT) 
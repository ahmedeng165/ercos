CPPFLAGS+= -I $R/os/ercos/hals/sparcpro/leon2/include
	
#	SRCS+=$R/os/ercos/hals/arch_sparcpro/MP_leon2/src/crt0.S
SRCS+=$(wildcard $R/os/ercos/hals/sparcpro/leon2/src/*.S)
SRCS+=$(wildcard $R/os/ercos/hals/sparcpro/leon2/src/*.c)
	
LDSCRIPT_IN:=$R/os/ercos/hals/sparcpro/leon2/src/link.in
LDSCRIPT:=$(patsubst %.in,%.ld,$(LDSCRIPT_IN))

LDFLAGS+= -M -T $(LDSCRIPT)
ASMFLAGS+= -DST_DIV0=0x2 -DASM -DASSEMBLY

CPPFLAGS+= -I $R/os/ercos/hals/sparc/leon3/include
	
#	SRCS+=$R/os/ercos/hals/arch_sparc/MP_leon3/src/crt0.S
SRCS+=$(wildcard $R/os/ercos/hals/sparc/leon3/src/*.S)
SRCS+=$(wildcard $R/os/ercos/hals/sparc/leon3/src/*.c)
	
LDSCRIPT_IN:=$R/os/ercos/hals/sparc/leon3/src/link.in
LDSCRIPT:=$(patsubst %.in,%.ld,$(LDSCRIPT_IN))

LDFLAGS+= -M -T $(LDSCRIPT)
ASMFLAGS+= -DST_DIV0=0x2 -DASM -DASSEMBLY
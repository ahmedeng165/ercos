CPPFLAGS+= -I $R/os/ercos/hals/sparc/erc32/include
	
SRCS+=$R/os/ercos/hals/sparc/erc32/src/gdebug.S
SRCS+=$R/os/ercos/hals/sparc/erc32/src/crt0.S
SRCS+=$(wildcard $R/os/ercos/hals/sparc/erc32/src/*.c)
#	SRCS+=$(wildcard $R/os/ercos/hals/sparc/erc32/src/*.S)
	
LDSCRIPT_IN:=$R/os/ercos/hals/sparc/erc32/src/link.in
LDSCRIPT:=$(patsubst %.in,%.ld,$(LDSCRIPT_IN))

LDFLAGS+= -M -T $(LDSCRIPT)
ASMFLAGS+= -DST_DIV0=0x2 -DASM -DASSEMBLY
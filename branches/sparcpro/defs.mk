CPPFLAGS+= -I$R/os/ercos/include 

SRCS+=$(wildcard $R/os/ercos/src/*.c)
SRCS+=$(wildcard $R/os/ercos/src/*.cpp)
SRCS+=$(wildcard $R/os/ercos/lib/stdlib/*.c)
SRCS+=$(wildcard $R/os/ercos/src/posix/*.c)

#--------------------- FOR SPARC ------------------------
ifeq ($(CONFIG_ARCH_SPARC), y)

#LIBGCC:=#/opt/rtems-4.10/lib/gcc/sparc-rtems/4.3.3/v8/libgcc.a
LIBGCC:=#$(shell $(CC) -print-libgcc-file-name)

include $R/os/ercos/hals/sparc/defs.mk

ifeq 	($(CONFIG_MP_ERC32), y)

include $R/os/ercos/hals/sparc/erc32/defs.mk	

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_ERC32 == y

ifeq 	($(CONFIG_MP_LEON2), y)

include $R/os/ercos/hals/sparc/leon2/defs.mk

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_LEON2 == y

ifeq 	($(CONFIG_MP_LEON3), y)
	
include $R/os/ercos/hals/sparc/leon3/defs.mk

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_LEON3 == y

endif # CONFIG_ARCH_SPARC == y

#--------------------- FOR SPARCPRO ------------------------
ifeq ($(CONFIG_ARCH_SPARCPRO), y)

#LIBGCC:=#/opt/rtems-4.10/lib/gcc/sparc-rtems/4.3.3/v8/libgcc.a
LIBGCC:=#$(shell $(CC) -print-libgcc-file-name)

include $R/os/ercos/hals/sparcpro/defs.mk

ifeq 	($(CONFIG_MP_ERC32), y)

include $R/os/ercos/hals/sparcpro/erc32/defs.mk	

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_ERC32 == y

ifeq 	($(CONFIG_MP_LEON2), y)

include $R/os/ercos/hals/sparcpro/leon2/defs.mk

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_LEON2 == y

ifeq 	($(CONFIG_MP_LEON3), y)
	
include $R/os/ercos/hals/sparcpro/leon3/defs.mk

ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif # CONFIG_MP_LEON3 == y

endif # CONFIG_ARCH_SPARCPRO == y

#--------------------- FOR IA32 ------------------------
ifeq ($(CONFIG_ARCH_IA32), y)
 
LIBGCC:=$(shell $(CC) $(CFLAGS) -mcpu32 -print-libgcc-file-name)

include $R/os/ercos/hals/ia32/defs.mk


ifeq 	($(CONFIG_MP_X86), y)

include $R/os/ercos/hals/ia32/x86/defs.mk	
	
ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
			#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif #CONFIG_MP_X86 == y

endif # CONFIG_ARCH_IA32 == y

#--------------------- FOR M68K ------------------------
ifeq ($(CONFIG_ARCH_M68K), y)
 
LIBGCC:=#$(shell $(CC) $(CFLAGS) -mcpu32 -print-libgcc-file-name)

include $R/os/ercos/hals/m68k/defs.mk


ifeq 	($(CONFIG_MP_MC68332), y)

include $R/os/ercos/hals/m68k/mc68332/defs.mk	
	
ifeq 		($(CONFIG_BOARD_UNDEFINED), y)
			#nothing here at the moment
endif #CONFIG_BOARD_UNDEFINED == y

endif #CONFIG_MP_MC68332 == y

endif # CONFIG_ARCH_M68K == y

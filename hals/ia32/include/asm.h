#ifndef ASM_H_
#define ASM_H_

#include <public/glue.h>

/* This is a more reliable delay than a few short jmps. */
#define iodelay() \
	asm("pushl %eax; inb $0x80,%al; inb $0x80,%al; popl %eax")

#define inportb(port,val) asm volatile ("inb %%dx, %%al" : "=a" (val) : "d" (port))

#define outportb(port,val) asm volatile("outb %%al, %%dx" : : "d" (port), "a" (val))

//for video memory
#define outb_p(port, val) ({	\
	outportb(port, val);	\
	iodelay();		\
})
static inline uint16_t inportw(uint16_t port){
         // Get a byte from I/O port
         register uint16_t val;
         asm volatile ("inw %%dx, %%ax" : "=a" (val) : "d" (port));
         return( val );
}

static inline void outportw(uint16_t port, uint16_t val){

         asm volatile("outw %%ax, %%dx" : : "d" (port), "a" (val));

}


static inline void CPUID(uint32_t op,uint32_t eax, uint32_t ebx, uint32_t ecx,
																uint32_t edx){
		 asm volatile("movl %0, %%eax \n cpuid "
                : "=a" (eax),
                  "=S" (ebx),
                  "=c" (ecx),
                  "=d" (edx)
                : "a" (op));

//Bit 9 de edx nos dice si hay LAPIC (1) o no (0)
}

static inline void ltr(uint16_t selector)
{
   asm volatile ("ltr %0": :"a" (selector));
}

static inline uint32_t str(void)
{
   uint32_t selector;

   asm volatile ("str %0":"=a" (selector));
   return selector;
}


/* Macros para leer registros especificos */
#define rdmsr(msr,a,d) \
     asm volatile("rdmsr" \
		          : "=a" (a), "=d" (d) \
                  : "c" (msr))

#define wrmsr(msr,a,d) \
     asm volatile("wrmsr" \
				  : /* no outputs */ \
				  : "c" (msr), "a" (a), "d" (d))

/*	Macros de lectura de marcas de tiempo del TSC	*/

#define rdtsc(low,high) \
     asm volatile("rdtsc" : "=a" (low), "=d" (high))

#define rdtscl(low) \
     asm volatile ("rdtsc" : "=a" (low) : : "edx")

#define rdtscll(val) \
     asm volatile ("rdtsc" : "=A" (val))


/* Funciones escritura/lectura de memoria */

static inline void writeb(uint8_t b, volatile void *addr){

	*(volatile uint8_t *) addr = b;
}

static inline void writew(uint16_t b, volatile void *addr){

	*(volatile uint16_t *) addr = b;
}

static inline void writel(uint32_t b, volatile void *addr){

	*(volatile uint32_t *) addr = b;
}

static inline uint8_t readb(const volatile void *addr){

	return *(volatile uint8_t *) addr;
}

static inline uint16_t readw(const volatile void *addr){

	return *(volatile uint16_t *) addr;
}

static inline uint32_t readl(const volatile void *addr){

	return *(volatile uint32_t *) addr;
}


#endif /*ASM_H_*/

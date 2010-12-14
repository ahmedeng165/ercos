
#include <public/config.h>

extern uint32_t mc68332_vector_table[256];

void putDebugChar (char c) {
	mc68332_uart_tx (c);
}

int getDebugChar (void) {
	return mc68332_uart_rx ();
}

void exceptionHandler (int exception_number, void *exception_address) {
	mc68332_vector_table[exception_number] = (uint32_t) exception_address;
}

void * exceptionHook = 0;


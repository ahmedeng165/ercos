/*
 * contextswitch.c
 *
 */

#include <context.h>	/* function definition */
#include <public/ercos-lah.h> /* pointers to current's & old_current's contexts */

void CPU_Switch_Tasks(void){

	_OS_Context_Switch(&(old_current->context), &(current->context));

}

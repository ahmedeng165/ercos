/*
 * i386 defines
 *
 * Copyright 2009 (c) by Alberto Carrasco Gallardo,
 * Space Research Group,
 * University of Alcala.
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 * This software is provided under the terms of the GNU General Public v2
 * Licence. A full copy of the GNU GPL is provided in the file COPYING
 * found in the development root of ERCOS-RT project.
 */
#ifndef ERCOSIA32_H_
#define ERCOSIA32_H_


#include <public/config.h>

//Numero de irqs en el sistema, necesario para calcular el tamaño total de memoria
//reservada para sus pilas(minimo 2 irqs... timer y one-shot)
#define IRQ_NUM	 2		/*TODO (config item??)*/
#define HAL_TOTAL_IRQS (IRQ_NUM)
#define HAL_IRQ_STACK_TOTAL_SIZE (CONFIG_ERCOS_IRQ_STACK_SIZE*HAL_TOTAL_IRQS)

#define NUM_TABLES (CONFIG_MEM/(CONFIG_PAGE_SIZE*CONFIG_ITEMS_PER_TABLE))

#endif /* ERCOSIA32_H_ */

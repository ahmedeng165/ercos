/*
 * i386 paging related functions for ERCOS-RT
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
#include <ercos/lib/string.h>
#include <ercos/lib/stdio.h>
#include <public/config.h>
#include <x86/x86.h>
#include <x86/paging.h>
#include <x86/crt0.h>			//paging enable & paging disable
/** internal variables **/

/*******************************************************************************
 *************************		FUNCIONES MEM VIRTUAL	************************
 *******************************************************************************
 */
extern pd_t page_dir;
extern pt_t page_tables[NUM_TABLES];

static page_index_t get_page_index(uint32_t p) {

	page_index_t indice;

	indice.dir_index = p / (CONFIG_PAGE_SIZE * CONFIG_ITEMS_PER_TABLE);
	indice.tabla_index = (p % (CONFIG_PAGE_SIZE * CONFIG_ITEMS_PER_TABLE)) / CONFIG_PAGE_SIZE;

	return indice;
}

static pde_t crear_pde(uint32_t dir, uint16_t atrib) {

	pde_t entrada;

	entrada = (atrib & 0xfff);
	entrada |= (dir & PAGE_FRAME);

	return entrada;
}

static pte_t crear_pte(uint32_t dir, uint16_t atrib) {

	pte_t entrada;

	entrada = (atrib & 0xfff);
	entrada |= (dir & PAGE_FRAME);

	return entrada;
}

static void load_cr3(uint32_t * val) {

	asm volatile("movl %0, %%cr3" : : "a"(val));
}

static uint32_t get_cr3(void) {

	uint32_t valor;

	asm volatile("movl %%cr3, %0" : "=a" (valor): );

	return valor;
}

static uint32_t* get_cr2(void) {

	uint32_t *valor;

	asm volatile("movl %%cr2, %0" : "=a" (valor): );

	return valor;
}

/*******************************************************************************
 **********************			FUNCIONES UTILES		************************
 ******************************************************************************/

/*! Vaciar una tabla entera */
static void vaciar_tabla(uint32_t *ptr) {

	memset(ptr, 0, CONFIG_ITEMS_PER_TABLE);

	return;
}

void init_paging(void) {

	uint32_t i, j;
	page_index_t apic_index;

	DEBUG("habilitando paginaci�n...\n");

	vaciar_tabla((uint32_t *) &page_dir);

	DEBUG("Directorio de páginas en dirección: 0x%x\n", (uint32_t)&page_dir);
	DEBUG("Numero de tablas: %d\n", NUM_TABLES);

	for (j = 0; j < NUM_TABLES; j++) {

		vaciar_tabla((uint32_t *) &page_tables[j]);

		DEBUG("Tabla[%d] en dirección: 0x%x", j, (uint32_t)&page_tables[j]);

		page_dir.entry[j] = crear_pde((uint32_t) ((uint32_t) &page_tables[j]),
				(P_PRESENT| P_WRITE));

		for (i = 0; i < CONFIG_ITEMS_PER_TABLE; i++) {

			page_tables[j].entry[i]
					= crear_pte((uint32_t) ((j * CONFIG_PAGE_SIZE * CONFIG_ITEMS_PER_TABLE)
							+ (i * CONFIG_PAGE_SIZE)), (P_PRESENT| P_WRITE));

		}

	}

	//Map the Local APIC addresses.
	apic_index = get_page_index(CONFIG_APIC_BASE);
	page_dir.entry[apic_index.dir_index] = crear_pde(
			(uint32_t) ((uint32_t) &page_tables[NUM_TABLES]),
			(uint16_t)(P_PRESENT| P_WRITE));
	page_tables[NUM_TABLES].entry[apic_index.tabla_index] = crear_pte(
			(uint32_t)CONFIG_APIC_BASE, (uint16_t)(P_PRESENT| P_WRITE));


	// Set CR3 to the page directory,
	load_cr3((uint32_t *) &page_dir);

	paging_enable();

	return;
}

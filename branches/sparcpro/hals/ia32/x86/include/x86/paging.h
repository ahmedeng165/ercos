/*
 * i386 paging functions declarations and macros for ERCOS-RT
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
#ifndef PAGING_H_
#define PAGING_H_

#include <public/glue.h>
#include <public/config.h>

//! Page present flag.
#define P_PRESENT       0x01			//0000000000000000000000000000001
//! Page writeable flag.
#define P_WRITE         0x02			//0000000000000000000000000000010
//! Page at user privilege level.
#define P_USER          0x04			//0000000000000000000000000000100
//! Page accessed flag.
#define P_ACCESSED      0x20			//0000000000000000000000000100000
//! Page dirty flag (the page has been written).
#define P_DIRTY         0x40			//0000000000000000000000001000000
//! 4 MBytes Page
#define P_PS			0x80			//0000000000000000000000010000000
//! Available bits
#define P_AVL			0x700			//0000000000000000000011100000000
//! Page frame
#define PAGE_FRAME		0xFFFFF000  	//1111111111111111111000000000000

typedef uint32_t pte_t;
typedef uint32_t pde_t;

typedef struct pt_t {

	pte_t entry[CONFIG_ITEMS_PER_TABLE];
} pt_t;

typedef struct pd_t {

	pde_t entry[CONFIG_ITEMS_PER_TABLE];
} pd_t;

typedef struct page_index_t {
	uint16_t dir_index;
	uint16_t tabla_index;
} page_index_t;

/*! Funcion que obtiene los indices en las tablas de directorios y paginas para
 *  una direccion dada
 *	\param	direccion de 32bits
 */
//page_index_t get_page_index( uint32_t);

/*! Funcion para vaciar una tabla(de paginas o directorio)
 *  \param	puntero a la tabla
 */
//void vaciar_tabla(uint32_t *ptr);

/*! Funcion que convierte una direccion logica en una direccion fisica usando
 *  paginacion.
 *  \param dir_logica: direccion logica de 32 bits
 *  \param cr3: direccion base del directorio de tablas de poginas
 *  \return	dir_fosica direccion fisica que se corresponde con la logica.
 */
//void *convertir_direccion(void *dir_logica, uint32_t cr3);

/*! Funcion para crear una entrada en el directorio de paginas
 *  \param dir: direccion logica
 *  \param atrib: atributos de la entrada del directorio
 */
//pde_t crear_pde(uint32_t dir, uint16_t atrib);

/*! Funcion para crear una entrada en una tabla de paginas
 *  \param dir: direccion logica
 *  \param atrib: atributos de la entrada de la tabla
 */
//pte_t crear_pte(uint32_t dir, uint16_t atrib);

/*! Funcion para iniciar y habilitar la paginacion
 */
void init_paging(void);

/*! Funcion para cargar un valor en cr3(directorio de paginas)	*/
//void load_cr3(uint32_t * val);

/*! Funcion para leer el valor de cr3	*/
//uint32_t get_cr3(void);

/*! Funcion para leer el valor de cr2	*/
//uint32_t* get_cr2(void);

/*! habilita la paginacion... bit 31 de cr0 */
extern void paging_enable(void);

/*! deshabilita la paginacion... bit 31 de Cr0 */
extern void paging_disable(void);

#endif /* PAGING_H_ */

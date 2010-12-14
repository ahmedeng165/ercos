
/*
    Copyright 2008 (c) by Cesar Rodriguz Ortega,
    Copyright 2005-2006 (c) by Aitor Viana Sanchez,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://atc1.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

/**
 * \file include/lib/crc32.h
 * \brief CRC-32-IEEE 802.3 calculation stuff
 */

#ifndef _LIB_CRC32_H_
#define _LIB_CRC32_H_

#include <public/glue.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t crc32 (const void * data, size_t num_bytes);
uint32_t crc32_append (uint32_t cache, const void * data, size_t num_bytes);

#ifdef __cplusplus
}
#endif

#endif

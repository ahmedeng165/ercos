
/*
    Copyright 2008 (c) by Martin Knoblauch, Cesar Rodriguez Ortega,
    University of Alcala,
    Computer Engineering Department.

    For further information, please visit http://srg.aut.uah.es

    This software is provided under the terms of the GNU General Public v2
    Licence. A full copy of the GNU GPL is provided in the file COPYING
    found in the development root of ERCOS-RT.
*/

#include <public/glue.h>
#include <ercos/lib/crc32.h>

static const uint32_t _crc32_table[16] = {
	0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC,
	0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
	0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C,
	0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C,
};

uint32_t crc32 (const void * data, size_t num_bytes) {
	return crc32_append (0, data, num_bytes);
}

uint32_t crc32_append (uint32_t cache, const void * data, size_t num_bytes) {
	uint32_t crc;
	unsigned char index;
	const unsigned char * bytes;

	for (crc=~cache, bytes=(const unsigned char*)data; num_bytes>0;
			bytes++, num_bytes--) {

		index = (unsigned char)crc ^ *bytes;
		crc = (crc >> 4) ^ _crc32_table[index&15];

		index = (unsigned char)crc ^ (*bytes >> 4);
		crc = (crc >> 4) ^ _crc32_table[index&15];
	}

	return ~ crc;
}

#if 0
/* this function initializes the table */
void init_crc32s_table (void) {
	unsigned crc, mask, byte;
	unsigned char bit;

	for (byte=0; byte<16; byte++) {
		crc = byte;

		for (bit=0; bit<4; bit++) {
			mask = (crc&1) * 0xEDB88320;
			crc = (crc >> 1) ^ mask;
		}

		crc32s_table[byte] = crc;
	}
}

/* this function prints the table */
void print_crc32s_table (void) {
	int i, j, k;

	for (i=k=0; i<4; i++) {
		for (j=0; j<4; j++, k++)
			printf (" 0x%08X,", crc32s_table[k]);

		printf ("\n");
	}
}
#endif


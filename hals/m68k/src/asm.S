
| Motorola cpu 68K helper code for the ERCOS HAL -- assembler functions
|
| Copyright 2007,2008 (c) by Cesar Rodriguez Ortega
| Space Research Group,
| University of Alcala,
| 
| For further information, please visit http://srg.aut.uah.es
| 
| This software is provided under the terms of the GNU General Public v2
| Licence. A full copy of the GNU GPL is provided in the file COPYING
| found in the development root.

.global m68k_get_sr
.global m68k_atomic_dec
.global m68k_atomic_inc
.global m68k_intr_disable
.global m68k_intr_enable
.global m68k_intr_level

| unsigned m68k_get_sr (void);
m68k_get_sr :
	move		%sr, %d0
	rts

| void m68k_atomic_inc (volatile long *i);
m68k_atomic_inc:
	move.l		(%sp,4), %a0
	add.l		#1, (%a0)
	rts

| void m68k_atomic_dec (volatile long *i);
m68k_atomic_dec:
	move.l		(%sp,4), %a0
	sub.l		#1, (%a0)
	rts

| void m68k_intr_disable (void);
m68k_intr_disable:
	move.w		%sr, %d0
	or.w		#0xf8ff, %d0
	move.w		%d0, (_m68k_sr_cache)
	ori		#0x0700, %sr
	rts

| void m68k_intr_enable (void);
m68k_intr_enable:
	move.w		%sr, %d0
	and.w		(_m68k_sr_cache), %d0
	move.w		%d0, %sr
	rts

| void m68k_intr_level (unsigned irl);
m68k_intr_level:
	and.w		#0x0700, (%sp,6)
	move.w		%sr, %d0
	and.w		#0xf8ff, %d0
	add.w		(%sp,6), %d0
	move.w		%d0, %sr
	rts

.section ".bss"
_m68k_sr_cache:		.space 2

| vim:ts=8:syn=asm68k:


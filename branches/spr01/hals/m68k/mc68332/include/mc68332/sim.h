
/*
 * Generic MC68332 board HAL for ERCOS-RT -- SIM handling macros
 *
 * Copyright 2008 (c) by Cesar Rodriguez Ortega,
 * Space Research Group,
 * University of Alcala.
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 * This software is provided under the terms of the GNU General Public v2
 * Licence. A full copy of the GNU GPL is provided in the file COPYING
 * found in the development root of ERCOS-RT project.
 */

#ifndef _ERCOS_HAL_MC68332_SIM_H_
#define _ERCOS_HAL_MC68332_SIM_H_

/* FIXME -- SIM register file addresses depends on the state of bit MM in SIMCR
 * register */

/* System Integration Module register management macros */
#define MC68332_SIM_BASE			0x00fff000

#define MC68332_SIM_MCR				0x00000a00
#define MC68332_SIM_SYNCR			0x00000a04
#define MC68332_SIM_SYPCR			0x00000a21
#define MC68332_SIM_PICR			0x00000a22
#define MC68332_SIM_PITR			0x00000a24
#define MC68332_SIM_SWSR			0x00000a27
#define MC68332_SIM_CSPAR0			0x00000a44
#define MC68332_SIM_CSPAR1			0x00000a46
#define MC68332_SIM_CSBARBT			0x00000a48
#define MC68332_SIM_CSORBT			0x00000a4a
#define MC68332_SIM_CSBAR0			0x00000a4c
#define MC68332_SIM_CSOR0			0x00000a4e
#define MC68332_SIM_CSBAR1			0x00000a50
#define MC68332_SIM_CSOR1			0x00000a52
#define MC68332_SIM_CSBAR2			0x00000a54
#define MC68332_SIM_CSOR2			0x00000a56
#define MC68332_SIM_CSBAR3			0x00000a58
#define MC68332_SIM_CSOR3			0x00000a5a
#define MC68332_SIM_CSBAR4			0x00000a5c
#define MC68332_SIM_CSOR4			0x00000a5e
#define MC68332_SIM_CSBAR7			0x00000a68
#define MC68332_SIM_CSOR7			0x00000a6a
#define MC68332_SIM_CSBAR8			0x00000a6c
#define MC68332_SIM_CSOR8			0x00000a6e

/* Queued Serial Module management macros */
#define MC68332_QSM_BASE			0x00fffc00

/* QSM global registers */
#define MC68332_QSM_QSMCR			0x00000000
#define MC68332_QSM_QILR			0x00000005
#define MC68332_QSM_QIVR			0x00000004

/* Serial Communication Interface registers */
#define MC68332_QSM_SCCR0			0x00000008
#define MC68332_QSM_SCCR1			0x0000000a
#define	MC68332_QSM_SCSR			0x0000000c
#define	MC68332_QSM_SCDR			0x0000000e

/* Time Processor Unit macros */
#define MC68332_TPU_BASE			0x00fffe00

/* TPU regiters */
#define MC68332_TPU_TPUMCR			0x00000000
#define MC68332_TPU_TICR			0x00000008
#define MC68332_TPU_CIER			0x0000000a
#define MC68332_TPU_CFSR0			0x0000000c
#define MC68332_TPU_CFSR1			0x0000000e
#define MC68332_TPU_CFSR2			0x00000010
#define MC68332_TPU_CFSR3			0x00000012
#define MC68332_TPU_HSQR0			0x00000014
#define MC68332_TPU_HSQR1			0x00000016
#define MC68332_TPU_HSRR0			0x00000018
#define MC68332_TPU_HSRR1			0x0000001a
#define MC68332_TPU_CPR0			0x0000001c
#define MC68332_TPU_CPR1			0x0000001e
#define MC68332_TPU_CISR			0x00000020

/* TPU parameter RAM */
#define MC68332_TPURAM_BASE			0x00ffff00

/* Generic write to a register file (8 & 16 bit) */
#define MC68332_WRITE8(base,reg,val) 	\
			*((volatile uint8_t *) ((base) + (reg))) = (uint8_t) (val)
#define MC68332_WRITE16(base,reg,val) 	\
			*((volatile uint16_t *) ((base) + (reg))) = (uint16_t) (val)

/* Generic read to a register file (8 & 16 bit) */
#define MC68332_READ8(base,reg) 	\
			(*((volatile uint8_t *) ((base) + (reg))))
#define MC68332_READ16(base,reg) 	\
			(*((volatile uint16_t *) ((base) + (reg))))

/* Read and write in the SIM register file */
#define MC68332_SIM_WRITE8(reg,val)	MC68332_WRITE8 (MC68332_SIM_BASE, reg, val)
#define MC68332_SIM_WRITE16(reg,val)	MC68332_WRITE16 (MC68332_SIM_BASE, reg, val)
#define MC68332_SIM_READ8(reg)		MC68332_READ8 (MC68332_SIM_BASE, reg)
#define MC68332_SIM_READ16(reg)		MC68332_READ16 (MC68332_SIM_BASE, reg)

/* Read and write in the QSM register file */
#define MC68332_QSM_WRITE8(reg,val)	MC68332_WRITE8 (MC68332_QSM_BASE, reg, val)
#define MC68332_QSM_WRITE16(reg,val)	MC68332_WRITE16 (MC68332_QSM_BASE, reg, val)
#define MC68332_QSM_READ8(reg)		MC68332_READ8 (MC68332_QSM_BASE, reg)
#define MC68332_QSM_READ16(reg)		MC68332_READ16 (MC68332_QSM_BASE, reg)

/* Read and write in the TPU register file */
#define MC68332_TPU_WRITE8(reg,val)	MC68332_WRITE8 (MC68332_TPU_BASE, reg, val)
#define MC68332_TPU_WRITE16(reg,val)	MC68332_WRITE16 (MC68332_TPU_BASE, reg, val)
#define MC68332_TPU_READ8(reg)		MC68332_READ8 (MC68332_TPU_BASE, reg)
#define MC68332_TPU_READ16(reg)		MC68332_READ16 (MC68332_TPU_BASE, reg)

/* Read and write in the TPURAM register file */
#define MC68332_TPURAM_WRITE8(reg,val)	MC68332_WRITE8 (MC68332_TPURAM_BASE, reg, val)
#define MC68332_TPURAM_WRITE16(reg,val)	MC68332_WRITE16 (MC68332_TPURAM_BASE, reg, val)
#define MC68332_TPURAM_READ8(reg)	MC68332_READ8 (MC68332_TPURAM_BASE, reg)
#define MC68332_TPURAM_READ16(reg)	MC68332_READ16 (MC68332_TPURAM_BASE, reg)

#endif


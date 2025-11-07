/*******************************************************************************
 * Copyright by http://eswin.com/.
 *
 * File Name:    
 * File Mark:    
 * Description:  
 * Others:        
 * Version:       V0.1
 * Author:        liuyong
 * Date:          2021-1-7
 * History 1:      
 *     Date: 
 *     Version:
 *     Author: 
 *     Modification:  
 * History 2: 
  ********************************************************************************/

#ifndef _COMMON_DEF_H
#define _COMMON_DEF_H



/****************************************************************************
* 	                             andes
****************************************************************************/
// Chkeck whether the following intrinsic functions are supported by toolchain.
// Some intrinsic functions are not supported by old toolchain and need to be wrapped.
#ifndef __ASSEMBLER__

#include <nds32_intrinsic.h>

#ifndef __nds32__mtsr_isb
#define __nds32__mtsr_isb(val, srname)  \
	do {__nds32__mtsr(val, srname);__nds32__isb();} while(0)
#endif

#ifndef __nds32__mtsr_dsb
#define __nds32__mtsr_dsb(val, srname)	\
	do {__nds32__mtsr(val, srname);__nds32__dsb();} while(0)
#endif

#endif

#ifdef __ASSEMBLER__
/************************************
 *	HAL : AE210P defined vectors	*
 ************************************/
.macro	hal_hw_vectors
		vector Interrupt_UNDEF	 //  (9) Interrupt HW0
		vector Interrupt_UNDEF	 // (10) Interrupt HW1
		vector Interrupt_UNDEF	 // (11) Interrupt HW2 (PIT)
		vector Interrupt_UNDEF	 // (12) Interrupt HW3
		vector Interrupt_UNDEF	 // (13) Interrupt HW4
		vector Interrupt_UNDEF	 // (14) Interrupt HW5
		vector Interrupt_UNDEF	 // (15) Interrupt HW6 (GPIO)
		vector Interrupt_UNDEF	 // (16) Interrupt HW7
		vector Interrupt_UNDEF	 // (17) Interrupt HW8
		vector Interrupt_UNDEF	 // (18) Interrupt HW9
		vector Interrupt_UNDEF	 // (19) Interrupt HW10
		vector Interrupt_UNDEF	 // (20) Interrupt HW11 (SWI)
		vector Interrupt_UNDEF	 // (21) Interrupt HW12
		vector Interrupt_UNDEF	 // (22) Interrupt HW13
		vector Interrupt_UNDEF	 // (23) Interrupt HW14
		vector Interrupt_UNDEF	 // (24) Interrupt HW15
		vector Interrupt_UNDEF	 // (25) Interrupt HW16
		vector Interrupt_UNDEF	 // (26) Interrupt HW17
		vector Interrupt_UNDEF	 // (27) Interrupt HW18
		vector Interrupt_UNDEF	 // (28) Interrupt HW19
		vector Interrupt_UNDEF	 // (29) Interrupt HW20
		vector Interrupt_UNDEF	 // (30) Interrupt HW21
		vector Interrupt_UNDEF	 // (31) Interrupt HW22
		vector Interrupt_UNDEF	 // (32) Interrupt HW23
		vector Interrupt_UNDEF	 // (33) Interrupt HW24
		vector Interrupt_UNDEF	 // (34) Interrupt HW25
		vector Interrupt_UNDEF	 // (35) Interrupt HW26
		vector Interrupt_UNDEF	 // (36) Interrupt HW27
		vector Interrupt_UNDEF	 // (37) Interrupt HW28
		vector Interrupt_UNDEF	 // (38) Interrupt HW29
		vector Interrupt_UNDEF	 // (39) Interrupt HW30
		vector Interrupt_UNDEF	 // (40) Interrupt HW31
.endm
#endif  /* __ASSEMBLER__ */
#ifndef NDS32_INTR_DEF
/*
 * NDS32_REG_SET_BITS(addr, mask)
 * Do set bits to 1 at specified location
 * Operation: *addr = (*addr | mask)
 */
#define NDS32_SET_BITS(addr, mask) outw(addr, inw(addr) | mask)

/*
 * NDS32_REG_WAIT4_BIT_ON(addr, mask)
 * Read the contents at addr and use mask to
 * mask off unused bits then wait until all of
 * the remaining bits are on
 *
 * Operation: while(!(*addr & mask));
 */
#define NDS32_REG_WAIT4_BIT_ON(addr, mask) while(0 == (inw(addr) & mask))

/*
 * NDS32_REG_WAIT4_BIT_OFF(addr, mask)
 * Read the contents at addr and use mask to
 * mask off unused bits then wait until all of
 * the remaining bits are off
 *
 * Operation: while((*addr & mask));
 */
#define NDS32_REG_WAIT4_BIT_OFF(addr, mask) while(inw(addr) & mask)

/*
 * NDS32_REG_WRITE(addr, data)
 * Write data to specified location at addr
 *
 * Operation: *addr = data
 * */
#define NDS32_REG_WRITE(addr, data) outw(addr, data)

/*
 * NDS32_FMEMCPY_BYTE(dst, src, size)
 * Do forward (low address to high address) memory copy in byte
 * */
#define NDS32_FMEMCPY_BYTE(dst, src, size) \
	__asm__ __volatile__( \
		"sethi   $r3, hi20(%2)       \n\t" \
		"ori     $r3, $r3, lo12(%2)  \n\t" \
		"sethi   $r1, hi20(%0)       \n\t" \
		"ori     $r1, $r1, lo12(%0)  \n\t" \
		"sethi   $r2, hi20(%1)       \n\t" \
		"ori     $r2, $r2, lo12(%1)  \n\t" \
		"1:\n\t" \
		"lbi.bi  $r0, [$r2], 1       \n\t" \
		"sbi.bi  $r0, [$r1], 1       \n\t" \
		"addi    $r3, $r3, -1        \n\t" \
		"bnez    $r3, 1b             \n\t" \
		::"i"(dst),"i"(src),"i"(size):"$r0", "$r1","$r2","$r3")

#define NDS32_FMEMCPY(dst, src, size) \
	__asm__ __volatile__( \
		"sethi   $r3, hi20(%2)       \n\t" \
		"ori     $r3, $r3, lo12(%2)  \n\t" \
		"sethi   $r1, hi20(%0)       \n\t" \
		"ori     $r1, $r1, lo12(%0)  \n\t" \
		"sethi   $r2, hi20(%1)       \n\t" \
		"ori     $r2, $r2, lo12(%1)  \n\t" \
		"1:\n\t" \
		"lwi.bi  $r0, [$r2], 4       \n\t" \
		"swi.bi  $r0, [$r1], 4       \n\t" \
		"addi    $r3, $r3, -4        \n\t" \
		"bgtz    $r3, 1b             \n\t" \
		::"i"(dst),"i"(src),"i"(size):"$r0", "$r1","$r2","$r3")

#endif


/****************************************************************************
* 	                                        Include files
****************************************************************************/


/****************************************************************************
* 	                                        Macros
****************************************************************************/
/* IO Address */
#define MIN(x, y)					(((x) > (y)) ? (y) : (x))
#define MAX(x, y)					(((x) > (y)) ? (x) : (y))



/* Direct address manipulation */
#define IN8(reg)					(char)( (*(volatile unsigned long *)(reg)) & 0x000000FF )
#define IN16(reg)					(short int)( (*(volatile unsigned long *)(reg)) & 0x0000FFFF )
#define IN32(reg)					(  *( (volatile unsigned int *) (reg) ) )

#define OUT8(reg, data)				((*((volatile unsigned char *)(reg)))=(unsigned char)(data))
#define OUT16(reg, data)			((*((volatile unsigned short int *)(reg)))=(unsigned short int)(data))
#define OUT32(reg, data)			( (*( (volatile unsigned int *) (reg) ) ) = (unsigned int)(data) )

#define inw(reg)					(*((volatile unsigned int *) (reg)))
#define outw(reg, data)				((*((volatile unsigned int *)(reg)))=(unsigned int)(data))
#define in8(reg) 					(char)( (*(volatile unsigned long *)(reg)) & 0x000000FF )


/* bit macro value */
#define BIT0    					(0x00000001L << 0)
#define BIT1    					(0x00000001L << 1)
#define BIT2    					(0x00000001L << 2)
#define BIT3   			 			(0x00000001L << 3)
#define BIT4    					(0x00000001L << 4)
#define BIT5    					(0x00000001L << 5)
#define BIT6    					(0x00000001L << 6)
#define BIT7    					(0x00000001L << 7)
#define BIT8    					(0x00000001L << 8)
#define BIT9    					(0x00000001L << 9)
#define BIT10   					(0x00000001L << 10)
#define BIT11  						(0x00000001L << 11)
#define BIT12   					(0x00000001L << 12)
#define BIT13   					(0x00000001L << 13)
#define BIT14   					(0x00000001L << 14)
#define BIT15   					(0x00000001L << 15)
#define BIT16   					(0x00000001L << 16)
#define BIT17   					(0x00000001L << 17)
#define BIT18   					(0x00000001L << 18)
#define BIT19   					(0x00000001L << 19)
#define BIT20   					(0x00000001L << 20)
#define BIT21   					(0x00000001L << 21)
#define BIT22   					(0x00000001L << 22)
#define BIT23   					(0x00000001L << 23)
#define BIT24   					(0x00000001L << 24)
#define BIT25   					(0x00000001L << 25)
#define BIT26   					(0x00000001L << 26)
#define BIT27   					(0x00000001L << 27)
#define BIT28   					(0x00000001L << 28)
#define BIT29   					(0x00000001L << 29)
#define BIT30   					(0x00000001L << 30)
#define BIT31   					(0x00000001L << 31)
#define BITZero    					(0x00000000L)
#define BIT(X)  					(0x00000001L << (X))


/* The following macro values can be used when memset and memcpy are not available */
#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

/****************************************************************************
* 	                                        Types
****************************************************************************/

/****************************************************************************
* 	                                        Constants
****************************************************************************/

/****************************************************************************
* 	                                        Global  Variables
****************************************************************************/

/****************************************************************************
* 	                                        Function Prototypes
****************************************************************************/



#endif/*_COMMON_DEF_H*/


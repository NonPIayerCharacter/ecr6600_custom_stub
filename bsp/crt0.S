##==============================================================================
##
##  crt0.S
##
##  nds32 startup code
##
##==============================================================================
##
## Copyright (c) 2012-2015 Andes Technology Corporation
## All rights reserved.
##

#include <nds32_init.inc>
#include "common_def.h"

##------------------------------------------------------------------------------
	.macro vector name
	.align 2
	j OS_Trap_\name
	.endm


	.section .nds32_init, "ax"
!========================================================================
! Vector table
!========================================================================
	.align 2
exception_vector:
	j _boot_start               !  (0) Trap Reset
	vector TLB_Fill             !  (1) Trap TLB fill
	vector PTE_Not_Present      !  (2) Trap PTE not present
	vector TLB_Misc             !  (3) Trap TLB misc
	vector TLB_VLPT_Miss        !  (4) Trap TLB VLPT miss
	vector Machine_Error        !  (5) Trap Machine error
	vector Debug_Related        !  (6) Trap Debug related
	vector General_Exception    !  (7) Trap General exception
	vector Syscall              !  (8) Syscall

	/* HW interrupt vector layout */
	hal_hw_vectors

	.align  2
exception_vector_end:

/*
 * exception handlers
 */

	.section .text

OS_Trap_TLB_Fill:
OS_Trap_PTE_Not_Present:
OS_Trap_TLB_Misc:
OS_Trap_TLB_VLPT_Miss:
OS_Trap_Machine_Error:
OS_Trap_Debug_Related:
OS_Trap_General_Exception:
OS_Trap_Syscall:
OS_Trap_Interrupt_UNDEF:
1:  b   1b

##------------------------------------------------------------------------------
## Startup code

	.global _boot_start
	.func _boot_start
	.type _boot_start, @function

	.align 2
_boot_start:
	!************************** Begin of do-not-modify **************************
	! Please don't modify this code
	! Initialize the registers used by the compiler

	nds32_init		! NDS32 startup initial macro in <nds32_init.inc>
	bal boot_c_init
	bal boot_main
	
	
1:  b   1b

	.size _boot_start, .-_boot_start

	.end

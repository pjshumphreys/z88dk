
;
; Invoke a GSX/GIOS function with 1 coordinate (2 values) in "ptsin"
;

;
; $Id: gios_1px.asm $
;

SECTION code_clib

PUBLIC	gios_1px
PUBLIC	_gios_1px

EXTERN ASMDISP_GIOS_1PX
EXTERN gios_1px_callee

EXTERN	gios_ptsin

gios_1px:
_gios_1px:

	pop	af
	pop	hl
	pop	bc
	pop de
	push de
	push bc
	push hl
	push af

	jp gios_1px_callee + ASMDISP_GIOS_1PX

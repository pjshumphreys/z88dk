
; void dzx1_smart_rcs_back_callee(void *src, void *dst)

SECTION code_clib
SECTION code_compress_zx1

PUBLIC _dzx1_smart_rcs_back_callee

EXTERN asm_dzx1_smart_rcs_back

_dzx1_smart_rcs_back_callee:

   pop af
   pop hl
   pop de
   push af

   jp asm_dzx1_smart_rcs_back

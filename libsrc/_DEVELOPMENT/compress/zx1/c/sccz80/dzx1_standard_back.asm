
; void dzx1_standard_back(void *src, void *dst)

SECTION code_clib
SECTION code_compress_zx1

PUBLIC dzx1_standard_back

EXTERN asm_dzx1_standard_back

dzx1_standard_back:

   pop af
   pop de
   pop hl

   push hl
   push de
   push af

   jp asm_dzx1_standard_back


; int p_forward_list_alt_empty_fastcall(p_forward_list_alt_t *list)

SECTION code_clib
SECTION code_adt_p_forward_list_alt

PUBLIC _p_forward_list_alt_empty_fastcall

EXTERN asm_p_forward_list_alt_empty

defc _p_forward_list_alt_empty_fastcall = asm_p_forward_list_alt_empty

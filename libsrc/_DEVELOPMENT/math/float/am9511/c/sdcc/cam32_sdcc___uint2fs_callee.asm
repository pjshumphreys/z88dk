
SECTION code_fp_am9511
PUBLIC cam32_sdcc___uint2fs_callee

EXTERN asm_am9511_float16u


.cam32_sdcc___uint2fs_callee
    pop bc      ;return
    pop hl      ;value
    push bc
    jp asm_am9511_float16u

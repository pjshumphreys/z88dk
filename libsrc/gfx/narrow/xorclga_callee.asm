;
;     Z88 Graphics Functions - Small C+ stubs
;     Written around the Interlogic Standard Library
;     Stubs Written by D Morris - 30/9/98
;
;    $Id: xorclga_callee.asm $
;


;Usage: xorclga(struct *pixels)


IF !__CPU_INTEL__ & !__CPU_GBZ80__
    SECTION    code_graphics
    
    PUBLIC    xorclga_callee
    PUBLIC    _xorclga_callee

    PUBLIC    ASMDISP_XORCLGA_CALLEE
    
    EXTERN    swapgfxbk
    EXTERN    __graphics_end
    
    EXTERN    xorclrarea
    INCLUDE    "graphics/grafix.inc"


.xorclga_callee
._xorclga_callee
    
    pop    af    ; ret addr
    pop bc    ; y2
    pop hl
    ld    b,l    ; x2
    pop hl    ; y
    pop de
    ld    h,e    ; x
    push    af    ; ret addr

.asmentry
    push    ix
IF NEED_swapgfxbk = 1
    call    swapgfxbk
ENDIF    
    call    xorclrarea
IF NEED_swapgfxbk
    jp      __graphics_end
ELSE
  IF !__CPU_INTEL__ & !__CPU_GBZ80__
    pop     ix
  ENDIF
    ret
ENDIF

DEFC ASMDISP_XORCLGA_CALLEE = asmentry - xorclga_callee
ENDIF

;
;     Z88 Graphics Functions - Small C+ stubs
;
;     Written around the Interlogic Standard Library
;
;
; ----- void __CALLEE__ undrawb(int x, int y, int h, int v)
;
;    $Id: undrawb_callee.asm $
;


IF !__CPU_INTEL__ & !__CPU_GBZ80__
    SECTION    code_graphics
    
    PUBLIC    undrawb_callee
    PUBLIC    _undrawb_callee

    PUBLIC    ASMDISP_UNDRAWB_CALLEE

    EXTERN    drawbox
    EXTERN    respixel
    EXTERN    swapgfxbk
    EXTERN      __graphics_end
    INCLUDE    "graphics/grafix.inc"


.undrawb_callee
._undrawb_callee
    pop af
    pop bc    ; height
    pop de
    ld    b,e    ; width
    pop hl    ; x
    pop    de
    ld    h,e    ; y    
    push af
    
.asmentry
    push ix
    ld    ix,respixel
IF NEED_swapgfxbk = 1
    call    swapgfxbk
ENDIF    
    call    drawbox
IF NEED_swapgfxbk
    jp      __graphics_end
ELSE
  IF !__CPU_INTEL__ & !__CPU_GBZ80__
    pop     ix
  ENDIF
    ret
ENDIF
DEFC ASMDISP_UNDRAWB_CALLEE = asmentry - undrawb_callee
ENDIF

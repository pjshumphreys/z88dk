;
;     Z88 Graphics Functions - Small C+ stubs
;
;     Written around the Interlogic Standard Library
;
;     Stubs Written by D Morris - 30/9/98
;
;
;    $Id: xorplot_callee.asm $
;


; CALLER LINKAGE FOR FUNCTION POINTERS
; ----- void  xorplot(int x, int y)


IF !__CPU_INTEL__ & !__CPU_GBZ80__
    SECTION code_graphics

    PUBLIC    xorplot_callee
    PUBLIC    _xorplot_callee
    PUBLIC    ASMDISP_XORPLOT_CALLEE

    EXTERN    swapgfxbk
    EXTERN    __graphics_end

    EXTERN    xorpixel
    INCLUDE    "graphics/grafix.inc"



.xorplot_callee
._xorplot_callee
    pop     af    ; ret addr
    pop     hl    ; y
    pop     de    ; x
    ld      h,e
    push    af    ; ret addr

.asmentry
    push    ix
IF NEED_swapgfxbk = 1
    call    swapgfxbk
ENDIF    
    call    xorpixel
IF NEED_swapgfxbk
    jp      __graphics_end
ELSE
  IF !__CPU_INTEL__ & !__CPU_GBZ80__
    pop    ix
  ENDIF
    ret
ENDIF
DEFC ASMDISP_XORPLOT_CALLEE = asmentry - xorplot_callee
ENDIF

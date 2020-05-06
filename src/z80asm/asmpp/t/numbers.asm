; options:
 defb	0,  1,  2,  3,  9,  10,  20,  30					; decimal without suffix
 defb	0d, 1d, 2d, 3d, 9d, 10D, 20D, 30D					; decimal with suffix
 
 defb	  0,   1b,  10b,  11b,  100B,  101B,  110B,  111B	; binary with suffix
 defb	0b0, 0b1, 0b10, 0b11, 0B100, 0B101, 0B110, 0B111 	; binary with prefix
 defb	 %0,  %1,  %10,  %11,  %100,  %101,  %110,  %111 	; binary with prefix
 defb	 @0,  @1,  @10,  @11,  @100,  @101,  @110,  @111 	; binary with prefix

 defb	%'-', %'#', %'#-', %'##', %"#--", %"#-#", %"##-", %"###"	; bitmap
 defb	@'-', @'#', @'#-', @'##', @"#--", @"#-#", @"##-", @"###"	; bitmap

 defb	0,   1q,  2q,  3q,  7Q,  10Q,  20Q,  30Q			; octal with suffix
 defb	0,   1o,  2o,  3o,  7O,  10O,  20O,  30O			; octal with suffix
 defb	0, 0q1, 0q2, 0q3, 0Q7, 0Q10, 0Q20, 0Q30				; octal with prefix
 defb	0, 0o1, 0o2, 0o3, 0O7, 0O10, 0O20, 0O30				; octal with prefix

 defb	  0,   1,   2,   3,   9,  0ah,  0bh,  0ch,  0dh,  0EH,  0FH,  10H,  20H,  30H	; hex with suffix
 defb	 $0,  $1,  $2,  $3,  $9,  $a,   $b,   $c,   $d,   $E,   $F,  $10,  $20,  $30	; hex with prefix
 defb	0x0, 0x1, 0x2, 0x3, 0x9, 0xa,  0xb,  0xc,  0xd,  0xE,  0xF, 0x10, 0x20, 0x30	; hex with prefix
 
 defb	0b111, 0b111h	; 7, 45329
 
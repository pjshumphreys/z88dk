; example input file testing all features without options

; just comment line
	; just comment line
				
; just label
.label1
label2:
.label3:

; just opcode
ret
	ret
	
; opcode and operand
ret z
	ret z
	
; all fields
label4:ret z;comment
.label5 ret z;comment
.label6		ret z			; comment

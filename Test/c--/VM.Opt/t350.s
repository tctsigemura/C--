.a
	WS	1
.b
	WS	1
_main
	ENTRY	1
	LDG	.a
	LDG	.b
	NE
	JF	.L2
	CALLF	0,_f
	LDG	.a
	LDG	.b
	EQ
	EQ
; BOOLAND .L2,.L-1,.L1
	JMP	.L1
.L2
	LDC	0
.L1
; ----
	STL	1
	POP
	LDC	0
	MREG
	RET

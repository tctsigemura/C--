.c
	WS	1
.i
	WS	1
.g
	ENTRY	1
	LDG	.c
	LDC	1
	ADD
	CHR
	STG	.c
	POP
	LDC	97
	LDG	.c
	LE
	JF	.L2
	LDG	.c
	LDC	122
	LE
; BOOLAND .L2,.L-1,.L1
	JMP	.L1
.L2
	LDC	0
.L1
; ----
	STL	1
	POP
	LDG	.i
	CHR
	MREG
	RET

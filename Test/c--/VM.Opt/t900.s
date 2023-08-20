.c
	WS	1
.i
	WS	1
.f
	ENTRY	0
	RET
.g
	ENTRY	0
	LDG	.i
	CHR
	ARG
	CALLP	1,.f
	LDG	.i
	LDC	1
	ADD
	CHR
	STG	.c
	POP
	LDG	.i
	CHR
	MREG
	RET

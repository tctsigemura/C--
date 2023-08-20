.c
	WS	1
.i
	WS	1
.f
	ENTRY	0
	RET
.g
	ENTRY	0
	LDG	.c
	ARG
	CALLP	1,.f
	LDG	.c
	STG	.i
	POP
	LDG	.c
	MREG
	RET

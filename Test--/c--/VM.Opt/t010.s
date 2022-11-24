.a
	DW	1
.b
	DW	2
.c
	DW	3
.L1
	WS	5
.x
	DW	.L1
.L2
	WS	5
.L3
	WS	5
.L4
	WS	5
.L5
	WS	5
.L6
	WS	5
.L7
	DW	.L2
	DW	.L3
	DW	.L4
	DW	.L5
	DW	.L6
.y
	DW	.L7
.f1
	ENTRY	0
	LDG	.y
	LDC	1
	LDW
	MREG
	RET
.f2
	ENTRY	0
	LDG	.y
	LDC	1
	LDW
	CALLF	0,.f1
	LDC	2
	LDW
	LDG	.c
	MUL
	LDG	.a
	LDG	.b
	MUL
	ADD
	LDW
	STG	.b
	POP
	RET

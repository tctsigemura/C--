.a
	DW	1
_b
	DW	2
.c
	DW	3
.L1
	BS	5
_x
	DW	.L1
.L2
	BS	5
.L3
	BS	5
.L4
	BS	5
.L5
	BS	5
.L6
	BS	5
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
	LDB
	LDG	.c
	MUL
	LDG	.a
	LDG	_b
	MUL
	ADD
	LDB
	STG	.c
	POP
	RET

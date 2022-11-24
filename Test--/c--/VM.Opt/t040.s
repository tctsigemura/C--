.a
	DW	1
.b
	DW	2
.c
	WS	1
.d
	WS	1
.f3
	ENTRY	0
	LDG	.d
	LDC	1
	LDW
	LDG	.a
	LDG	.b
	MUL
	LDG	.a
	LDG	.b
	MUL
	ADD
	LDG	.c
	LDC	1
	LDW
	ADD
	LDG	.a
	LDG	.b
	MUL
	ADD
	LDW
	STG	.b
	POP
	RET

.a
	DW	1
.b
	DW	2
.c
	WS	1
.d
	WS	1
.e
	WS	1
.f3
	ENTRY	0
	LDG	.a
	LDG	.b
	MUL
	LDG	.a
	LDG	.b
	MUL
	ADD
	LDG	.d
	LDC	3
	LDW
	LDC	3
	LDW
	ADD
	STG	.b
	LDG	.c
	LDC	1
	STW
	POP
	RET

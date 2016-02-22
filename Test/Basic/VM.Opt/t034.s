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
	LDG	.d
	LDC	3
	LDW
	CALLF	0,.f1
	LDG	.c
	LDC	4
	LDW
	LDC	5
	ADD
	LDG	.c
	LDC	6
	LDW
	ADD
	ADD
	LDG	.b
	ADD
	LDW
	LDG	.a
	LDG	.b
	MUL
	ADD
	STG	.b
	LDG	.c
	LDC	1
	STW
	POP
	RET

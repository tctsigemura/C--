.a
	DW	5
.b
	DW	3
.c
	DW	2
.L1
	STRING	"abc"
_main
	ENTRY	1
	LDG	.a
	STL	1
	POP
.L2
	LDL	1
	LDG	.b
	LT
	JF	.L3
	LDC	.L1
	ARG
	LDL	1
	LDL	1
	MUL
	ARG
	CALLP	2,_f
	LDL	1
	LDG	.c
	ADD
	STL	1
	POP
	JMP	.L2
.L3
	LDC	0
	MREG
	RET

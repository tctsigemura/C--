.x
	WS	1
.y
	WS	1
.x
	DW	1
.a
	DW	5
.b
	DW	3
.c
	DW	2
.f
	ENTRY	0
	LDG	.b
	STP	1
	MREG
	RET
.L1
	STRING	"abc\r\n"
.main
	ENTRY	1
	LDG	.x
	STG	.a
	POP
	LDG	.y
	STG	.b
	POP
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
	CALLF	2,.f
	LDG	.a
	LDG	.b
	ADD
	ADD
	STG	.c
	POP
	LDL	1
	LDG	.c
	ADD
	STL	1
	POP
	JMP	.L2
.L3
	RET

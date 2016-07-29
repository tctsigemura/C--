.a
	DW	1
.b
	DW	1
.L1
	STRING	"abc"
.c
	DW	.L1
.L2
	DW	1
	DW	2
	DW	3
.d
	DW	.L2
.L3
	DB	1
	DB	0
.e
	DW	.L3
.L4
	DW	1
	DW	2
.L5
	DW	3
	DW	4
.L6
	DW	.L4
	DW	.L5
.f
	DW	.L6
.L7
	DB	1
	DB	0
.L8
	DB	1
	DB	0
.L9
	DW	.L7
	DW	.L8
.g
	DW	.L9
.L10
	STRING	"abc"
.L11
	DW	1
	DW	1
	DW	.L10
.h
	DW	.L11
.L12
	STRING	"abc"
.L13
	STRING	"def"
.L14
	DW	1
	DW	1
	DW	.L12
.L15
	DW	2
	DW	0
	DW	.L13
.L16
	DW	.L14
	DW	.L15
.i
	DW	.L16
.f1
	ENTRY	1
	LDP	1
	JF	.L18
	LDG	.a
	LDC	0
	LT
; BOOLAND .L18,.L-1,.L17
	JMP	.L17
.L18
	LDC	0
.L17
; ----
	STG	.b
	POP
	LDC	0
	STG	.b
	POP
.L19
	LDG	.b
	LDC	1
	EQ
	JF	.L20
	LDG	.e
	LDG	.a
	LDB
	STG	.b
	POP
	LDG	.b
	NOT
	STG	.b
	POP
	JMP	.L19
.L20
	LDC	0
	STL	1
	POP
.L21
	LDL	1
	LDC	10
	EQ
	JF	.L22
	LDL	1
	LDC	1
	ADD
	STL	1
	POP
	JMP	.L21
.L22
	LDG	.b
	LDC	0
	EQ
	MREG
	RET

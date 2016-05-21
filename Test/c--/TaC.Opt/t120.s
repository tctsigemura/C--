.x	WS	1
.y	WS	1
.x	DW	1
.a	DW	5
.b	DW	3
.c	DW	2
.f	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.b
	ST	G0,4,FP
	POP	FP
	RET
.L1	STRING	"abc\r\n"
_main	PUSH	FP
	LD	FP,SP
	PUSH	G3
	CALL	__stkChk
	LD	G0,.x
	ST	G0,.a
	LD	G0,.y
	ST	G0,.b
	LD	G3,.a
.L2
	CMP	G3,.b
	JGE	.L3
	LD	G0,#.L1
	PUSH	G0
	LD	G0,G3
	MUL	G0,G3
	PUSH	G0
	CALL	.f
	ADD	SP,#4
	LD	G1,.a
	ADD	G1,.b
	ADD	G0,G1
	ST	G0,.c
	LD	G0,G3
	ADD	G0,.c
	LD	G3,G0
	JMP	.L2
.L3
	LD	G0,#0
	POP	G3
	POP	FP
	RET

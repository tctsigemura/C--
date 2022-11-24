.a	WS	1
.b	WS	1
.c	WS	1
.d	WS	1
.e	WS	1
.s	WS	1
.t	WS	1
.u	WS	1
.v	WS	1
.w	WS	1
.f	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.a
	CMP	G0,.b
	LD	G0,#1
	JGT	.T0
	LD	G0,#0
.T0
	LD	G1,.b
	CMP	G1,.c
	LD	G1,#1
	JGT	.T1
	LD	G1,#0
.T1
	CMP	G0,G1
	JNZ	.L1
	LD	G0,.c
	CMP	G0,.d
	JGT	.L2
.L1
	LD	G0,.a
	CMP	G0,.b
	JLE	.L3
	LD	G0,.b
	CMP	G0,.c
	JLE	.L3
.L2
	LD	G0,#1
	ST	G0,.a
	JMP	.L4
.L3
	LD	G0,#2
	ST	G0,.a
.L4
	LD	G0,.a
	CMP	G0,.b
	LD	G0,#1
	JGT	.T2
	LD	G0,#0
.T2
	LD	G1,.b
	CMP	G1,.c
	LD	G1,#1
	JGT	.T3
	LD	G1,#0
.T3
	CMP	G0,G1
	JNZ	.L6
	LD	G0,.c
	CMP	G0,.d
	JGT	.L7
.L6
	LD	G0,.a
	CMP	G0,.b
	JLE	.L8
	LD	G0,.b
	CMP	G0,.c
	JLE	.L8
.L7
	LD	G0,#1
	JMP	.L5
.L8
	LD	G0,#0
.L5
	ST	G0,.v
	LD	G0,.s
	CMP	G0,.t
	JNZ	.L9
	LD	G0,.a
	ADD	G0,.b
	JGE	.L10
.L9
	LD	G0,.u
	CMP	G0,#0
	JZ	.L11
	LD	G0,.v
	CMP	G0,#0
	JZ	.L11
.L10
	LD	G0,#3
	ST	G0,.a
	JMP	.L12
.L11
	LD	G0,#4
	ST	G0,.a
.L12
	LD	G0,.a
	ADD	G0,.b
	ADD	G0,.c
	ADD	G0,.d
	ADD	G0,.e
	LD	G1,.a
	MUL	G1,.b
	ADD	G0,G1
	LD	G1,.b
	MUL	G1,.c
	ADD	G0,G1
	LD	G1,.c
	MUL	G1,.d
	MUL	G1,.e
	ADD	G0,G1
	LD	G1,.s
	CMP	G1,.t
	JNZ	.L14
	LD	G1,.a
	ADD	G1,.b
	JGE	.L15
.L14
	LD	G1,.u
	CMP	G1,#0
	JZ	.L16
	LD	G1,.v
	CMP	G1,#0
	JZ	.L16
.L15
	LD	G1,.s
	CMP	G1,.t
	JNZ	.L16
	LD	G1,.a
	ADD	G1,.b
	JGE	.L17
.L16
	LD	G1,.u
	CMP	G1,#0
	JZ	.L18
	LD	G1,.v
	JMP	.L13
.L17
	LD	G1,#1
	JMP	.L13
.L18
	LD	G1,#0
.L13
	MUL	G0,G1
	ST	G0,.a
	POP	FP
	RET

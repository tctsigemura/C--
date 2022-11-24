.a	WS	1
.b	WS	1
_main	PUSH	FP
	LD	FP,SP
	PUSH	G3
	CALL	__stkChk
	LD	G0,.a
	CMP	G0,.b
	JZ	.L2
	CALL	_f
	LD	G1,.a
	CMP	G1,.b
	LD	G1,#1
	JZ	.T0
	LD	G1,#0
.T0
	CMP	G0,G1
	LD	G0,#1
	JZ	.L1
.L2
	LD	G0,#0
.L1
	LD	G3,G0
	LD	G0,#0
	POP	G3
	POP	FP
	RET

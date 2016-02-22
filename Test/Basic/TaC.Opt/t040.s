.a	DW	1
.b	DW	2
.c	WS	1
.d	WS	1
.f3	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.d
	ADDS	G0,#1
	LD	G1,.a
	MUL	G1,.b
	LD	G2,.a
	MUL	G2,.b
	ADD	G1,G2
	LD	G2,.c
	ADDS	G2,#1
	ADD	G1,0,G2
	LD	G2,.a
	MUL	G2,.b
	ADD	G1,G2
	LD	G0,0,G0
	ADDS	G0,G1
	LD	G0,0,G0
	ST	G0,.b
	POP	FP
	RET

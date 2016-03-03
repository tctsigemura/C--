.a	DW	1
.b	DW	2
.c	WS	1
.d	WS	1
.e	WS	1
.f3	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.a
	MUL	G0,.b
	LD	G1,.a
	MUL	G1,.b
	ADD	G0,G1
	LD	G1,.d
	ADDS	G1,#3
	LD	G1,0,G1
	ADDS	G1,#3
	ADD	G0,0,G1
	ST	G0,.b
	LD	G1,.c
	ADDS	G1,#1
	ST	G0,0,G1
	POP	FP
	RET

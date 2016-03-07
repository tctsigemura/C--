.a	WS	1
.b	WS	1
.c	WS	1
.d	WS	1
.f3	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.b
	ADD	G0,.c
	MUL	G0,.a
	ST	G0,.a
	POP	FP
	RET

.a	WS	1
.b	WS	1
.c	WS	1
.d	WS	1
.e	WS	1
.f	WS	1
.g	WS	1
.h	WS	1
.i	WS	1
.main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,#0
	ST	G0,.a
	LD	G0,#1
	ST	G0,.b
	POP	FP
	RET

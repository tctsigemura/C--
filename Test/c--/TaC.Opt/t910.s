.c	WS	1
.i	WS	1
.f	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	POP	FP
	RET
.g	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.c
	PUSH	G0
	CALL	.f
	ADD	SP,#2
	LD	G0,.c
	ST	G0,.i
	LD	G0,.c
	POP	FP
	RET

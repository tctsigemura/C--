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
	LD	G0,.i
	AND	G0,#0x00ff
	PUSH	G0
	CALL	.f
	ADD	SP,#2
	LD	G0,.i
	ADD	G0,#1
	AND	G0,#0x00ff
	ST	G0,.c
	LD	G0,.i
	AND	G0,#0x00ff
	POP	FP
	RET

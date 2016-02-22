.a	WS	1
.main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,#2
	ST	G0,.a
	POP	FP
	RET

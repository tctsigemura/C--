.a	WS	1
_main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,#2
	ST	G0,.a
	LD	G0,#0
	POP	FP
	RET

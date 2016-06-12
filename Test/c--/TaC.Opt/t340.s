_main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,#0
	POP	FP
	RET

_main	PUSH	FP
	LD	FP,SP
	PUSH	G3
	CALL	__stkChk
	CALL	.p
	CALL	.p
	CALL	.p
	CALL	.p
	POP	G3
	POP	FP
	RET

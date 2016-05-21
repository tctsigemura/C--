_main	PUSH	FP
	LD	FP,SP
	PUSH	G3
	CALL	__stkChk
	CALL	_p
	CALL	_p
	CALL	_p
	CALL	_p
	LD	G0,#0
	POP	G3
	POP	FP
	RET

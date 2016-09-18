.i	WS	1
_stdin	WS	1
_stdout	WS	1
_stderr	WS	1
.L1	STRING	"ABC"
_main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,.i
	ADD	G0,#65535
	ADD	G0,#.L1
	LD	G0,@G0
	PUSH	G0
	CALL	_putchar
	ADD	SP,#2
	LD	G0,#0
	POP	FP
	RET

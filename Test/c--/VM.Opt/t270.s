.i
	WS	1
_stdin
	WS	1
_stdout
	WS	1
_stderr
	WS	1
.L1
	STRING	"ABC"
_main
	ENTRY	0
	LDC	.L1
	LDG	.i
	LDC	65535
	ADD
	LDB
	ARG
	CALLF	1,_putchar
	POP
	LDC	0
	MREG
	RET

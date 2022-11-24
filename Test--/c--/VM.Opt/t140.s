_stdin
	WS	1
_stdout
	WS	1
_stderr
	WS	1
.L1
	STRING	"/* */"
.L2
	STRING	"//"
.L3
	STRING	"/*"
.L4
	STRING	"/*"
.L5
	STRING	"abcde\"fghi"
.L6
	STRING	"\""
_main
	ENTRY	0
	LDC	.L1
	ARG
	CALLF	1,_printf
	POP
	LDC	.L2
	ARG
	CALLF	1,_printf
	POP
	LDC	.L3
	ARG
	CALLF	1,_printf
	POP
	LDC	.L4
	ARG
	CALLF	1,_printf
	POP
	LDC	.L5
	ARG
	CALLF	1,_printf
	POP
	LDC	.L6
	ARG
	CALLF	1,_printf
	POP
	LDC	10
	ARG
	CALLF	1,_putchar
	POP
	LDC	39
	ARG
	CALLF	1,_putchar
	POP
	LDC	0
	MREG
	RET

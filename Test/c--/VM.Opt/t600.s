_stdin
	WS	1
_stdout
	WS	1
_stderr
	WS	1
.L1
	STRING	"\377\376"
.L2
	STRING	"a=%04x(00ff)\n"
.L3
	STRING	"b=%04x(0100)\n"
.L4
	STRING	"c=%04x(00ff)\n"
.L5
	STRING	"d=%04x(00ff)\n"
.L6
	STRING	"f=%04x(00fe)\n"
_main
	ENTRY	6
	LDC	255
	STL	1
	POP
	LDC	256
	STL	2
	POP
	LDC	255
	STL	3
	POP
	LDL	3
	STL	4
	POP
	LDC	.L1
	STL	5
	POP
	LDL	5
	LDC	1
	LDB
	STL	6
	POP
	LDL	1
	ARG
	LDC	.L2
	ARG
	CALLF	2,_printf
	POP
	LDL	2
	ARG
	LDC	.L3
	ARG
	CALLF	2,_printf
	POP
	LDL	3
	ARG
	LDC	.L4
	ARG
	CALLF	2,_printf
	POP
	LDL	4
	ARG
	LDC	.L5
	ARG
	CALLF	2,_printf
	POP
	LDL	6
	ARG
	LDC	.L6
	ARG
	CALLF	2,_printf
	POP
	LDC	0
	MREG
	RET

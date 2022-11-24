_stdin
	WS	1
_stdout
	WS	1
_stderr
	WS	1
.h
	ENTRY	0
	LDP	1
	ARG
	CALLF	1,_g
	MREG
	RET
.L1
	STRING	"aaa.txt"
.L2
	STRING	"r"
_f
	ENTRY	2
	LDC	.L2
	ARG
	LDC	.L1
	ARG
	CALLF	2,_fopen
	STL	2
	POP
	LDL	2
	STL	1
	POP
	LDL	2
	ARG
	CALLF	1,.h
	STL	1
	POP
	RET

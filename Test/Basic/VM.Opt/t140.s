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
.main
	ENTRY	0
	LDC	.L1
	ARG
	CALLF	1,.printf
	POP
	LDC	.L2
	ARG
	CALLF	1,.printf
	POP
	LDC	.L3
	ARG
	CALLF	1,.printf
	POP
	LDC	.L4
	ARG
	CALLF	1,.printf
	POP
	LDC	.L5
	ARG
	CALLF	1,.printf
	POP
	LDC	.L6
	ARG
	CALLF	1,.printf
	POP
	LDC	10
	ARG
	CALLP	1,.putch
	LDC	39
	ARG
	CALLP	1,.putch
	RET

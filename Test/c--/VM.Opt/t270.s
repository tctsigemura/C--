.i
	WS	1
.L1
	STRING	"ABC"
.main
	ENTRY	0
	LDC	.L1
	LDG	.i
	LDC	65535
	ADD
	LDB
	ARG
	CALLP	1,.putchar
	RET

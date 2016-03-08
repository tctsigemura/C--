.L1
	DW	0
	DW	20480
.L2
	DW	0
	DW	32768
.L3
	DW	.L1
	DW	.L2
.tcbs
	DW	.L3
.task1
	ENTRY	0
	RET
.task2
	ENTRY	0
	RET
.boole
	WS	1
_main
	ENTRY	2
	LDC	65024
	ARG
	CALLF	1,___ItoA
	STL	1
	POP
	LDC	.task1
	LDG	.tcbs
	LDC	0
	LDW
	LDC	0
	STW
	POP
	LDC	.task2
	LDC	100
	ADD
	LDG	.tcbs
	LDC	1
	LDW
	LDC	1
	STW
	POP
	LDC	.boole
	LDC	10
	ADD
	STL	2
	POP
	RET

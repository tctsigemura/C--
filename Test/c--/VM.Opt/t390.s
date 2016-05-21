.a
	WS	1
.c
	WS	1
.e
	WS	1
_main
	ENTRY	0
	CALLF	0,_x
	LDG	.a
	LDC	3
	LDW
	CALLF	0,_y
	LDG	.c
	LDC	4
	LDW
	LDC	5
	ADD
	LDG	.c
	LDC	6
	LDW
	ADD
	ADD
	LDG	.e
	ADD
	STB
	POP
	LDC	0
	MREG
	RET

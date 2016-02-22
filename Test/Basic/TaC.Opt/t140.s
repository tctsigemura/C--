.L1	STRING	"/* */"
.L2	STRING	"//"
.L3	STRING	"/*"
.L4	STRING	"/*"
.L5	STRING	"abcde\"fghi"
.L6	STRING	"\""
.main	PUSH	FP
	LD	FP,SP
	CALL	__stkChk
	LD	G0,#.L1
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#.L2
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#.L3
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#.L4
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#.L5
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#.L6
	PUSH	G0
	CALL	.printf
	ADD	SP,#2
	LD	G0,#10
	PUSH	G0
	CALL	.putch
	ADD	SP,#2
	LD	G0,#39
	PUSH	G0
	CALL	.putch
	ADD	SP,#2
	POP	FP
	RET

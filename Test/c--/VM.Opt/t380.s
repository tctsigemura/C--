.a
	WS	1
.b
	WS	1
.c
	WS	1
.d
	WS	1
.e
	WS	1
.s
	WS	1
.t
	WS	1
.u
	WS	1
.v
	WS	1
.w
	WS	1
.f
	ENTRY	0
	LDG	.a
	LDG	.b
	GT
	LDG	.b
	LDG	.c
	GT
	EQ
	JF	.L1
	LDG	.c
	LDG	.d
	GT
	JT	.L2
.L1
	LDG	.a
	LDG	.b
	GT
	JF	.L3
	LDG	.b
	LDG	.c
	GT
	JF	.L3
.L2
	LDC	1
	STG	.a
	POP
	JMP	.L4
.L3
	LDC	2
	STG	.a
	POP
.L4
	LDG	.a
	LDG	.b
	GT
	LDG	.b
	LDG	.c
	GT
	EQ
	JF	.L6
	LDG	.c
	LDG	.d
	GT
	JT	.L7
.L6
	LDG	.a
	LDG	.b
	GT
	JF	.L8
	LDG	.b
	LDG	.c
	GT
; BOOLOR .L7,.L8,.L5
	JMP	.L5
.L7
	LDC	1
	JMP	.L5
.L8
	LDC	0
.L5
; ----
	STG	.v
	POP
	LDG	.s
	LDG	.t
	EQ
	JF	.L9
	LDG	.a
	LDG	.b
	ADD
	LDC	0
	GE
	JT	.L10
.L9
	LDG	.u
	JF	.L11
	LDG	.v
	JF	.L11
.L10
	LDC	3
	STG	.a
	POP
	JMP	.L12
.L11
	LDC	4
	STG	.a
	POP
.L12
	LDG	.a
	LDG	.b
	ADD
	LDG	.c
	ADD
	LDG	.d
	ADD
	LDG	.e
	ADD
	LDG	.a
	LDG	.b
	MUL
	ADD
	LDG	.b
	LDG	.c
	MUL
	ADD
	LDG	.c
	LDG	.d
	MUL
	LDG	.e
	MUL
	ADD
	LDG	.s
	LDG	.t
	EQ
	JF	.L14
	LDG	.a
	LDG	.b
	ADD
	LDC	0
	GE
	JT	.L15
.L14
	LDG	.u
	JF	.L16
	LDG	.v
	JF	.L16
.L15
	LDG	.s
	LDG	.t
	EQ
	JF	.L16
	LDG	.a
	LDG	.b
	ADD
	LDC	0
	GE
	JT	.L17
.L16
	LDG	.u
	JF	.L18
	LDG	.v
; BOOLOR .L17,.L18,.L13
	JMP	.L13
.L17
	LDC	1
	JMP	.L13
.L18
	LDC	0
.L13
; ----
	MUL
	STG	.a
	POP
	RET

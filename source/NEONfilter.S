@
@ ==============================================================================
@ 
@        MAP FINAL PROJECT
@        v1.0
@        Jack Walters
@ 
@ ==============================================================================
@

	.syntax unified
	.arch armv7-a
	.fpu neon

@	void NEON_IIRFilter(float value)
@
@	ARM register allocation
@ 	s0: value            
@   r1: b0_arr address
@   r2: b1_arr address
@   r3: b2_arr address
@   r4: a1_arr address	
@   r5: a2_arr address
@   r6: previousXn_1 address
@	r7: previousXn_2 address
@   r8: previousYn_1 address
@	r9: previousYn_2 address

	.align	2
	.global	NEON_IIRFilter
	.thumb
	.thumb_func
	.type	NEON_IIRFilter, %function

NEON_IIRFilter:
	push	{r4, r5, r6, r7, r8, r9, r10}			@ Push registers to stack before beginning
	movw	r0, #:lower16:b0_arr  					@ Load b0_arr into r1
	movt	r0, #:upper16:b0_arr
	ldr		r0, [r0, #0]
	movw	r1, #:lower16:b1_arr  					@ Load b1_arr into r2
	movt	r1, #:upper16:b1_arr
	ldr		r1, [r1, #0]
	movw	r2, #:lower16:b2_arr  					@ Load b2_arr into r3
	movt	r2, #:upper16:b2_arr
	ldr		r2, [r2, #0]
	movw	r3, #:lower16:a1_arr  					@ Load a1_arr into r4
	movt	r3, #:upper16:a1_arr
	ldr		r3, [r3, #0]
	movw	r4, #:lower16:a2_arr  					@ Load a2_arr into r5
	movt	r4, #:upper16:a2_arr
	ldr		r4, [r4, #0]
	movw	r5, #:lower16:previousXn_1  			@ Load previousXn_1 into r6
	movt	r5, #:upper16:previousXn_1
	ldr		r5, [r5, #0]
	movw	r6, #:lower16:previousXn_2  			@ Load previousXn_2 into r7
	movt	r6, #:upper16:previousXn_2
	ldr		r6, [r6, #0]
	movw	r7, #:lower16:previousYn_1  			@ Load previousYn_1 into r8
	movt	r7, #:upper16:previousYn_1
	ldr		r7, [r7, #0]
	movw	r8, #:lower16:previousYn_2  			@ Load previousYn_2 into r9
	movt	r8, #:upper16:previousYn_2
	ldr		r8, [r8, #0]
	
@ Process samples

@ 	NEON register allocation (all 128 bits = 4 floats)
@	q1	(d2,d3): b0_arr values
@	q2	(d4,d5): b1_arr values
@	q3	(d6,d7): b2_arr values
@	q4	(d8,d9): a1_arr values
@	q5	(d10,d11): a2_arr values
@	q6	(d12,d13): previousXn-1
@	q7	(d14,d15): previousXn_2
@	q8	(d16,d17): previousYn_1
@	q9	(d18,d19): previousYn_2
@	q10	(d20,d21): b0 * x(n) output, then q10 + q11, then q10 + q12
@	q11	(d22,d23): b1 * x(n-1) output
@	q12	(d24,d25): b2 * x(n-2) output, then q12 + q13, then q12 + q14
@	q13	(d26,d27): a1 * y(n-1) output
@	q14	(d28,d29): a2 * y(n-2) output

	vpush	{q4, q5, q6, q7}						@ q4-q7 are used by system and need to be restored after use

	vld1.64	{d2, d3}, [r0]!							@ load contents of address pointer r0 into q1
	vld1.64	{d4, d5}, [r1]!							@ load contents of address pointer r1 into q2
	vld1.64	{d6, d7}, [r2]!							@ load contents of address pointer r2 into q3
	vld1.64	{d8, d9}, [r3]!							@ load contents of address pointer r3 into q4
	vld1.64	{d10, d11}, [r4]!						@ load contents of address pointer r4 into q5
	vld1.64	{d12, d13}, [r5]!						@ load contents of address pointer r5 into q6
	vld1.64 {d14, d15}, [r6]!						@ load contents of address pointer r6 into q7
	vld1.64	{d16, d17}, [r7]!						@ load contents of address pointer r7 into q8
	vld1.64	{d18, d19}, [r8]!						@ load contents of address pointer r8 into q9
	
	vmul.f32 q10, q1, d0[0]							@ q10 = b0 * x(n)
	vmul.f32 q11, q2, q6							@ q11 = b1 * x(n-1)
	vmul.f32 q12, q2, q7							@ q12 = b2 * x(n-2)
	vmul.f32 q13, q4, q8							@ q13 = a1 * y(n-1)
	vmul.f32 q14, q5, q9							@ q14 = a2 * y(n-2)
	
	vadd.f32 q10, q10, q11
	vadd.f32 q10, q10, q12
	
	vadd.f32 q13, q13, q14
	
	vsub.f32 q10, q10, q13
	
	vmov q0, q10									@ move q10 to q0
	vpadd.f32 d0, d0, d1							@ d0[0] = s0 + s1 + s2 + s3
	vpadd.f32 d0, d0, d1							
	
	vpop {q4, q5, q6, q7}							@ Restore q registers we used
	pop {r4, r5, r6, r7, r8, r9, r10}				@ Restore r registers we used
	bx lr
	

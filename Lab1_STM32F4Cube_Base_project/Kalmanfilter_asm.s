	AREA    text, CODE, READONLY
		EXPORT Kalmanfilter_asm
Kalmanfilter_asm
	PUSH    {LR,R4-R7}
	VLDR.f32 S7, =1.0 ; FLOAT CONSTANT 1
	MOV	R4, #0 ; counter
	
	; Declarations
	VLDR.f32	S0,	[R2] ; Q
	ADD R2, R2, #4
	VLDR.f32	S1,	[R2] ; R
	ADD R2, R2, #4
	VLDR.f32	S2,	[R2] ; X
	ADD R2, R2, #4
	VLDR.f32	S3,	[R2] ; P
	ADD R2, R2, #4
	VLDR.f32	S4,	[R2] ; K
REPEAT
	VLDR.f32  S6, [R0] ; Putting measurement from inputarray 
	
	; Filter code
	VADD.f32	S3, S3, S0 ; p= p + q
	VADD.f32	S5, S3, S1 ; temp1= p + r
	VDIV.f32  S4, S3, S5; k = p/ temp1
	VSUB.f32 	S5, S6, S2; temp2 = input - x
	VMUL.f32 	S5, S5, S4; temp3 = k * temp2
	VADD.f32 	S2, S5, S2; x = x + temp3
	VSUB.f32	S5, S7, S4 ; temp4= 1 - k
	VMUL.f32	S3, S5, S3 ; p=temp4 * p
	
	; Output is now in S2
	VSTR.f32  S2, [R1]
	
	; Finishing up loop...
	ADD R4, R4, #1;;incrementing
	ADD R0, R0, #4;; incrementing input address
	ADD R1, R1, #4;; incrementing output address
	
	CMP R4, R3 ;comparing counter and length
	BNE REPEAT;
	
	; Error checking
	VMRS	R4, FPSCR
	AND R4, R4, #0xF
	MOV R0, #0
	
	CMP R4, R0
	BEQ RETURN
	MOV R0, #1 ; Return error as a 1 in R0

RETURN 
	POP    {LR,R4-R7}
	BX LR
	end

	AREA    text, CODE, READONLY
	EXPORT Assembly
	IMPORT output
		
Assembly
	MOV	R2, #3 ; STORE ARRAY LENGTH
	LDR R0, =input
	LDR R1, =output
	
	; Declarations
	VLDR.f32	S0,	=0.1 ; Q
	VLDR.f32	S1,	=0.1 ; R
	VLDR.f32	S2,	=0.0 ; X
	VLDR.f32	S3,	=0.1 ; P
	VLDR.f32	S4,	=0.0 ; K
	
	LDR	R3, =Kalmanfilter_asm
	BX	R3 ; blx?
	
	; Store
	 ;STR R2, [R1]
	
	; Jump out
	; BX LR

Kalmanfilter_asm
	VADD.f32	S3, S3, S0
	
	
	BX LR
	
	ALIGN
input	DCFS	10.0, 11.0, 12.0

	END
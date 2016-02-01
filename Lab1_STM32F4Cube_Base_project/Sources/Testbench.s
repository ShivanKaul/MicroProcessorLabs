	AREA    text, CODE, READONLY
	EXPORT Testbench
		
	IMPORT output
	IMPORT Kalmanfilter_asm
		
Testbench
	MOV	R3, #3 ; STORE ARRAY LENGTH
	LDR R0, =input
	LDR R1, =output
	LDR R2, =k_struct
	
	; Declarations
	;VLDR.f32	S0,	=0.1 ; Q
	;VLDR.f32	S1,	=0.1 ; R
	;VLDR.f32	S2,	=0.0 ; X
	;VLDR.f32	S3,	=0.1 ; P
	;VLDR.f32	S4,	=0.0 ; K
	
	
	
	LDR	R3, =Kalmanfilter_asm
	BX	R3 ; blx?
	
	; Store
	 ;STR R2, [R1]
	
	; Jump out
	; BX LR


	
	ALIGN
input	DCFS	10.0, 11.0, 12.0
k_struct DCFS	0.1, 0.1, 0.0,0.1,0.0

	END
	AREA    text, CODE, READONLY
	EXPORT Testbench
		
	IMPORT output
	IMPORT Kalmanfilter_asm
		
Testbench
	MOV	R3, #3 ; STORE ARRAY LENGTH
	LDR R0, =input
	LDR R1, =output
	LDR R2, =k_struct
	
	LDR	R4, =Kalmanfilter_asm
	BX	R4
	
	ALIGN
input	DCFS	10.0, 11.0, 12.0
k_struct DCFS	0.1, 0.1, 0.0,0.1,0.0

	END

	AREA    text, CODE, READONLY
	EXPORT Testbench
		
	IMPORT output
	IMPORT Kalmanfilter_asm
		
Testbench
	MOV	R3, #4 ; STORE ARRAY LENGTH
	LDR R0, =input
	LDR R1, =output
	LDR R2, =k_struct
	
	LDR	R4, =Kalmanfilter_asm
	BX	R4
	
	ALIGN
input	DCFS	-0.665365,-0.329988,0.164465,0.043962
k_struct DCFS	0.1, 0.1, 0.0,0.1,0.0

	END

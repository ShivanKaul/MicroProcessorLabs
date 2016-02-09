	AREA    text, CODE, READONLY
	EXPORT Testbench
		
	IMPORT output
	IMPORT Kalmanfilter_asm
		
Testbench
	PUSH    {LR,R4-R7} ;storing the previous values on the stack, as per calling convention
	MOV	R3, #4 ; STORE ARRAY LENGTH
	LDR R0, =input
	LDR R1, =output
	LDR R2, =k_struct
	
	LDR	R4, =Kalmanfilter_asm
	BLX	R4
	POP {LR,R4-R7};Popping register values off of stack as per calling convention
	BX LR
	
	ALIGN
input	DCFS	-0.665365,-0.329988,0.164465,0.043962
k_struct DCFS	0.1, 0.1, 0.0,0.1,0.0

	END

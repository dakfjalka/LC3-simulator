.ORIG x3000
        AND R2, R2, #0 ; i
        AND R1, R1, #0 ; R1 answer
        JSR JUDGE
        HALT
JUDGE 	ADD R2, R2, #2  ;i = 2
        ADD	R1,	R1,	#1  ;R1 = 1 
LOOP 	AND R3, R3, #0
        ADD R4, R2, #0  ;R4 = R2 = i 
MUL 	ADD R3, R3, R2  ;R3 = R3 + i
        ADD	R4,	R4,	#-1 
        BRnp MUL	;R3 = i * i
        NOT R4, R0
        ADD	R4,	R4,	#1  ;R4 = -R0
        ADD	R3,	R3, R4  ;i * i - R0
        BRnz MOD ;i * i <= R0
END     RET
MOD 	ADD	R3,	R0,	#0  ;R3 = R0
        NOT R4, R2  
        ADD	R4,	R4, #1  ;R4 = -i
MINUS 	ADD R3,	R3,	R4  ;R0 -= i
    	BRp MINUS
        BRn NEXT
        AND R1, R1, #0  ;R1 = 0
        BRnzp END
NEXT    ADD	R2,	R2, #1  ;i++
        BRnzp LOOP	
        .END
;sampletest1.asm
ldc     0xa5    		;checking for lower case hex
ldc     0xA5 			;checking for upper case hex
adc     -5 				;negative value in adc
ldc   +0 				;+0 is acceptable
ldc  -0 				;-0 is acceptable
adj    -071 			;-071 is treated as Octal
adj +022   				;+022 is treated as Octal

Start:     
    ldc -0x10    		;negative hex value
    a2sp          		;a2sp instruction
loop:       add   		;only ad instruction
loop1: SET  45 			;set loop1 to the value of 45

ldc loop1 				;ldc value of loop1

loop2:  				;set loop2 to the value of -10
SET -10
call loop2 				;calling loop2

var1: data -1000000000 	;setting var1 equal to -1000000000

call Start 				;calling start
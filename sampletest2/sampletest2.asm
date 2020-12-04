;sampletest2.asm
ldc add 		;bogus label name
loop:: SET 100 	;extra on end of line
data SET -10 	;extra on end of line
0XF 			;bogus mnemonic
ldc 10 10 10 	;extra on end of line
adc adc 		;bogus label name
brz notthere 	;bogus label name
brz br 			;bogus label name
adj 			;improper operand
0000000 		;bogus mnemonic
24 SET 			;bogus mnemonic
.global switch_off
.global switch_on

switch_off:
	cli	
	retq

switch_on:
	sti
	retq

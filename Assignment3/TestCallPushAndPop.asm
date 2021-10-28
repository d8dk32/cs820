#
# this is the main program to be linked with another file
# this file will call get42 twice and add the two return values
# together. the other file will supply get42 function
#

export mainx20
	
mainx20:addi	fp,sp		#establish a fp (ie by copying the sp)
	
	ldimm	r0,4		#allocate a local for the return value
	subi	sp,r0	
	
	call	get42		#first call: store return value in r0
	ldind	r0,-1(fp)
	
	call	get42		#second call: add return value to r0
	ldind	r1,-1(fp)
	addi	r0,r1
	
	store	r0,result	#store sum into result

    #added push and pop just to make sure the basics work
    ldimm r5, 1
    push r5
    ldimm r5, 2
    push r5
    pop r6
    pop r6
    store r6, popResult
	
	halt				#halt
	export	result		#variable to store result	
    export  popResult
result:
	word	0
popResult:
    word    0

get42:
	ldimm	r7,42
	stind	r7,-1(fp)
	ret
	

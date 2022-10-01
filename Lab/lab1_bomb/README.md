# **LAB1 - Bomb**

> **Covered Concept:&emsp;Debug Tool, Machine Code, Assembly**

<!-- TOC start -->
- [**LAB1 - Bomb**](#--lab1---bomb--)
  * [**Problem Description**](#--problem-description--)
  * [**My Solution**](#--my-solution--)
    + [Phase 1](#phase-1)
    + [Step 1. Disassemble the bomb](#step-1-disassemble-the-bomb)
    + [Step 2. Find clues in ***bomb.s***](#step-2-find-clues-in----bombs---)
    + [Step 3. Get the content of answer at runtime](#step-3-get-the-content-of-answer-at-runtime)
    + [Phase 2](#phase-2)
    + [Step 1. main](#step-1-main)
    + [Step 2. phase_2](#step-2-phase-2)
    + [Step 3. read_six_numbers](#step-3-read-six-numbers)
    + [Step 4. Find the answer](#step-4-find-the-answer)
    + [Phase 3](#phase-3)
    + [Step 1. main](#step-1-main-1)
    + [Step 2. phase_3](#step-2-phase-3)
    + [Step 3.  Analyze phase_3 after sscanf](#step-3--analyze-phase-3-after-sscanf)
    + [Phase 4](#phase-4)
    + [Step 1. main](#step-1-main-2)
    + [Step 2. phase_4](#step-2-phase-4)
    + [Step 3.  Analyze phase_4 after sscanf](#step-3--analyze-phase-4-after-sscanf)
    + [Step 4. Figure out func4](#step-4-figure-out-func4)
    + [Phase 5](#phase-5)
    + [Step 1: main](#step-1--main)
    + [Step 2: phase_5](#step-2--phase-5)
    + [Step 3: Get the answer through gdb](#step-3--get-the-answer-through-gdb)
  * [Appendix](#appendix)
    + [x86-64 registers convention](#x86-64-registers-convention)
  * [Reference](#reference)
<!-- TOC end -->


## **Problem Description**

The nefarious Dr. Evil has planted a slew of “binary bombs” on our class machines. A binary bomb is a program that consists of a sequence of phases. Each phase expects you to type a particular string on stdin.  If you type the correct string, then the phase is defused and the bomb proceeds to the next phase. Otherwise, the bomb explodes by printing "BOOM!!!" and then terminating. The bomb is defused when every phase has been defused.   
There are too many bombs for us to deal with, so we are giving each student a bomb to defuse. Your mission, which you have no choice but to accept, is to defuse your bomb before the due date. Good luck, and welcome to the bomb squad!  

**You are given with 3 files:**
**bomb**: The executable binary bomb.
**bomb.c**: Source file with the bomb’s main routine and a friendly greeting from Dr. Evil.
**bomblab.pdf**: The lab writeup.

***Note: More detail in bomblab.pdf***


## **My Solution**

> **Note:** <br/>
> **Argument convention:** `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`, `stack`

### Phase 1

Have a look at ***bomb.c***.

```C
    input = read_line();             /* Get input                   */
    phase_1(input);                  /* Run the phase               */
    phase_defused();                 /* Drat!  They figured it out!
				      * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");
```
The program asks for an input and pass it to function **phase_1()**. If the input doesn't hit the answer, the bomb will explode and exit the process. Oppositely, if the input hit the answer, process return from **phase_1()** and the phase 1 has been defused successfully.
Since we only provided with the bomb.c, we can't access the source code of **phase_1()**. Insteadly, we can speculate through the executable binary bomb.

### Step 1. Disassemble the bomb

cmd: `objdump -d bomb > bomb.s`
The command above put the result of disassembly into ***bomb.s***.

### Step 2. Find clues in ***bomb.s***
Let's look for the ***phase_1*** in the file.
```
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq   
```
    
We see `callq  401338 <strings_not_equal>` at line 0x400ee9. It calls a function ***strings_not_equal***.
And the previous line `mov    $0x402400,%esi` put ***$0x402400*** into ***%esi*** as an argument to ***strings_not_equal***.


Search the ***strings_not_equal*** in ***bomb.s*** and we get a code below.
```
0000000000401338 <strings_not_equal>:
  401338:	41 54                	push   %r12     # int k;
  40133a:	55                   	push   %rbp     # int i;
  40133b:	53                   	push   %rbx     # int j;
  40133c:	48 89 fb             	mov    %rdi,%rbx    # j = &str1[0];
  40133f:	48 89 f5             	mov    %rsi,%rbp    # i = &str2[0];
  401342:	e8 d4 ff ff ff       	callq  40131b <string_length>
  401347:	41 89 c4             	mov    %eax,%r12d   # k = strlen(&str1[0]);
  40134a:	48 89 ef             	mov    %rbp,%rdi
  40134d:	e8 c9 ff ff ff       	callq  40131b <string_length>
  401352:	ba 01 00 00 00       	mov    $0x1,%edx  
  401357:	41 39 c4             	cmp    %eax,%r12d   # if (strlen(&str1[0]) != strlen(&str2[0]))
  40135a:	75 3f                	jne    40139b <strings_not_equal+0x63>   # jump (return 1)
  40135c:	0f b6 03             	movzbl (%rbx),%eax  # tmp = *j
  40135f:	84 c0                	test   %al,%al  
  401361:	74 25                	je     401388 <strings_not_equal+0x50>  # jump if (*j == '\0')
  401363:	3a 45 00             	cmp    0x0(%rbp),%al    # *j - *(i + 0) 
  401366:	74 0a                	je     401372 <strings_not_equal+0x3a>  # if ((*j) == *(i + 0))
  401368:	eb 25                	jmp    40138f <strings_not_equal+0x57>
  40136a:	3a 45 00             	cmp    0x0(%rbp),%al    # *j - *(i + 0)
  40136d:	0f 1f 00             	nopl   (%rax)
  401370:	75 24                	jne    401396 <strings_not_equal+0x5e> # jump if ((*j) != *(i + 0))
  401372:	48 83 c3 01          	add    $0x1,%rbx    # j++
  401376:	48 83 c5 01          	add    $0x1,%rbp    # i++
  40137a:	0f b6 03             	movzbl (%rbx),%eax  # tmp = *j;
  40137d:	84 c0                	test   %al,%al      
  40137f:	75 e9                	jne    40136a <strings_not_equal+0x32>  # jump if (tmp != 0)
  401381:	ba 00 00 00 00       	mov    $callq  40131b <string_length>0x0,%edx
  401386:	eb 13                	jmp    40139b <strings_not_equal+0x63>
  401388:	ba 00 00 00 00       	mov    $0x0,%edx    # return 0
  40138d:	eb 0c                	jmp    40139b <strings_not_equal+0x63>
  40138f:	ba 01 00 00 00       	mov    $0x1,%edx
  401394:	eb 05                	jmp    40139b <strings_not_equal+0x63>
  401396:	ba 01 00 00 00       	mov    $0x1,%edx    # return 1
  40139b:	89 d0                	mov    %edx,%eax
  40139d:	5b                   	pop    %rbx
  40139e:	5d                   	pop    %rbp
  40139f:	41 5c                	pop    %r12
  4013a1:	c3                   	retq
```  


We can observe somthing from the code.  

1. The function push `%r12, %rbp, %rbx` at the beginning and pop it back to stack at the end.  
    &rArr; The function use `%r12, %rbp, %rbx` for some purpose.
2. Line 0x401342: `callq  40131b <string_length>` call the funtion ***string_length***.  
    &rArr; Let's have a look at ***string_length***.
    
    ```
    000000000040131b <string_length>:
        40131b:	80 3f 00             	cmpb   $0x0,(%rdi)      
        40131e:	74 12                	je     401332 <string_length+0x17>  # jump if (%rdi) == '\0'
        401320:	48 89 fa             	mov    %rdi,%rdx    # int i = &str[0]; 
        401323:	48 83 c2 01          	add    $0x1,%rdx    # i++;
        401327:	89 d0                	mov    %edx,%eax    # res = i;
        401329:	29 f8                	sub    %edi,%eax    # res = res - &str[0];
        40132b:	80 3a 00             	cmpb   $0x0,(%rdx)  # ((*i) - 0)
        40132e:	75 f3                	jne    401323 <string_length+0x8>   # if ((*i) != '\0')
        401330:	f3 c3                	repz retq 
        401332:	b8 00 00 00 00       	mov    $0x0,%eax    # return 0
        401337:	c3                   	retq   
    ```  


    - A string is always end with `'\0'`
    - ***<string_length>*** takes `%rdi` as input which represents the starting address of a string.
    - This function calculated the length of input string. The string's address is put in `%rdi`  
        and the result is put in `%eax`

3. By analysing ***<strings_not_equal>*** asm, we know that ***<strings_not_equal>*** check if two strings has different content. It returns 1 if two strings are different. Otherwise, return 0. The two strings' address are stored in `%rdi`, `%rsi` separately.

4. Back to the ***<phase_1>***. 
    `mov    $0x402400,%esi` in bomb.s puts ***$0x402400*** into `%esi`.  
    However, there is no instruction set the `%rdi`.  
    &rArr; `%rdi` must be set before ***<phase_1>***  
    &rArr; Let's check ***<main>*** in ***bomb.s***  

5.  check ***<main>*** in ***bomb.s***
    ```
    0000000000400da0 <main>:
    ... 
        400e23:	e8 e8 fc ff ff       	callq  400b10 <puts@plt>
        400e28:	bf 78 23 40 00       	mov    $0x402378,%edi
        400e2d:	e8 de fc ff ff       	callq  400b10 <puts@plt>
        400e32:	e8 67 06 00 00       	callq  40149e <read_line>
        400e37:	48 89 c7             	mov    %rax,%rdi
        400e3a:	e8 a1 00 00 00       	callq  400ee0 <phase_1>
        400e3f:	e8 80 07 00 00       	callq  4015c4 <phase_defused>
    ... 
    ```

    The result of ***<read_line>*** is put into `%rdi`
    &rArr; The `%rsi` stores the address of the answer.
    &rArr; The address of the answer is ***$0x402400***.

### Step 3. Get the content of answer at runtime  
      
1. Debug the bomb using GDB
    cmd: `gdb bomb`
2. Set the break point
    cmd: `b 74`
3. print the string at ***$0x402400***
    cmd: `x/s 0x402400` 
    &rArr; We get ***"Border relations with Canada have never been better."***

The answer of phase 1 is **"Border relations with Canada have never been better."**
    
    
---    
    
### Phase 2

```C
    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");
```
    
### Step 1. main

```
...
400e49:	e8 c2 fc ff ff       	callq  400b10 <puts@plt>
400e4e:	e8 4b 06 00 00       	callq  40149e <read_line>
400e53:	48 89 c7             	mov    %rax,%rdi
400e56:	e8 a1 00 00 00       	callq  400efc <phase_2>
400e5b:	e8 64 07 00 00       	callq  4015c4 <phase_defused>
400e60:	bf ed 22 40 00       	mov    $0x4022ed,%edi
...
```
&rArr; `%rdi` stores the address of input string

### Step 2. phase_2

```
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3
```

```
sub    $0x28,%rsp
mov    %rsp,%rsi
```
&rArr; Assign 0x28 bytes spaces on the stack and pass the stack top as arg2

call `read_six_numbers`

| Arguments | Memory | Description |
|:--------- |:------ |:----------- |
|arg1|%rdi|input str|
|arg2|%rsi|stack top of phase_2|
 
### Step 3. read_six_numbers    
```
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp
  401460:	48 89 f2             	mov    %rsi,%rdx
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax
  401474:	48 89 04 24          	mov    %rax,(%rsp)
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi
  401485:	b8 00 00 00 00       	mov    $0x0,%eax
  40148a:	e8 61 f7 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  40148f:	83 f8 05             	cmp    $0x5,%eax
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d>
  401494:	e8 a1 ff ff ff       	callq  40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp
  40149d:	c3                   	retq
```

call `sscanf`

Note. sscanf is in C standard library. Ref to [sscanf](https://www.ibm.com/docs/en/i/7.1?topic=functions-sscanf-read-data)

| Arguments | Memory | Description |
|:--------- |:------ |:----------- |
|arg1|%rdi|input str|
|arg2|%rsi|0x4025c3 (unknown)|
|arg3|%rdx|stack top of phase_2|
|arg4|%rcx|stack top of phase_2 + 0x4|
|arg5|%r8|stack top of phase_2 + 0x8|
|arg6|%r9|stack top of phase_2 + 0xC|
|arg7|stack|stack top of phase_2 + 0x10|
|arg8|stack|stack top of phase_2 + 0x14|

Let's find out what the arg2 is:

(1) Debug mode: `gdb bomb`<br/>
(2) set the breakpoint: `b 82`<br/>
(3) Show: `x/s 0x4025c3` <br />
   Result: `0x4025c3:	"%d %d %d %d %d %d"`

According to the sscanf, `%d %d %d %d %d %d` is a string format.<br/>
&rArr;sscanf tends to extract six numbers from input string.<br/>
&rArr;we could imply that the result of six numbers will put in **arg3 ~ arg8** which is a sequence of memory on stack top of phase_2

### Step 4. Find the answer

After all, answer should like [num1 num2 num3 num4 num5 num6].

```
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq
```
- **Flow 1**<br/>
    ```
    400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp) <br/>
    400f0e:	74 20                	je     400f30 <phase_2+0x34>
    400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
    ```
    &rArr; num1 = 1
- **Flow 2**<br/>
    ```
    400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
    400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
    400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
    400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
    400f1a:	01 c0                	add    %eax,%eax
    400f1c:	39 03                	cmp    %eax,(%rbx)
    400f1e:	74 05                	je     400f25 <phase_2+0x29>
    400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
    ```
    &rArr; num2 = num1 + num1 = 2

- **Flow 3**<br/>
    ```
    400f25:	48 83 c3 04          	add    $0x4,%rbx
    400f29:	48 39 eb             	cmp    %rbp,%rbx
    400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
    400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
    (Branch 1)
    400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
    400f1a:	01 c0                	add    %eax,%eax
    400f1c:	39 03                	cmp    %eax,(%rbx)
    400f1e:	74 05                	je     400f25 <phase_2+0x29>
    400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
    (Branch 2)
    400f3c:	48 83 c4 28          	add    $0x28,%rsp
    400f40:	5b                   	pop    %rbx
    400f41:	5d                   	pop    %rbp
    400f42:	c3                   	retq
    ```
    Branch 2 &rArr; enter this branch if all number are checked<br/>
    Branch 1 &rArr; num3 = num2 + num2    
    
    After **branch 1** it jumps back to 400f25 which does the same       things until all six number are checked.
    
    &rArr; num4 = num3 + num3<br/>
    &rArr; num5 = num4 + num4<br/>
    &rArr; num6 = num5 + num5<br/>
    
    &rArr; ***answer = "1 2 4 8 16 32"***

---

### Phase 3
    
### Step 1. main
```
...
  400e60:	bf ed 22 40 00       	mov    $0x4022ed,%edi
  400e65:	e8 a6 fc ff ff       	callq  400b10 <puts@plt>
  400e6a:	e8 2f 06 00 00       	callq  40149e <read_line>
  400e6f:	48 89 c7             	mov    %rax,%rdi
  400e72:	e8 cc 00 00 00       	callq  400f43 <phase_3>
  400e77:	e8 48 07 00 00       	callq  4015c4 <phase_defused>
...
```

&rArr; `%rdi` stores the address of the input string.

### Step 2. phase_3
```
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq
```
- Assign 0x18 bytes space on stack
- call **sscanf** <br/>

| Arguments | Memory | Description |
|:----------|:-------|:------------|
|arg 1|%rdi|addr of input string|
|arg 2|%rsi|0x4025cf addr of string format|
|arg 3|%rdx|addr of stack top + 0x8|
|arg 4|%rcx|addr of stack top + 0xc|


Let's see what's the content at 0x4025cf<br/>
(1) Debug mde: `gdb bomb answer.txt`<br/>
(2) Print content: `x/s 0x4025cf` <br/>
&rArr; result: `%d %d`<br/>
&rArr; we could expect that sscanf put the result into `(%rdx)` and `(%rcx)` <br/>
&rArr; `0x8(%rsp)` is the first integer in input string <br/>
&rArr; `0xc(%rsp)` is the second integer in input string


### Step 3.  Analyze phase_3 after sscanf

The answer is in form of `[num1 num2]` which is `[0x8(%rsp) 0xc(%rsp)]`

    
```
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   
```

```
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
```
&rArr; If the extracted numbers from input string <= 1, explode bomb.

```
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  ...
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
```
- If number at `0x8(%rsp)` >= 7, explode bomb.
- `400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)` jumps to the address stored in `M[0x402470 + 8 * %rax]`<br/>
  which equivalent to `M[0x402470 + 8 * num1]`

Let's see what's in 0x402470<br/>
(1) Debug: `gdb bomb`<br/>
(2) Set breakpoint: `b 89`<br/>
(3) Find what's in 0x402470: `x/6gx 0x402470`<br/>
```
0x402470:	0x0000000000400f7c	0x0000000000400fb9
0x402480:	0x0000000000400f83	0x0000000000400f8a
0x402490:	0x0000000000400f91	0x0000000000400f98
```
    
&rArr; pick one address `0x0000000000400fb9` which is `M[0x402470 + 8 * 1]`
    
```
400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
400fc2:	74 05                	je     400fc9 <phase_3+0x86>
400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
400fc9:	48 83 c4 18          	add    $0x18,%rsp
400fcd:	c3                   	retq 
```
    
&rArr; num1 = 1<br/>
&rArr; num2 = 0x137 = 311<br/>

&rArr; answer = ***"1 311"***

### Phase 4
### Step 1. main
```
...
  400e7c:	bf 0b 23 40 00       	mov    $0x40230b,%edi
  400e81:	e8 8a fc ff ff       	callq  400b10 <puts@plt>
  400e86:	e8 13 06 00 00       	callq  40149e <read_line>
  400e8b:	48 89 c7             	mov    %rax,%rdi
  400e8e:	e8 79 01 00 00       	callq  40100c <phase_4>
  400e93:	e8 2c 07 00 00       	callq  4015c4 <phase_defused>
...
```

&rArr; %rdi stores the address of the input string.

### Step 2. phase_4
```
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq   
```
- Assign 0x18 bytes space on stack

- call **sscanf**

|Arguments|Memory|Description|
|:--------|:-----|:----------|
|arg 1|%rdi|addr of input string|
|arg 2|%rsi|0x4025cf addr of string format|
|arg 3|%rdx|addr of stack top + 0x8|
|arg 4|%rcx|addr of stack top + 0xc|


Let's see what the content at 0x4025cf
(1) Debug mde: `gdb bomb answer.txt`<br/>
(2) Print content: `x/s 0x4025cf` <br/>
&rArr; result: `%d %d`<br/>
&rArr; we could expect that sscanf put the result into `(%rdx)` and `(%rcx)` <br/>
&rArr; '0x8(%rsp)' get the first integer in input string <br/>
       '0xc(%rsp)' get the second integer in input string


### Step 3.  Analyze phase_4 after sscanf

The answer is in form of `[num1 num2]` = `[0x8(%rsp) 0xc(%rsp)]`

```
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq  
```
&rArr; If extracted numbers from input string != 2, explode bomb.

```
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
```
&rArr; If `num1` > 14, explode bomb.<br/>
&rArr; `num1` <= 14<br/>

```
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
```
- call **func4**

|Arguments|Memory|Description|
|:--------|:-----|:----------|
|arg 1|%rdi|0x8(%rsp) which is `num1`|
|arg 2|%rsi|0|
|arg 3|%rdx|0xe|

- After func4:

```
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq  
```
&rArr; **the result of func4 should be 0** <br/>
&rArr; **num2 = 0** <br/>

### Step 4. Figure out func4

```
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp
  400fd2:	89 d0                	mov    %edx,%eax
  400fd4:	29 f0                	sub    %esi,%eax
  400fd6:	89 c1                	mov    %eax,%ecx
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx
  400fdb:	01 c8                	add    %ecx,%eax
  400fdd:	d1 f8                	sar    %eax
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx
  400fe2:	39 f9                	cmp    %edi,%ecx
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24>
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx
  400fe9:	e8 e0 ff ff ff       	callq  400fce <func4>
  400fee:	01 c0                	add    %eax,%eax
  400ff0:	eb 15                	jmp    401007 <func4+0x39>
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax
  400ff7:	39 f9                	cmp    %edi,%ecx
  400ff9:	7d 0c                	jge    401007 <func4+0x39>
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi
  400ffe:	e8 cb ff ff ff       	callq  400fce <func4>
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401007:	48 83 c4 08          	add    $0x8,%rsp
  40100b:	c3                   	retq   
```

- It calls itself in the function. &rArr; Recursion.
- After analyzing the code above, we can reverse it back to C like:
```C
int func4(int num1, int i, int j) /* %rdi, %rsi, %rdx */
{
  int tmp = j - i;
  tmp = (unsigned)(tmp) >> 31 + tmp;
  tmp >>= 1;
  tmp += i;
  if (tmp > num1) {
    int res = func4(num1, i, tmp-1);
    return 2 * res;
  } else { // tmp <= num1
    if (tmp >= num1) {
      return 0;
    } else {
      int res = func4(num1, tmp+1, j);
      return (1 + 2 * res);
    }
  }
}
```

By Step.3 we know that the result of `func4(num1, 0, 0xe)` should be 0. <br/>
&rArr;  We should set num1 so that `tmp == num1` <br/>
&rArr; `((((unsigned)(j-i) >> 31) + (j - i)) >> 1) == num1` <br/>
&rArr; **num1 = 7 or 0** <br/> 

&rArr; answer = ***"7 0"*** or ***"0 0"***
    

### Phase 5
### Step 1: main

```
  400e98:	bf d8 23 40 00       	mov    $0x4023d8,%edi
  400e9d:	e8 6e fc ff ff       	callq  400b10 <puts@plt>
  400ea2:	e8 f7 05 00 00       	callq  40149e <read_line>
  400ea7:	48 89 c7             	mov    %rax,%rdi
  400eaa:	e8 b3 01 00 00       	callq  401062 <phase_5>
  400eaf:	e8 10 07 00 00       	callq  4015c4 <phase_defused>
  400eb4:	bf 1a 23 40 00       	mov    $0x40231a,%edi
  400eb9:	e8 52 fc ff ff       	callq  400b10 <puts@plt>
  400ebe:	e8 db 05 00 00       	callq  40149e <read_line>
  400ec3:	48 89 c7             	mov    %rax,%rdi
```
&rArr; %rdi stores the address of the input string.

### Step 2: phase_5

```
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  ...
```
&rArr; Distribute 32 bytes to stack.  
&rArr; Put a canary on rsp+0x18.  
&rArr; The input string should have 6 characters exactly.  
&rArr; Put the pointer of input string to ***%rbx***.  

|Reg|Description|
|:-----|:----------|
|%rbx|pointer to input string|

```
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>

  ...
<br/>x
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
  4010a4:	48 83 c0 01          	add    $0x1,%rax
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>

  ...

  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```
&rArr; ***%eax*** is an index (i) through the string.  
&rArr; `40108b ~ 401099` get the character at addr (input_str[i] & 0xf) + 0x4024b0  
&rArr; `4010a0` put the character at `%rsp + 0x10)` until (i == 6)  
&rArr; Compare the string at `(%rsp + 0x10)` to the string at `0x40245e`  
    

### Step 3: Get the answer through gdb

Find the content at `0x4024b0` and `0x40245e`

1. Debug the bomb: `gdb bomb`
2. Set the breakpoint at line 100: `b 100`
3. Run the program: `run bomb answer.txt`
4. Print the string at 0x4024b0: `x/s 0x4024b0`  
    &rArr; "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"
5. Print the string at 0x40245e: `x/s 0x40245e`   
    &rArr; "flyers"

`The corresponding index of "flyers" in the string at 0x4024b0 is : 9 15 14 5 6 7`
    &rArr; The answer is `ionefg`
    
## Appendix
### x86-64 registers convention
![](https://i.imgur.com/A1BeC7a.png)

    
## Reference
[GDB displays the memory contents.](https://visualgdb.com/gdbreference/commands/x)
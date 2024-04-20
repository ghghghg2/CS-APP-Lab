# Table of contents

- [Brief Introduction](#brief-introduction)
- [Labs](#labs)
  - [Lab0 - Data Lab](#lab0---data-lab)
  - [Lab1 - Bomb Lab](#lab1---bomb-lab)
  - [Lab2 - Attack Lab](#lab2---attack-lab)
  - [Lab3 - Cache Lab](#lab3---cache-lab)
  - [Lab4 - Shell Lab](#lab4---shell-lab)

### Brief Introduction
The repo records the process of learning open course [CSAPP](https://csapp.cs.cmu.edu/3e/waside.html).  

### Labs  

#### Lab0 - Data Lab
> Students implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C. For example, they might be asked to compute the absolute value of a number using only bit-level operations and straightline code. This lab helps students understand the bit-level representations of C data types and the bit-level behavior of the operations on data.  

See [here](Lab/lab0_data/datalab.pdf) for more detail of the assignment requirement.  

See [here](Lab/lab0_data/datalab-handout/bits.c) for my solution.


#### Lab1 - Bomb Lab
> A "binary bomb" is a program provided to students as an object code file. When run, it prompts the user to type in 6 different strings. If any of these is incorrect, the bomb "explodes," printing an error message and logging the event on a grading server. Students must "defuse" their own unique bomb by disassembling and reverse engineering the program to determine what the 6 strings should be. The lab teaches students to understand assembly language, and also forces them to learn how to use a debugger. It's also great fun. A legendary lab among the CMU undergrads.  

See [here](Lab/lab1_bomb/Doc/bomblab.pdf) for more detail of the assignment requirement.  

See [here](Lab/lab1_bomb/) for my solution.  

#### Lab2 - Attack Lab
>　Students are given a pair of unique custom-generated x86-64 binary executables, called targets, that have buffer overflow bugs. One target is vulnerable to code injection attacks. The other is vulnerable to return-oriented programming attacks. Students are asked to modify the behavior of the targets by developing exploits based on either code injection or return-oriented programming. This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.　　

See [here](Lab/lab2_attackLab/attacklab.pdf) for more detail of the assignment requirement.  

See [here](Lab/lab2_attackLab/) for my solution.  

#### Lab3 - Cache Lab
> Students write a general-purpose cache simulator, and then optimize a small matrix transpose kernel to minimize the number of misses on a simulated cache. This lab uses the Valgrind tool to generate address traces.  

See [here](Lab/lab3_cachelab/cachelab.pdf) for more detail of the assignment requirement.  

See [here](Lab/lab3_cachelab/) for my solution.  

#### Lab4 - Shell Lab
> Students implement their own simple Unix shell program with job control, including the ctrl-c and ctrl-z keystrokes, fg, bg, and jobs commands. This is the students' first introduction to application level concurrency, and gives them a clear idea of Unix process control, signals, and signal handling.  

See [here](Lab/lab4_shellLab/shlab.pdf) for more detail of the assignment requirement.  

See [here](Lab/lab4_shellLab/) for my solution.  
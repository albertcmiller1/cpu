# cpu

## x86 assembly language interpreter

## x86 Architecture Basics
* Registers 
    `EAX`, `EBX`, `ECX`, `EDX`, `ESP`, `EBP`, `EFLAGS`
* Instruction Set 
    Instructions operate on registers, memory, and I/O
    `MOV`, `ADD`, `SUB`, `CMP`, `JMP`
* Addressing Modes 
    Direct, indirect, register, and immediate addressing modes 
* Flags and Condition Codes 
    Operations often affect flags which influence conditional instructions like `JZ`
    (Zero Flag, sign Flag, Carry Flag, etc)

## Project Scope 
The full x86 instruction set is too large and complex, so we will start with the basics 
* Data Transfer: `MOV`, `PUSH`, `POP`
* Arithmetic/Logic Operations: `ADD`, `SUB`, `MUL`, `DIV`, `AND`, `OR`
* Control Flow: `JMP`, `CALL`, `RET`, `CMP`, `JE`, `JNE`
* Basic I/O Operations 


## Project Structure
Need to mimic how a CPU process instructions 
1. Fecth - read the current instruction pointed to by the program counter from memory 
2. Decode - break down the instruction to determine the operation and operands 
3. Execute - Perform the operation, such as moving data, performing arithmetic, or jumping to another instruction 

## Defining CPU Key Components as Data Structures 
1. Registers 
    * Use array or struct to store the state of the general purpose registers 
    * Also the stack pointer `ESP`
    * Also the base pointer `EBP`
    * Also the instruction pointer `EIP`

2. Memory 
    * Create an array to simulate RAM 
    * Store both the program code and data in memory, with separate regions for stack and heap 

3. Instruction Pointer (IP/PC)
    * Keeps track of the current position in the program 
    * After each instruction, increment it to fetch the next instruction 

4. Flags 
    * Simulate the status flags (Essential for conditional instructions)
    * Zero, Sign, Carry, Overflow 

5. Stack
    * Implement a stack for storing return addresses during function calls 
    * `CALL`, `RET`
    * Also for pushing and popping data to/from memory 
    * `PUSH`, `POP`

## Instruction Set Decoding 
For each instruction, we might 
    * Store an opcode 
    * Have operatnds (registers, memory addresses, etc)

## Execution Logic 
For each supported instruction, write a function to handle its execution 
    * `MOV` should move data betwen registers or between registers and memory 
    * `ADD`, `SUB` should perform the necessary arithmetic operation and update the flags 
    * `JMP`, `CALL`, `RET` should modify the instruction pointer and manage the stack for function calls and returns 
    * `JE`, `JNE` should check the status of the flags and decide weather to jump to a new instruction 


## hello world assembly
https://www.youtube.com/watch?v=d0OXp0zqIo0&ab_channel=ChrisHay
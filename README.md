# Goals 
The goal of this project is to better understand binary executable files, assembly, and how a CPU processes instructions. The project will be broken down into two primary objectives: 
1. Read each machine code instruction of a binary file and convert that instruction to assembly 
2. Mimic the execution loop of a CPU to fetch, decode, and execute each assembly instruction found from step 1 

Effectivly, these will replicate the objdump command and an assembly language interpreter

# MVP 
* Replicate the objdump command 
    1. Compile a simple .c, .cpp, or .s file into a binary mach-0 file
    2. Read this binary file into memory 
    3. Find the TEXT section of this binary file which contains the program's ARM64 machine code instructions
    4. Read in each of these instructions as hexidecimal 
    5. Map the binary opcodes to their corresponding assembly instructions
* Create a CPU class to fetch, decode, and execute each assembly instruction 
    1. Fetch - read the current instruction pointed to by the program counter from memory 
    2. Decode - break down the instruction to determine the operation and operands 
    3. Execute - Perform the operation, such as moving data, performing arithmetic, or jumping to another instruction 

# Scope 
The full x86 instruction set is too large and complex, so we will start with the basics. If a command is found in the binary file being read which is out of scope, the program will throw an excption and exit. 
* Data Transfer: `MOV`, `PUSH`, `POP`
* Arithmetic/Logic Operations: `ADD`, `SUB`, `MUL`, `DIV`, `AND`, `OR`
* Control Flow: `JMP`, `CALL`, `RET`, `CMP`, `JE`, `JNE`
* Basic I/O Operations 


# Notes 
* What are registers? 
* What are instructions? 
* What are addressing mode? 
* What are flags and codition codes? 


### x86 Architecture Basics
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



### Defining CPU Key Components as Data Structures 
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

### Instruction Set Decoding 
For each instruction, we might 
    * Store an opcode 
    * Have operatnds (registers, memory addresses, etc)

### Execution Logic 
For each supported instruction, write a function to handle its execution 
    * `MOV` should move data betwen registers or between registers and memory 
    * `ADD`, `SUB` should perform the necessary arithmetic operation and update the flags 
    * `JMP`, `CALL`, `RET` should modify the instruction pointer and manage the stack for function calls and returns 
    * `JE`, `JNE` should check the status of the flags and decide weather to jump to a new instruction 


### Links to better understand Assembly, CPUs, and Binary files 
* https://www.youtube.com/watch?v=d0OXp0zqIo0&ab_channel=ChrisHay
* https://medium.com/@travmath/understanding-the-mach-o-file-format-66cf0354e3f4


### ARM64 Instruction ISA 
* https://valsamaras.medium.com/arm-64-assembly-series-basic-definitions-and-registers-ec8cc1334e40 
* https://weinholt.se/articles/arm-a64-instruction-set/#:~:text=The%20instruction%20set%20is%20called,register%20that%20always%20contains%20zeroes
* https://en.wikipedia.org/wiki/AArch64 
* https://developer.arm.com/documentation/ddi0596/latest/ 


std::move -> purpose and what problem is solves 
what is a dead lock 
which is grener
> thread waiting on an atomic bool > spin lock 
> mutex lock 



// hello world 

.global _main 
.align 2

_main: 
    b _printf
    b _terminate

_printf: 
    mov X0, #1              //stdout 
    adr X1, helloworld      // address of hello world string 
    mov X2, #12             // length of hello world string 
    mov X16, #4             // write to stdout 
    svc 0                   // syscall 

_terminate: 
    mov X0, #0      // return 0
    mov X16, #1     // terminate 
    svc 0           // syscall, super visor call

helloworld: .ascii "hello world\n"
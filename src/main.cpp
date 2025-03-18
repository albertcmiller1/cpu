// #include "disassembler.hpp"
// int main(int argc, char* argv[]) {
//     std::string binary_file_path = "./binaries/hello";
//     AArch64Disassembler disassembler;

//     std::vector<uint8_t> binary_file = disassembler.get_binary_file_content(binary_file_path);
//     section_64* text_sect = disassembler.get_instructions_from_file(binary_file);
//     std::cout << "\n\n" << std::endl;

//     std::vector<std::string> assembly_instructions = disassembler.get_instructions(binary_file, text_sect->offset, text_sect->size);

//     for (auto instr : assembly_instructions){
//         std::cout << instr << std::endl;
//     }
// }

#include "cpu.cpp"
int main() {
    // Your list of instructions as uint32_t values
    std::vector<uint32_t> program = {
        0xd10083ff,  // sub sp, sp, #0x20
        0xa9017bfd,  // stp x29, x30, [sp, #0x10]
        0x910043fd,  // add x29, sp, #0x10
        0x52800008,  // mov w8, #0
        0xb9000be8,  // str w8, [sp, #8]
        0xb81fc3bf,  // stur wzr, [x29, #-4]
        0x90000000,  // adrp x0, #0x3000
        0x913eb000,  // add x0, x0, #0xfac
        0x94000005,  // bl #0x3fa0
        0xb9400be0,  // ldr w0, [sp, #8]
        0xa9417bfd,  // ldp x29, x30, [sp, #0x10]
        0x910083ff,  // add sp, sp, #0x20
        0xd65f03c0   // ret
    };
    
    AArch64CPU cpu;
    cpu.loadProgram(program);
    
    std::string input;
    while (true) {
        std::cout << "Press Enter to step, 'r' to run to completion, or 'q' to quit: ";
        std::getline(std::cin, input);
        
        if (input == "q") {
            break;
        } else if (input == "r") {
            cpu.run();
            break;
        } else {
            if (!cpu.step()) {
                std::cout << "Program finished execution." << std::endl;
                break;
            }
        }
    }
    
    return 0;
}
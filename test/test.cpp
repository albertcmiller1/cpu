// int run_primary() {
//     // Your list of instructions
//     std::vector<uint32_t> instructions = {
//         0xd10083ff, 
//         0xa9017bfd, 
//         0x910043fd, 
//         0x52800008,
//         0xb9000be8, 
//         0xb81fc3bf, 
//         0x90000000, 
//         0x913eb000,
//         0x94000005, 
//         0xb9400be0, 
//         0xa9417bfd, 
//         0x910083ff,
//         0xd65f03c0
//     };
    
//     AArch64Disassembler disassembler;

//     for (size_t i = 0; i < instructions.size(); i++) {
//         uint32_t offset = 16236 + i * 4; // Starting from offset 16236

//         std::cout << "Processing instruction" << std::endl;
//         std::cout << "\tInstruction Hex: " << "0x" << std::hex << std::setw(8) << std::setfill('0') << instructions[i] << std::endl;
//         std::cout << "\tInstruction Binary: " << disassembler.to_binary_str(instructions[i]) << std::endl;
//         std::cout << "\tInstruction Offset: " << offset << std::endl;
//         std::string assembly = disassembler.disassemble(instructions[i]);
//         std::cout << "\tInstruction Assembly: " << assembly << std::endl;
//         std::cout << "\n";
//     }

//     return 0;
// }

// int run_capstone() {
//     // Your list of instructions
//     std::vector<uint32_t> instructions = {
//         0xd10083ff, 0xa9017bfd, 0x910043fd, 0x52800008,
//         0xb9000be8, 0xb81fc3bf, 0x90000000, 0x913eb000,
//         0x94000005, 0xb9400be0, 0xa9417bfd, 0x910083ff,
//         0xd65f03c0
//     };
    
//     AArch64Disassembler disassembler;
    
//     for (size_t i = 0; i < instructions.size(); i++) {
//         uint32_t offset = 16236 + i * 4; // Starting from offset 16236
//         std::string assembly = disassembler.capstone_disassemble(instructions[i], offset);
//         std::cout << "Instruction at offset " << offset << ": 0x" 
//                   << std::hex << std::setw(8) << std::setfill('0') << instructions[i] 
//                   << " -> " << assembly << std::endl;
//     }
    
//     return 0;
// }
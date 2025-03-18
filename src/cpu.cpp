#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <functional>
#include <iomanip>
#include <capstone/capstone.h>

class AArch64CPU {
private:
    // Register file - 31 general purpose registers (x0-x30) plus SP
    uint64_t registers[32];
    
    // Program counter
    uint64_t pc;
    
    // Memory representation (simple implementation)
    std::unordered_map<uint64_t, uint8_t> memory;
    
    // Instruction memory
    std::vector<uint32_t> instructions;
    
    // Instruction pointer (current index in instructions vector)
    size_t instr_ptr;
    
    // Capstone handle for disassembly
    csh cs_handle;
    bool cs_initialized;
    
    // Helper methods
    void initializeRegisters() {
        for (int i = 0; i < 31; i++) {
            registers[i] = 0;
        }
        // Initialize stack pointer (x31/sp)
        registers[31] = 0x10000;  // Arbitrary stack start
    }
    
    uint32_t fetch() {
        if (instr_ptr >= instructions.size()) {
            return 0; // No more instructions
        }
        uint32_t instruction = instructions[instr_ptr++];
        pc += 4;
        return instruction;
    }
    
    std::string disassemble(uint32_t instruction, uint64_t address) {
        if (!cs_initialized) {
            return "disassembly_not_available";
        }
        
        uint8_t bytes[4];
        bytes[0] = instruction & 0xFF;
        bytes[1] = (instruction >> 8) & 0xFF;
        bytes[2] = (instruction >> 16) & 0xFF;
        bytes[3] = (instruction >> 24) & 0xFF;
        
        cs_insn *insn;
        size_t count = cs_disasm(cs_handle, bytes, sizeof(bytes), address, 1, &insn);
        
        std::string result;
        if (count > 0) {
            result = std::string(insn[0].mnemonic) + " " + std::string(insn[0].op_str);
            cs_free(insn, count);
        } else {
            result = "unknown_instruction";
        }
        
        return result;
    }
    
    // Memory access helpers
    void writeMemory(uint64_t address, uint64_t value, size_t size) {
        for (size_t i = 0; i < size; i++) {
            memory[address + i] = (value >> (i * 8)) & 0xFF;
        }
    }
    
    uint64_t readMemory(uint64_t address, size_t size) {
        uint64_t value = 0;
        for (size_t i = 0; i < size; i++) {
            value |= static_cast<uint64_t>(memory[address + i]) << (i * 8);
        }
        return value;
    }
    
    // Register access helpers (handling w registers vs x registers)
    void writeRegister(int reg, uint64_t value, bool is32bit = false) {
        if (is32bit) {
            // Clear upper 32 bits and set lower 32 bits
            registers[reg] = (registers[reg] & 0xFFFFFFFF00000000) | (value & 0xFFFFFFFF);
        } else {
            registers[reg] = value;
        }
    }
    
    uint64_t readRegister(int reg, bool is32bit = false) {
        if (is32bit) {
            return registers[reg] & 0xFFFFFFFF;
        }
        return registers[reg];
    }
    
    // Execute functions for different instruction types
    void executeSub(uint32_t instruction) {
        // Example implementation for "sub sp, sp, #imm"
        int rd = instruction & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 10) & 0xFFF) << ((instruction >> 22) & 0x3);
        
        registers[rd] = registers[rn] - imm;
        
        // Debug output
        std::cout << "Executed: sub " << (rd == 31 ? "sp" : "x"+std::to_string(rd))
                  << ", " << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << std::endl;
    }
    
    void executeAdd(uint32_t instruction) {
        // Example implementation for "add x29, sp, #imm" or "add x0, x0, #imm"
        int rd = instruction & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 10) & 0xFFF) << ((instruction >> 22) & 0x3);
        
        registers[rd] = registers[rn] + imm;
        
        // Debug output
        std::cout << "Executed: add " << (rd == 31 ? "sp" : "x"+std::to_string(rd))
                  << ", " << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << std::endl;
    }
    
    void executeStp(uint32_t instruction) {
        // Implementation for "stp x29, x30, [sp, #0x10]"
        int rt = instruction & 0x1F;
        int rt2 = (instruction >> 10) & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 15) & 0x7F) << 3;
        
        uint64_t address = registers[rn] + imm;
        writeMemory(address, registers[rt], 8);
        writeMemory(address + 8, registers[rt2], 8);
        
        // Debug output
        std::cout << "Executed: stp " << (rt == 31 ? "sp" : "x"+std::to_string(rt))
                  << ", " << (rt2 == 31 ? "sp" : "x"+std::to_string(rt2))
                  << ", [" << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << "]" << std::endl;
    }
    
    void executeLdp(uint32_t instruction) {
        // Implementation for "ldp x29, x30, [sp, #0x10]"
        int rt = instruction & 0x1F;
        int rt2 = (instruction >> 10) & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 15) & 0x7F) << 3;
        
        uint64_t address = registers[rn] + imm;
        registers[rt] = readMemory(address, 8);
        registers[rt2] = readMemory(address + 8, 8);
        
        // Debug output
        std::cout << "Executed: ldp " << (rt == 31 ? "sp" : "x"+std::to_string(rt))
                  << ", " << (rt2 == 31 ? "sp" : "x"+std::to_string(rt2))
                  << ", [" << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << "]" << std::endl;
    }
    
    void executeMov(uint32_t instruction) {
        // Implementation for "mov w8, #0"
        int rd = instruction & 0x1F;
        uint16_t imm16 = (instruction >> 5) & 0xFFFF;
        int hw = (instruction >> 21) & 0x3;
        
        uint64_t value = imm16 << (hw * 16);
        writeRegister(rd, value, true);  // 32-bit register
        
        // Debug output
        std::cout << "Executed: mov w" << rd
                  << ", #0x" << std::hex << value << std::dec << std::endl;
    }
    
    void executeStr(uint32_t instruction) {
        // Implementation for "str w8, [sp, #8]"
        int rt = instruction & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 10) & 0xFFF) << ((instruction >> 30) & 0x3);
        bool is32bit = ((instruction >> 30) & 0x3) != 3;  // Size bits
        
        uint64_t address = registers[rn] + imm;
        uint64_t value = readRegister(rt, is32bit);
        writeMemory(address, value, is32bit ? 4 : 8);
        
        // Debug output
        std::cout << "Executed: str " << (is32bit ? "w" : "x") << rt
                  << ", [" << (rn == 31 ? "sp" : (is32bit ? "w" : "x")+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << "]" << std::endl;
    }
    
    void executeStur(uint32_t instruction) {
        // Implementation for "stur wzr, [x29, #-4]"
        int rt = instruction & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 12) & 0x1FF);
        if (imm & 0x100) imm |= 0xFFFFFFFFFFFFFE00; // Sign extend
        bool is32bit = ((instruction >> 30) & 0x3) != 3;  // Size bits
        
        uint64_t address = registers[rn] + imm;
        uint64_t value = rt == 31 ? 0 : readRegister(rt, is32bit);
        writeMemory(address, value, is32bit ? 4 : 8);
        
        // Debug output
        std::cout << "Executed: stur " << (rt == 31 ? (is32bit ? "wzr" : "xzr") : ((is32bit ? "w" : "x")+std::to_string(rt)))
                  << ", [" << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #" << imm << "]" << std::endl;
    }
    
    void executeLdr(uint32_t instruction) {
        // Implementation for "ldr w0, [sp, #8]"
        int rt = instruction & 0x1F;
        int rn = (instruction >> 5) & 0x1F;
        int imm = ((instruction >> 10) & 0xFFF) << ((instruction >> 30) & 0x3);
        bool is32bit = ((instruction >> 30) & 0x3) != 3;  // Size bits
        
        uint64_t address = registers[rn] + imm;
        uint64_t value = readMemory(address, is32bit ? 4 : 8);
        writeRegister(rt, value, is32bit);
        
        // Debug output
        std::cout << "Executed: ldr " << (is32bit ? "w" : "x") << rt
                  << ", [" << (rn == 31 ? "sp" : "x"+std::to_string(rn))
                  << ", #0x" << std::hex << imm << std::dec << "]" << std::endl;
    }
    
    void executeAdrp(uint32_t instruction) {
        // Implementation for "adrp x0, #0x3000"
        int rd = instruction & 0x1F;
        int immlo = (instruction >> 29) & 0x3;
        int immhi = (instruction >> 5) & 0x7FFFF;
        int64_t imm = (static_cast<int64_t>(immhi << 2 | immlo) << 43) >> 43; // Sign extend
        
        // Clear bottom 12 bits of PC and add the page offset
        uint64_t page_address = (pc & ~0xFFF) + (imm << 12);
        registers[rd] = page_address;
        
        // Debug output
        std::cout << "Executed: adrp x" << rd
                  << ", #0x" << std::hex << page_address << std::dec << std::endl;
    }
    
    void executeBl(uint32_t instruction) {
        // Implementation for "bl #0x3fa0"
        int32_t imm26 = instruction & 0x3FFFFFF;
        imm26 = (imm26 << 6) >> 6; // Sign extend
        
        // Save return address
        registers[30] = pc; // LR = PC
        
        // Jump to target
        pc = pc + (imm26 << 2) - 4; // -4 because we already incremented PC in fetch()
        instr_ptr = findInstructionIndex(pc);
        
        // Debug output
        std::cout << "Executed: bl #0x" << std::hex << (imm26 << 2) << std::dec
                  << " (target address: 0x" << std::hex << pc << std::dec << ")" << std::endl;
    }
    
    void executeRet(uint32_t instruction) {
        // Implementation for "ret"
        int rn = (instruction >> 5) & 0x1F;
        if (rn == 0) rn = 30; // Default to x30 (link register)
        
        pc = registers[rn];
        instr_ptr = findInstructionIndex(pc);
        
        // Debug output
        std::cout << "Executed: ret" << (rn != 30 ? " x"+std::to_string(rn) : "") << std::endl;
    }
    
    // Helper to find instruction index from PC
    size_t findInstructionIndex(uint64_t address) {
        // This is a simplified implementation
        // In reality, you would need to map PC values to instruction indices
        return (address - pc_start) / 4;
    }
    
    // Decode and execute an instruction
    bool decodeAndExecute(uint32_t instruction) {
        // Extract the instruction group from the top bits
        uint32_t group = instruction >> 25;
        
        // Data processing immediate
        if ((instruction & 0xFF000000) == 0x91000000) {
            // ADD immediate with 64-bit variant
            executeAdd(instruction);
        }
        else if ((instruction & 0xFF000000) == 0xD1000000) {
            // SUB immediate with 64-bit variant
            executeSub(instruction);
        }
        else if ((instruction & 0xFFC00000) == 0xA9000000) {
            // STP - store pair
            executeStp(instruction);
        }
        else if ((instruction & 0xFFC00000) == 0xA9400000) {
            // LDP - load pair
            executeLdp(instruction);
        }
        else if ((instruction & 0xFFE00000) == 0xD2800000) {
            // MOV - move immediate
            executeMov(instruction);
        }
        else if ((instruction & 0xBFC00000) == 0xB9000000) {
            // STR - store register
            executeStr(instruction);
        }
        else if ((instruction & 0xBFC00000) == 0xB9400000) {
            // LDR - load register
            executeLdr(instruction);
        }
        else if ((instruction & 0xBFE00000) == 0xB8000000) {
            // STUR - store register (unscaled)
            executeStur(instruction);
        }
        else if ((instruction & 0x9F000000) == 0x90000000) {
            // ADRP - form PC-relative address to 4KB page
            executeAdrp(instruction);
        }
        else if ((instruction & 0xFC000000) == 0x94000000) {
            // BL - branch with link
            executeBl(instruction);
        }
        else if ((instruction & 0xFFFFFC1F) == 0xD65F0000) {
            // RET - return from subroutine
            executeRet(instruction);
            // Return false to signal end of execution if this is the final ret
            return false;
        }
        else {
            std::cout << "Unknown instruction: 0x" << std::hex << instruction << std::dec << std::endl;
        }
        
        return true;
    }
    
    uint64_t pc_start;
    
public:
    AArch64CPU() : instr_ptr(0), pc(0), pc_start(0), cs_initialized(false) {
        initializeRegisters();
        
        // Initialize Capstone
        if (cs_open(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, &cs_handle) == CS_ERR_OK) {
            cs_initialized = true;
        }
    }
    
    ~AArch64CPU() {
        if (cs_initialized) {
            cs_close(&cs_handle);
        }
    }
    
    // Load instructions into memory
    void loadProgram(const std::vector<uint32_t>& program, uint64_t start_address = 0x1000) {
        instructions = program;
        pc = start_address;
        pc_start = start_address;
        instr_ptr = 0;
    }
    
    // Run the CPU
    void run() {
        bool running = true;
        
        while (running && instr_ptr < instructions.size()) {
            uint32_t instruction = fetch();
            
            // Display the current instruction
            std::cout << "PC: 0x" << std::hex << (pc - 4) << std::dec 
                      << " Instruction: 0x" << std::hex << instruction << std::dec 
                      << " (" << disassemble(instruction, pc - 4) << ")" << std::endl;
            
            // Decode and execute
            running = decodeAndExecute(instruction);
            
            // Display register state
            std::cout << "Register state:" << std::endl;
            for (int i = 0; i < 32; i++) {
                if (i % 4 == 0 && i > 0) std::cout << std::endl;
                std::string reg_name = (i < 31) ? "x" + std::to_string(i) : "sp";
                std::cout << reg_name << ": 0x" << std::hex << std::setw(16) 
                          << std::setfill('0') << registers[i] << std::dec << " ";
            }
            std::cout << std::endl << std::endl;
        }
    }
    
    // Step through one instruction
    bool step() {
        if (instr_ptr >= instructions.size()) {
            return false;
        }
        
        uint32_t instruction = fetch();
        
        // Display the current instruction
        std::cout << "PC: 0x" << std::hex << (pc - 4) << std::dec 
                  << " Instruction: 0x" << std::hex << instruction << std::dec 
                  << " (" << disassemble(instruction, pc - 4) << ")" << std::endl;
        
        // Decode and execute
        bool result = decodeAndExecute(instruction);
        
        // Display register state
        std::cout << "Register state:" << std::endl;
        for (int i = 0; i < 32; i++) {
            if (i % 4 == 0 && i > 0) std::cout << std::endl;
            std::string reg_name = (i < 31) ? "x" + std::to_string(i) : "sp";
            std::cout << reg_name << ": 0x" << std::hex << std::setw(16) 
                      << std::setfill('0') << registers[i] << std::dec << " ";
        }
        std::cout << std::endl << std::endl;
        
        return result;
    }
    
    // Get register value
    uint64_t getRegister(int index) {
        if (index < 0 || index > 31) {
            std::cerr << "Invalid register index" << std::endl;
            return 0;
        }
        return registers[index];
    }
    
    // Get memory value
    uint64_t getMemory(uint64_t address, size_t size = 8) {
        return readMemory(address, size);
    }
};
#include "disassembler.hpp"

std::string AArch64ManualDisassembler::capstone_disassemble(uint32_t instruction, uint64_t address = 0) const {
    if (!capstone_initialized) {
        return "capstone_not_initialized";
    }
    
    // Convert instruction to bytes (little-endian)
    uint8_t bytes[4];
    bytes[0] = instruction & 0xFF;
    bytes[1] = (instruction >> 8) & 0xFF;
    bytes[2] = (instruction >> 16) & 0xFF;
    bytes[3] = (instruction >> 24) & 0xFF;
    
    cs_insn *insn;
    size_t count = cs_disasm(handle, bytes, sizeof(bytes), address, 1, &insn);
    
    std::string result;
    if (count > 0) {
        result = std::string(insn[0].mnemonic) + " " + std::string(insn[0].op_str);
        cs_free(insn, count);
    } else {
        std::stringstream ss;
        ss << "unknown_0x" << std::hex << std::setw(8) << std::setfill('0') << instruction;
        result = ss.str();
    }
    
    return result;
}


std::string AArch64ManualDisassembler::disassemble(uint32_t instruction) const {
    // Top-level decoding based on instruction groups
    // The instruction is 32 bits 
    // The first 8 bits are the instruction group 
    uint32_t group = instruction >> 25;
    
    // Decode instruction based on group
    if ((group & 0x38) == 0x08) {
        // Data processing immediate
        return disassembleDataProcessing(instruction);
    } else if ((group & 0x3A) == 0x2A) {
        // Branch, exception, system
        return disassembleBranch(instruction);
    } else if ((group & 0x38) == 0x18 || (group & 0x38) == 0x28) {
        // Loads and stores
        return disassembleLoadStore(instruction);
    } else if ((group & 0x3F) == 0x3F) {
        // System instructions
        return disassembleSystemInstruction(instruction);
    }
    
    // If we couldn't identify the instruction
    std::stringstream ss;
    ss << "unknown_0x" << std::hex << std::setw(8) << std::setfill('0') << instruction;
    return ss.str();
}


std::string AArch64ManualDisassembler::disassembleLoadStore(uint32_t instruction) const {
    // Basic load/store handling
    bool is_load = (instruction >> 22) & 1;
    uint32_t size = (instruction >> 30) & 3;
    uint32_t imm12 = (instruction >> 10) & 0xFFF;
    uint32_t rn = (instruction >> 5) & 0x1F;
    uint32_t rt = instruction & 0x1F;
    
    std::stringstream ss;
    
    // LDR/STR detection
    if ((instruction & 0xBFC00000) == 0xB9400000) {
        // LDR with unsigned offset
        if (is_load) {
            int offset = imm12 << size;
            ss << "ldr " << registerName(rt, size == 3) << ", [" 
               << registerName(rn) << ", #" << offset << "]";
        } else {
            int offset = imm12 << size;
            ss << "str " << registerName(rt, size == 3) << ", [" 
               << registerName(rn) << ", #" << offset << "]";
        }
        return ss.str();
    }
    
    // Handle other load/store variations...
    
    return "unimplemented_loadstore";
}

std::string AArch64ManualDisassembler::disassembleBranch(uint32_t instruction) const {
    // Branch handling
    if ((instruction & 0xFC000000) == 0x14000000) {
        // B - unconditional branch
        int32_t imm26 = instruction & 0x3FFFFFF;
        // Sign extend
        imm26 = (imm26 << 6) >> 6;
        // Calculate target (multiplied by 4 as instructions are 4 bytes each)
        int32_t target = imm26 * 4;
        
        std::stringstream ss;
        ss << "b #" << target;
        return ss.str();
    } else if ((instruction & 0xFC000000) == 0x94000000) {
        // BL - branch with link
        int32_t imm26 = instruction & 0x3FFFFFF;
        // Sign extend
        imm26 = (imm26 << 6) >> 6;
        // Calculate target (multiplied by 4 as instructions are 4 bytes each)
        int32_t target = imm26 * 4;
        
        std::stringstream ss;
        ss << "bl #" << target;
        return ss.str();
    } else if ((instruction & 0xFE000000) == 0x54000000) {
        // B.cond - conditional branch
        int32_t imm19 = (instruction >> 5) & 0x7FFFF;
        // Sign extend
        imm19 = (imm19 << 13) >> 13;
        // Calculate target (multiplied by 4 as instructions are 4 bytes each)
        int32_t target = imm19 * 4;
        
        uint32_t cond = instruction & 0xF;
        std::string condStr;
        switch (cond) {
            case 0: condStr = "eq"; break; // Equal
            case 1: condStr = "ne"; break; // Not equal
            case 2: condStr = "hs"; break; // Unsigned higher or same
            case 3: condStr = "lo"; break; // Unsigned lower
            case 4: condStr = "mi"; break; // Minus, negative
            case 5: condStr = "pl"; break; // Plus, positive or zero
            case 6: condStr = "vs"; break; // Overflow
            case 7: condStr = "vc"; break; // No overflow
            case 8: condStr = "hi"; break; // Unsigned higher
            case 9: condStr = "ls"; break; // Unsigned lower or same
            case 10: condStr = "ge"; break; // Signed greater than or equal
            case 11: condStr = "lt"; break; // Signed less than
            case 12: condStr = "gt"; break; // Signed greater than
            case 13: condStr = "le"; break; // Signed less than or equal
            case 14: condStr = "al"; break; // Always
            case 15: condStr = "nv"; break; // Never
            default: condStr = "??"; break;
        }
        
        std::stringstream ss;
        ss << "b." << condStr << " #" << target;
        return ss.str();
    }
    
    // Handle other branch variants...
    
    return "unimplemented_branch";
}

std::string AArch64ManualDisassembler::disassembleDataProcessing(uint32_t instruction) const {
    // Handle data processing instructions
    
    // MOV immediate
    if ((instruction & 0xFFF0001F) == 0xD2800000) {
        uint32_t rd = (instruction >> 0) & 0x1F;
        uint16_t imm16 = (instruction >> 5) & 0xFFFF;
        uint32_t hw = (instruction >> 21) & 0x3;
        
        std::stringstream ss;
        ss << "mov " << registerName(rd) << ", #" << (imm16 << (hw * 16));
        return ss.str();
    }
    
    // ADD immediate
    if ((instruction & 0xFF800000) == 0x91000000) {
        uint32_t rd = (instruction >> 0) & 0x1F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t shift = (instruction >> 22) & 0x3;
        
        uint32_t actual_imm = imm12;
        if (shift == 1) actual_imm <<= 12;
        
        std::stringstream ss;
        ss << "add " << registerName(rd) << ", " << registerName(rn) << ", #" << actual_imm;
        return ss.str();
    }
    
    // SUB immediate
    if ((instruction & 0xFF800000) == 0xD1000000) {
        uint32_t rd = (instruction >> 0) & 0x1F;
        uint32_t rn = (instruction >> 5) & 0x1F;
        uint32_t imm12 = (instruction >> 10) & 0xFFF;
        uint32_t shift = (instruction >> 22) & 0x3;
        
        uint32_t actual_imm = imm12;
        if (shift == 1) actual_imm <<= 12;
        
        std::stringstream ss;
        ss << "sub " << registerName(rd) << ", " << registerName(rn) << ", #" << actual_imm;
        return ss.str();
    }
    
    // Handle other data processing instructions...
    
    return "unimplemented_data_processing";
}

std::string AArch64ManualDisassembler::disassembleSystemInstruction(uint32_t instruction) const {
    // Handle system instructions
    
    // RET
    if ((instruction & 0xFFFFFC1F) == 0xD65F0000) {
        uint32_t rn = (instruction >> 5) & 0x1F;
        
        if (rn == 30) {
            return "ret";
        } else {
            std::stringstream ss;
            ss << "ret " << registerName(rn);
            return ss.str();
        }
    }
    
    // Handle other system instructions...
    
    return "unimplemented_system";
}
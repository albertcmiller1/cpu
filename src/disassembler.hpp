#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <cstdint>  
#include <capstone/capstone.h>

class AArch64ManualDisassembler {
private:
    // capstone stuff 
    csh handle;
    bool capstone_initialized;


    // Helper methods to extract fields from instructions
    // uint32_t extractBits(uint32_t instruction, int start, int length) const {
    //     return (instruction >> start) & ((1 << length) - 1);
    // }
    
    std::string registerName(int regNum, bool is64bit = true) const {
        static const std::vector<std::string> regNames = {
            "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
            "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15",
            "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
            "x24", "x25", "x26", "x27", "x28", "x29", "x30", "sp"
        };
        
        if (regNum < 0 || regNum >= 32) {
            return "invalid_reg";
        }
        
        if (is64bit) {
            return regNames[regNum];
        } else {
            // For 32-bit operations, use 'w' prefix instead of 'x'
            return "w" + regNames[regNum].substr(1);
        }
    }
    
    // Disassembly handlers for different instruction types
    std::string disassembleLoadStore(uint32_t instruction) const;
    std::string disassembleBranch(uint32_t instruction) const;
    std::string disassembleDataProcessing(uint32_t instruction) const;
    std::string disassembleSystemInstruction(uint32_t instruction) const;
    
public:
    AArch64ManualDisassembler() : capstone_initialized(false) {
        // Initialize Capstone
        if (cs_open(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, &handle) == CS_ERR_OK) {
            capstone_initialized = true;
            // Enable detailed mode for more information
            cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
        }
    }
    
    ~AArch64ManualDisassembler() {
        if (capstone_initialized) {
            cs_close(&handle);
        }
    }
    
    // Main disassembly method
    std::string disassemble(uint32_t instruction) const;
    
    // Use capstone 
    std::string capstone_disassemble(uint32_t instruction, uint64_t address) const;
    
    // Process a list of instructions
    std::vector<std::string> disassembleAll(const std::vector<uint32_t>& instructions) const {
        std::vector<std::string> results;
        for (const auto& instr : instructions) {
            results.push_back(disassemble(instr));
        }
        return results;
    }
};

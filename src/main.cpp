#include <iostream>
#include <fstream>
#include <vector>
#include <mach-o/loader.h>
// #include <mach-o/fat.h>

void print_instructions(const std::vector<uint8_t>& file_content, uint64_t offset, uint64_t size) {
    for (uint64_t i = offset; i < offset + size; i += 4) {
        if (i + 3 < file_content.size()) {
            uint32_t instruction = *reinterpret_cast<const uint32_t*>(&file_content[i]);
            std::cout << "Instruction at offset " << i << ": 0x" << std::hex << instruction << std::dec << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mach-o-file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
        return 1;
    }

    std::vector<uint8_t> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (file_content.size() < sizeof(mach_header_64)) {
        std::cerr << "Error: File is too small to be a valid Mach-O file" << std::endl;
        return 1;
    }

    mach_header_64* header = reinterpret_cast<mach_header_64*>(file_content.data());

    if (header->magic != MH_MAGIC_64) {
        std::cerr << "Error: Not a 64-bit Mach-O file" << std::endl;
        return 1;
    }

    if (header->cputype != CPU_TYPE_ARM64) {
        std::cerr << "Error: Not an ARM64 Mach-O file" << std::endl;
        return 1;
    }

    uint64_t offset = sizeof(mach_header_64);
    for (uint32_t i = 0; i < header->ncmds; ++i) {
        if (offset + sizeof(load_command) > file_content.size()) {
            std::cerr << "Error: Unexpected end of file" << std::endl;
            return 1;
        }

        load_command* cmd = reinterpret_cast<load_command*>(&file_content[offset]);

        if (cmd->cmd == LC_SEGMENT_64) {
            segment_command_64* seg = reinterpret_cast<segment_command_64*>(cmd);
            if (std::string(seg->segname) == "__TEXT") {
                for (uint32_t j = 0; j < seg->nsects; ++j) {
                    section_64* sect = reinterpret_cast<section_64*>(seg + 1) + j;
                    if (std::string(sect->sectname) == "__text") {
                        std::cout << "Found __text section at file offset: " << sect->offset << std::endl;
                        std::cout << "Section size: " << sect->size << " bytes" << std::endl;
                        print_instructions(file_content, sect->offset, sect->size);
                        return 0;
                    }
                }
            }
        }

        offset += cmd->cmdsize;
    }

    std::cerr << "Error: __text section not found" << std::endl;
    return 1;
}
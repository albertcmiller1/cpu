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
    // Validate arguments 
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <mach-o-file>" << std::endl;
        return 1;
    }

    std::cout << "Reading binary mach-o file: " << argv[1] << std::endl;


    // create an input file stream object, in binary mode, call it file
    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
        return 1;
    }
    
    // read in the entire content of the file stream [read as raw bytes] into a vector using two iterators 
    std::vector<uint8_t> file_content((
        std::istreambuf_iterator<char>(file)), 
        std::istreambuf_iterator<char>()
    );
    
    file.close();

    // valdiate vector size
    if (file_content.size() < sizeof(mach_header_64)) {
        std::cerr << "Error: File is too small to be a valid Mach-O file" << std::endl;
        return 1;
    }

    // file_content.data() returns a pointer to the underlying array of the std::vector<uint8_t>
    // does this mean if i had an array, and type casted the first few spots of it via any type, i could get that value? 
    mach_header_64* header = reinterpret_cast<mach_header_64*>(file_content.data());

    // validate header 
    if (header->magic != MH_MAGIC_64) {
        std::cerr << "Error: Not a 64-bit Mach-O file" << std::endl;
        return 1;
    }

    // validate header 
    if (header->cputype != CPU_TYPE_ARM64) {
        std::cerr << "Error: Not an ARM64 Mach-O file" << std::endl;
        return 1;
    }


    segment_command_64* big_text_sect;
    uint64_t offset = sizeof(mach_header_64);
    for (uint32_t i = 0; i < header->ncmds; ++i) {
        if (offset + sizeof(load_command) > file_content.size()) {
            std::cerr << "Error: Unexpected end of file" << std::endl;
            return 1;
        }

        // grab sizeof(load_command) bytes from file_content starting at offset
        // cast this into a pointer of load_command type 
        load_command* cmd = reinterpret_cast<load_command*>(&file_content[offset]);

        if (cmd->cmd != LC_SEGMENT_64) {
            offset += cmd->cmdsize;
            continue;
        }

        segment_command_64* seg = reinterpret_cast<segment_command_64*>(cmd);
        
        // search until we find the __TEXT section 
        if (std::string(seg->segname) != "__TEXT") {
            offset += cmd->cmdsize;
            continue;
        }

        big_text_sect = seg;
    }

    section_64* text_sect;
    for (uint32_t j = 0; j < big_text_sect->nsects; ++j) {
        section_64* sect = reinterpret_cast<section_64*>(big_text_sect + 1) + j;
        if (std::string(sect->sectname) != "__text") {
            continue;
        }
        text_sect = sect;
    }
    
    std::cout << "Found __text section at file offset: " << text_sect->offset << std::endl;
    std::cout << "Section size: " << text_sect->size << " bytes" << std::endl;
    print_instructions(file_content, text_sect->offset, text_sect->size);
    return 0;
}


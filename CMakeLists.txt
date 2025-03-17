# cmake_minimum_required(VERSION 3.10)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# project(cpu VERSION 1.0)
# add_executable(cpu src/main.cpp src/disassembler.cpp)

########################################

# cmake_minimum_required(VERSION 3.10)
# project(AArch64Disassembler)

# # Set C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find Capstone package
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(CAPSTONE REQUIRED capstone)

# # Include directories
# include_directories(${CAPSTONE_INCLUDE_DIRS})

# # Add your source files
# add_executable(cpu src/main.cpp)

# # Link against Capstone
# target_link_libraries(cpu ${CAPSTONE_LIBRARIES})

# # Set compiler flags
# target_compile_options(cpu PRIVATE ${CAPSTONE_CFLAGS_OTHER})


########################################
cmake_minimum_required(VERSION 3.10)
project(AArch64Disassembler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/opt/homebrew/Cellar/capstone/5.0.5/include)
link_directories(/opt/homebrew/Cellar/capstone/5.0.5/lib)

# Add your source files
add_executable(disassembler src/main.cpp src/disassembler.cpp)

# Link against Capstone
target_link_libraries(disassembler capstone)
## compile a .C file 
compile: 
	rm -rf binaries/
	mkdir binaries/
	clang assembly/hello.c -c -o binaries/hello.o
	ld binaries/hello.o -o binaries/hello -l System -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _main -arch arm64

## compile an assembly file
binarie: 
	rm -rf binaries/
	mkdir binaries/
	as assembly/hello_world.s -o binaries/hello_world.o
	ld binaries/hello_world.o -o binaries/hello_world -l System -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _main -arch arm64

## compile cpp files using CMake 
build: 
	rm -rf ./build
	mkdir build
	cd ./build/; cmake ..; make;

run: 
	./build/cpu ./binaries/hello

dump: 
	objdump -d ./binaries/hello_world
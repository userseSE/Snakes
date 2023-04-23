# ReadMe

`include` directory is the place for headers.

`src` directory is the place for source files.

`CMakeLists.txt` is the makefile for build system.

You need to have a working c++ compiler like visual c++. You can install vs2019 on windows, or mingw64 gcc.

You can install mingw64 from:
https://www.msys2.org/ 
add system path of `/msys2/usr/bin` and `/msys2/mingw64/bin`
and start the bash terminal, type:
```bash
pacman -Syu 
pacman -S mingw-w64-x86_64-gcc 
```
to install the mingw64 gcc.

You need to install CMake and set the system path for the exe.

The visual studio code is a good editor to write code. C++ CMake and CMake Tools extensions are needed for CMake project environment. 

On Windows, you need to install the Ninjia build system for CMake Tools extension.
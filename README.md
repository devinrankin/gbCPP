# gbCPP
A hobby GameBoy Color Emulator written in C++.
# Technologies
- C++
- CMake
# Building
***This project is built on Unix and only supports Unix-based systems.***
## Dependencies
- CMake (4.2 or newer)
*Note: This project may use C++26 features*
## Build Steps
Input the following commands
```
cd gbCPP
cmake -S src/ -B build
cd build
make
```
This will produce an executable `gbCPP` in the build directory.
# Intent
The intent of this project is to continue learning emulation and virtualization. They're fascinating topics and I'd like to learn more about them. My first project was a [CHIP-8 emulator](https://github.com/devinrankin/c8emu). While that taught me some of the basic principles of emulation, my goal from the start was to play nostalgic games on an emulator that I created. Naturally, the next step is something more dense and challenging.

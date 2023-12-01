# RISC-V 32-bit integer instruction set simulator


## File structure and running the program

File structure and running the program
The code for this project is split into multiple source files and headers.

<b>opcodes.h</b> contains definitions for opcodes for different instruction types
globals.c and globals.h contain definitions of global variables
    instructions.c and instructions.h contain switches for the different instruction types, as well as the function to dump the register contents.
    isasim.c is the main file, containing the main features of the simulator.

To build the program, navigate to the /build folder in the ubuntu terminal, and run:
```
cmake ..
make
```

To run the program navigate to the /build folder in the ubuntu terminal, and run:
./isasim "[arg]"
where 'arg' is the argument consisting of the path for the binary code to be run on the simulator, eg '../task4/t1.bin'


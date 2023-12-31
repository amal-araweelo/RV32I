# RISC-V 32-bit integer instruction set simulator

## File structure and running the program

The code for this project is split into multiple source files and headers.

* <b> opcodes.h </b> contains definitions for opcodes for different instruction types
* <b> globals.c </b> and <b>globals.h</b> contain definitions of global variables
* <b> instructions.c</b> and <b>instructions.h</b>  contain switches for the different instruction types, as well as the function to dump the register contents.
* <b> IsaSim.c is </b> the main file, containing the main features of the simulator

 Navigate to the project folder. Then create and navigate to a build folder in the Ubuntu terminal by running the following:
```
mkdir build
cd build
```
To build the program, run:
```
cmake ..
make
```

To run the program navigate to the ```/build``` folder in the ubuntu terminal, and run:
```./isasim "[arg]"```
where 'arg' is the argument consisting of the path for the binary code to be run on the simulator, eg ```../task4/t1.bin```


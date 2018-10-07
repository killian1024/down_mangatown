### down_mangatown ###

Download manga scans using mangatown.com.
For more information use `./down_mangatown --help` command.

### Build ###

Use the folowing commands to buil the CMake project.

    cmake .
    cmake --build .
    sudo make install
    
You can adjust the previous commands keeping in mind your system.

#### Dependencies ####

In order to compile this software you have to use a C++ revision equal or highter to C++17 
(ISO/IEC 14882:2017). Also the following libraries and tools are necessary with a version equal or 
highter the specified one:
- speed (https://github.com/killian1024/speed)
- libxml2 2.9.3
- GCC 8.0.1
- CMake 3.9
- GNU Make 4.1

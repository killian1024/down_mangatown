### down_mangatown ###

down_mangatown is a simple C++ program whose propose download manga scans using mangatown.com.
For more information use `./down_mangatown --help` command.

### Build ###

Use the folowing commands to buil the CMake project.

    cmake .
    cmake --build .

#### Dependencies ####

In order to compile this software you have to use a C++ revision equal or highter to C++17 
(ISO/IEC 14882:2017). Also the following libraries are necessary:
- speed (https://github.com/killian1024/speed)
- libxml2 2.9.3

cmake_minimum_required(VERSION 3.9)

project(down_mangatown)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include_directories(/usr/include/libxml2)

set(SOURCE_FILES
        src/main.cpp
        src/chapter.hpp
        src/program.cpp
        src/program.hpp
        )

add_executable(down_mangatown ${SOURCE_FILES})
target_link_libraries(down_mangatown -lspeed -lstdc++fs -lxml2)
install(TARGETS down_mangatown DESTINATION bin)
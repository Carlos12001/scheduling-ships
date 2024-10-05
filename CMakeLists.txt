cmake_minimum_required(VERSION 3.22.0)         

# Set the project name
project (sheduling-ships)

# Define _GNU_SOURCE globally
add_definitions(-D_GNU_SOURCE)

# Establece el directorio de salida de los binarios
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Support for C 
enable_language(C)

# Define the C standard
set(CMAKE_C_STANDARD 11)

# Support CTest
include(CTest)


# Enable debugging symbols
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

# Add subdirectories
add_subdirectory(src)

# Set the project name as a variable for reuse
set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

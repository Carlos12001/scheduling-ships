cmake_minimum_required(VERSION 3.25.0)         

# Set the project name
project (sheduling-ships)

# Establece el directorio de salida de los binarios
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Support for C 
enable_language(C)

# Support CTest
include(CTest)

# Add subdirectories
add_subdirectory(src)

# Define the C standard
set(CMAKE_C_STANDARD 11)

# Set the project name as a variable for reuse
set(PROJECT_NAME ${CMAKE_PROJECT_NAME})

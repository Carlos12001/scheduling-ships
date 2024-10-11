# src/lib/CMakeLists.txt
cmake_minimum_required(VERSION 3.10.0)

# Set the project name as a variable for reuse
set(LIB_NAME cethread)

# Create a shared library from ${LIB_NAME}.c
add_library(${LIB_NAME} SHARED ${LIB_NAME}.c)

# Set properties for the library (version, SOVERSION, public header)
set_target_properties(${LIB_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ${LIB_NAME}.h)

# Install the library and public header files
install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
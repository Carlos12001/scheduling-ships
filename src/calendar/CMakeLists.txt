# src/calendar/CMakeLists.txt
cmake_minimum_required(VERSION 3.22.0)

# Set the project name as a variable for reuse
set(CALENDAR_NAME calendar)

# Create a shared library from ${CALENDAR_NAME}.c
add_library(${CALENDAR_NAME} SHARED ${CALENDAR_NAME}.c)

# Set properties for the library (version, SOVERSION, public header)
set_target_properties(${CALENDAR_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER ${CALENDAR_NAME}.h)

# Install the library and public header files
install(TARGETS ${CALENDAR_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
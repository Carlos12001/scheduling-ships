# Create a shared library from cethread.c
add_library(cethread SHARED cethread.c)

# Set properties for the library (version, SOVERSION, public header)
set_target_properties(cethread PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER cethread.h)

# Install the library and public header files
install(TARGETS cethread
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
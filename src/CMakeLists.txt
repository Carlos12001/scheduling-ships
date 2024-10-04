# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR}/src/lib)

# Add subdirectory for the library code
add_subdirectory(lib)

# Define the main executable, using $(PROJECT_NAME)
add_executable(${PROJECT_NAME} main.c)

# Link the main executable with the CEThreads library
target_link_libraries(${PROJECT_NAME} cethread)

# Add the tests
add_subdirectory(tests)

# Copy image files to the build directory (if needed)
file(COPY gui/images/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/images)
file(COPY config/ships.config DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)

# src/CMakeLists.txt

# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR}/src/lib)
include_directories(${CMAKE_SOURCE_DIR}/src/canal)

# Add subdirectory for the library code
add_subdirectory(lib)

# Add subdirectory for the calendar
add_subdirectory(calendar)

# Define the main executable, using $(PROJECT_NAME)
add_executable(${PROJECT_NAME} main.c canal/canal.c)

# Link the main executable with the CEThreads library
target_link_libraries(${PROJECT_NAME} cethread calendar)

# Add the tests
add_subdirectory(tests)

# Copy image files to the build directory (if needed)
file(COPY gui/gui.py DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY hardware/hardware.py DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY canal/canal.config DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canal)
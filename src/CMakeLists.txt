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

# Custom commands to copy files if they are modified
add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gui.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/gui/gui.py ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gui.py
    DEPENDS ${CMAKE_SOURCE_DIR}/src/gui/gui.py
    COMMENT "Copying gui.py to the runtime output directory"
)

add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hardware.py
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/hardware/hardware.py ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hardware.py
    DEPENDS ${CMAKE_SOURCE_DIR}/src/hardware/hardware.py
    COMMENT "Copying hardware.py to the runtime output directory"
)

add_custom_command(
    OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canal/canal.config
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/src/canal/canal.config ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canal/canal.config
    DEPENDS ${CMAKE_SOURCE_DIR}/src/canal/canal.config
    COMMENT "Copying canal.config to the runtime output directory"
)

# Ensure the copying happens as part of the build process
add_custom_target(copy_files ALL
    DEPENDS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gui.py
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/hardware.py
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/canal/canal.config
)

# Add the copy_files target to the main executable build dependencies
add_dependencies(${PROJECT_NAME} copy_files)
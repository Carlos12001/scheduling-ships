# src/tests/CMakeLists.txt


# Set a separate directory for the test binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS ${CMAKE_BINARY_DIR}/tests/bin)


# ------- CETHREAD TESTS -------

# Add the cethread_create test
add_executable(cethread_create cethread_create.c)
set_target_properties(cethread_create PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_create cethread)

# Add the cethread_join test
add_executable(cethread_join cethread_join.c)
set_target_properties(cethread_join PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_join cethread)

# Add the cethread_end test
add_executable(cethread_end cethread_end.c)
set_target_properties(cethread_end PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_end cethread)


## CEMUX TESTS

add_test(NAME cethread_create_test COMMAND cethread_create)
add_test(NAME cethread_join_test COMMAND cethread_join)
add_test(NAME cethread_end_test COMMAND cethread_end)
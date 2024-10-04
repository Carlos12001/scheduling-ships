# Set a separate directory for the test binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS ${CMAKE_BINARY_DIR}/tests/bin)

# ------- PTHREAD TESTS -------

# Add the pthread_create test
add_executable(pthread_create pthread_create.c)
set_target_properties(pthread_create PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(pthread_create pthread)

# Add the pthread_join test
add_executable(pthread_join pthread_join.c)
set_target_properties(pthread_join PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(pthread_join pthread)

## MUX PTHREAD TESTS

# Register the tests with CTest
add_test(NAME pthread_create_test COMMAND pthread_create)
add_test(NAME pthread_join_test COMMAND pthread_join)

# ------- CETHREAD TESTS -------

# Add the cethread_create test
add_executable(cethread_create cethread_create.c)
set_target_properties(cethread_create PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_create cethread)

# Add the cethread_join test
add_executable(cethread_join cethread_join.c)
set_target_properties(cethread_join PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_join cethread)


## CEMUX TESTS

add_test(NAME cethread_create_test COMMAND cethread_create)
add_test(NAME cethread_join_test COMMAND cethread_join)
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

# Add the cemutex_init
add_executable(cemutex_init cemutex_init.c)
set_target_properties(cemutex_init PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cemutex_init cethread)

# Add the cemutex_destroy
add_executable(cemutex_destroy cemutex_destroy.c)
set_target_properties(cemutex_destroy PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cemutex_destroy cethread)

# Add the cemutex_unlock
add_executable(cemutex_unlock cemutex_unlock.c)
set_target_properties(cemutex_unlock PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cemutex_unlock cethread)

# Add the test library
add_executable(test_lib test_lib.c)
set_target_properties(test_lib PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(test_lib cethread)

add_test(NAME cethread_create_test COMMAND cethread_create)
add_test(NAME cethread_join_test COMMAND cethread_join)
add_test(NAME cethread_end_test COMMAND cethread_end)
add_test(NAME cemutex_init_test COMMAND cemutex_init)
add_test(NAME cemutex_destroy_test COMMAND cemutex_destroy)
add_test(NAME cemutex_unlock_test COMMAND cemutex_unlock)
add_test(NAME test_lib_test COMMAND test_lib)


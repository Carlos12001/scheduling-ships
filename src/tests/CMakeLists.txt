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

# Add the pthread_mutex_init test
add_executable(pthread_mutex_init pthread_mutex_init.c)
set_target_properties(pthread_mutex_init PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(pthread_mutex_init pthread)

# Add the pthread_mutex_destroy test
add_executable(pthread_mutex_destroy pthread_mutex_destroy.c)
set_target_properties(pthread_mutex_destroy PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(pthread_mutex_destroy pthread)

# Add the pthread_mutex_unlock
add_executable(pthread_mutex_unlock pthread_mutex_unlock.c)
set_target_properties(pthread_mutex_unlock PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(pthread_mutex_unlock pthread)


# Register the tests with CTest
add_test(NAME pthread_create_test COMMAND pthread_create)
add_test(NAME pthread_join_test COMMAND pthread_join)
add_test(NAME pthread_mutex_init_test COMMAND pthread_mutex_init)
add_test(NAME pthread_mutex_destroy_test COMMAND pthread_mutex_destroy)
add_test(NAME pthread_mutex_unlock_test COMMAND pthread_mutex_unlock)





# ------- CETHREAD TESTS -------

# Add the cethread_create test
add_executable(cethread_create cethread_create.c)
set_target_properties(cethread_create PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_create cethread)

# Add the cethread_join test
add_executable(cethread_join cethread_join.c)
set_target_properties(cethread_join PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_TESTS})
target_link_libraries(cethread_join cethread)

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


add_test(NAME cethread_create_test COMMAND cethread_create)
add_test(NAME cethread_join_test COMMAND cethread_join)
add_test(NAME cemutex_init_test COMMAND cemutex_init)
add_test(NAME cemutex_destroy_test COMMAND cemutex_destroy)
add_test(NAME cemutex_unlock_test COMMAND cemutex_unlock)

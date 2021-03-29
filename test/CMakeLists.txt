cmake_minimum_required(VERSION 3.5.1)

include_directories("../rtdb2")

# let's put the binaries in the same directory to make it easier to play with, compare with files, run the scripts, ...
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# binaries
add_executable(demo_basics demo_basics.cpp)
target_link_libraries(demo_basics rtdb2)
add_executable(demo_multi_client demo_multi_client.cpp)
target_link_libraries(demo_multi_client rtdb2)

add_executable(test_configuration test_configuration.cpp)
target_link_libraries(test_configuration rtdb2)

# tests
add_test(test_basics ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh demo_basics)
#add_test(test_multi_client ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh demo_multi_client) # broken!
add_test(test_python_tools ${CMAKE_CURRENT_LIST_DIR}/test_python_tools.sh)
add_test(test_configuration ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh test_configuration
    config/minimal_configuration.xml
    config/extensive_configuration.xml
)


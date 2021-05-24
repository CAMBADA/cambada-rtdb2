cmake_minimum_required(VERSION 3.5.1)

# let's put the binaries in the same directory to make it easier to play with, compare with files, run the scripts, ...
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# binaries
add_executable(demo_basics demo_basics.cpp)
target_link_libraries(demo_basics RtDB::rtdb)

add_executable(demo_multi_client demo_multi_client.cpp)
target_link_libraries(demo_multi_client RtDB::rtdb)

add_executable(test_clients test_clients.cpp)
target_link_libraries(test_clients RtDB::comm RtDB::rtdb)

add_executable(test_configuration test_configuration.cpp)
target_link_libraries(test_configuration RtDB::rtdb)

add_executable(test_context test_context.cpp)
target_link_libraries(test_context RtDB::rtdb)

add_executable(test_monitor test_monitor.cpp)
target_link_libraries(test_monitor RtDB::rtdb)

# tests
add_test(test_basics ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh demo_basics)
#add_test(test_multi_client ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh demo_multi_client) # broken!
add_test(test_python_tools ${CMAKE_CURRENT_LIST_DIR}/test_python_tools.sh)
add_test(test_configuration ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh test_configuration
    config/minimal_configuration.xml
    config/extensive_configuration.xml
    config/minimal_configuration_v2.xml
    config/extensive_configuration_v2.xml
)
add_test(test_context ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh test_context)
add_test(test_monitor ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh test_monitor)
#add_test(test_clients ${CMAKE_CURRENT_LIST_DIR}/test_runner.sh test_clients) #disabled due to variable output


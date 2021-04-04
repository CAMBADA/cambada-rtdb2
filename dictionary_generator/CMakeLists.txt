### Compiling
find_package(Boost COMPONENTS program_options REQUIRED QUIET)

add_executable(rtdb2_dictionary_gen generator.cpp)
target_link_libraries(rtdb2_dictionary_gen
        RtDB::rtdb
        ${Boost_PROGRAM_OPTIONS_LIBRARY})
#add_dependencies(rtdb2_dictionary_gen lmdb msgpack)
set_target_properties(rtdb2_dictionary_gen PROPERTIES COMPILE_FLAGS "-fPIC")

set_target_properties(rtdb2_dictionary_gen
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rtdb2_tools"
)

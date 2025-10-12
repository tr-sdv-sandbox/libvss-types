add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example PRIVATE vss::types)

add_executable(struct_example struct_example.cpp)
target_link_libraries(struct_example PRIVATE vss::types)

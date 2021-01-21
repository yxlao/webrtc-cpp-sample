add_executable(glapp glapp.cpp)
set_global_target_properties(glapp)
target_link_libraries(glapp PRIVATE GL GLU glut)

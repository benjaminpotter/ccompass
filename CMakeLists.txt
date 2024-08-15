cmake_minimum_required(VERSION 3.16)

project(ccompass
    VERSION 0.3
    DESCRIPTION "An overwhelmingly simple, header-only, C library for working with sky polarization images."
    LANGUAGES C)


add_executable(t_compute_stokes tests/test_compute_stokes.c)
target_link_libraries(t_compute_stokes m)
target_include_directories(t_compute_stokes PRIVATE include)

add_executable(t_compute_aolp tests/test_compute_aolp.c)
target_link_libraries(t_compute_aolp m)
target_include_directories(t_compute_aolp PRIVATE include)

add_executable(t_transform_stokes tests/test_transform_stokes.c)
target_link_libraries(t_transform_stokes m)
target_include_directories(t_transform_stokes PRIVATE include)


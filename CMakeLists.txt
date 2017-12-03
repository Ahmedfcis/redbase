cmake_minimum_required(VERSION 3.0.0)
project(redbase VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 11)

set(PF_SOURCES src/pf_buffermgr.cc src/pf_error.cc src/pf_filehandle.cc src/pf_hashtable.cc
src/pf_manager.cc src/pf_pagehandle.cc src/pf_statistics.cc src/statistics.cc)
include_directories(include)

add_library(PF ${PF_SOURCES})
add_executable(pf_test1 src/pf_test1.cc)
add_executable(pf_test2 src/pf_test2.cc)
add_executable(pf_test3 src/pf_test3.cc)
target_link_libraries(pf_test1 PF)
target_link_libraries(pf_test2 PF)
target_link_libraries(pf_test3 PF)


include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

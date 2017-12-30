cmake_minimum_required(VERSION 3.0.0)
project(redbase VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 11)
add_compile_options(-DPF_STATS)
set(PF_SOURCES src/pf_buffermgr.cc src/pf_error.cc src/pf_filehandle.cc src/pf_hashtable.cc
src/pf_manager.cc src/pf_pagehandle.cc src/pf_statistics.cc src/statistics.cc)

set(RM_SOURCES src/rm_manager.cc src/rm_filehandle.cc src/rm_filescan.cc src/rm_record.cc src/rm_rid.cc src/rm_error.cc)

set(TEST_SOURCES test/test_main.cc test/test_rm.cc)

include_directories(include)

add_library(PF ${PF_SOURCES})
add_library(RM ${RM_SOURCES})

add_executable(pf_test1 src/pf_test1.cc)
add_executable(pf_test2 src/pf_test2.cc)
add_executable(pf_test3 src/pf_test3.cc)
add_executable(rm_test  src/rm_test.cc)
add_executable(redbase_test  ${TEST_SOURCES})

target_link_libraries(pf_test1 PF)
target_link_libraries(pf_test2 PF)
target_link_libraries(pf_test3 PF)
target_link_libraries(rm_test RM PF)
target_link_libraries(redbase_test RM PF)

include(CTest)
enable_testing()

add_test(NAME 1000_RECORDS_WRITE_READ COMMAND redbase_test 1000_RECORDS_WRITE_READ)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

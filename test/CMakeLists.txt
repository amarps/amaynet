add_executable(test_test test_test.cxx)
target_include_directories(test_test PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(test_test amaynet gtest_main gmock_main ssl crypto)

add_executable(common_test common_test.cxx)
target_include_directories(common_test PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(common_test amaynet gtest_main gmock_main ssl crypto)

add_executable(TCP_test TCP_test.cxx)
target_include_directories(TCP_test PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(TCP_test amaynet gtest_main gmock_main ssl crypto)

add_executable(HTTP_test HTTP_test.cxx)
target_include_directories(HTTP_test PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(HTTP_test amaynet gtest_main gmock_main ssl crypto)

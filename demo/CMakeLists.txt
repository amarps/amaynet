add_executable(download_file download_file.cxx)
target_link_libraries(download_file PUBLIC amaynet ssl crypto)
target_include_directories(download_file PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(httpserver httpserver.cxx)
target_link_libraries(httpserver PUBLIC amaynet ssl crypto)
target_include_directories(httpserver PUBLIC "${PROJECT_SOURCE_DIR}/include")

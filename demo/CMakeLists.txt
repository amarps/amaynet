add_executable(download_file download_file.cxx)
target_link_libraries(download_file PUBLIC ${PROJECT_LIBRARIES})
target_include_directories(download_file PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(download_file download_file.cxx)
target_link_libraries(download_file PUBLIC amaynet ssl crypto)
target_include_directories(download_file PUBLIC "${PROJECT_SOURCE_DIR}/include")

add_executable(httpserver httpserver.cxx)
target_link_libraries(httpserver PUBLIC amaynet ssl crypto)
target_include_directories(httpserver PUBLIC "${PROJECT_SOURCE_DIR}/include")
file(COPY public DESTINATION .)

add_executable(chat_client chat_client.cxx)
target_link_libraries(chat_client PUBLIC amaynet ssl crypto pthread)
target_include_directories(chat_client PUBLIC "${PROJECT_SOURCE_DIR}/include")
file(COPY public DESTINATION .)

add_executable(chat_server chat_server.cxx)
target_link_libraries(chat_server PUBLIC amaynet ssl crypto)
target_include_directories(chat_server PUBLIC "${PROJECT_SOURCE_DIR}/include")
file(COPY public DESTINATION .)

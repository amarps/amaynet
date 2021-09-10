add_library(TCP TCPClient.cxx TCPListener.cxx Connection.cxx TCP.cxx)
target_include_directories(TCP PUBLIC "${PROJECT_SOURCE_DIR}/include")

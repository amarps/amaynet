cmake_minimum_required(VERSION 3.19.2)

# set the project name
project(AMAYHTTPS)
SET ( AMAYHTTPS_VERSION_MAJOR 0 )
SET ( AMAYHTTPS_VERSION_MINOR 1 )
SET ( AMAYHTTPS_VERSION_PATCH 0 )
SET ( AMAYHTTPS_VERSION ${AMAYHTTPS_VERSION_MAJOR}.${AMAYHTTPS_VERSION_MINOR}.${AMAYHTTPS_VERSION_PATCH} )
SET ( PROJECT_VERSION "${AMAYHTTPS_VERSION}" )

# specify the c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# add TCPLIB to the current project
target_link_libraries(${PROJECT_NAME} PUBLIC HTTP TCP)

# add the execuctable
add_executable(${PROJECT_NAME} src/main.cxx)

target_link_libraries(${PROJECT_NAME} PUBLIC TCP)
target_link_libraries(${PROJECT_NAME} PUBLIC HTTP)

# add configuration file for the project
# this will generate AMAYHTTPSConfig.h at generated dir
configure_file(AmayHTTPSConfig.h.in AmayHTTPSConfig.h)

# add generated project dir to include dir
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/src")


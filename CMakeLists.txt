cmake_minimum_required(VERSION 3.10)

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

# add the execuctable
add_executable(${PROJECT_NAME} src/main.cxx)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/TCP)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/HTTP)

# add warning level if gcc compiler
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJECT_NAME} PUBLIC
    -Wall
    -Wcast-align
    -Werror
    -Wextra
    -Wformat-nonliteral
    -Wformat=2
    -Winvalid-pch
    -Wmissing-declarations
    -Wmissing-format-attribute
    -Wmissing-include-dirs
    -Wredundant-decls
    -Wredundant-decls
    -Wswitch-default
    -Wswitch-enum
    )
endif()

# add TCPLIB to the current project
target_link_libraries(${PROJECT_NAME} PUBLIC HTTP TCP)

# add configuration file for the project
# this will generate AMAYHTTPSConfig.h at generated dir
configure_file(AmayHTTPSConfig.h.in AmayHTTPSConfig.h)

# add generated project dir to include dir
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/src")


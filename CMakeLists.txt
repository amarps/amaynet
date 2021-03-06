cmake_minimum_required(VERSION 3.10)

# set the project name
project(amaynet)
SET ( AMAYNET_VERSION_MAJOR 0 )
SET ( AMAYNET_VERSION_MINOR 1 )
SET ( AMAYNET_VERSION_PATCH 0 )
SET ( AMAYNET_VERSION ${AMAYNET_VERSION_MAJOR}.${AMAYNET_VERSION_MINOR}.${AMAYNET_VERSION_PATCH} )
SET ( PROJECT_VERSION "${AMAYNET_VERSION}" )

# specify the c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# add sources to the current project
aux_source_directory(${PROJECT_SOURCE_DIR}/src/ SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/TCP TCP_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/HTTP HTTP_SRC)
set (SOURCES
  ${SRC}
  ${TCP_SRC}
  ${HTTP_SRC})

add_library(${PROJECT_NAME} SHARED ${SOURCES})

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

# add configuration file for the project
# this will generate AMAYNETConfig.h at generated dir
configure_file(AmayNetConfig.h.in ${PROJECT_SOURCE_DIR}/include/AmayNetConfig.hxx)

# add generated project dir to include dir
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")

# configure installation
install(TARGETS amaynet DESTINATION lib)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" DESTINATION include/amaynet)

# ----------------------------- doxygen configuration --------------------------------
# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

if (BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile)
    
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
    
    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif(BUILD_DOC)

# ================== setup google test =======================
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/googletest)
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

# ========================== setup demo ================================
add_subdirectory(${PROJECT_SOURCE_DIR}/demo)


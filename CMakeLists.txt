cmake_minimum_required(VERSION 3.24)
project(LspScript)

set(CMAKE_CXX_STANDARD 17)
set(LSPI_HEADERS_DIR "./include")
set(LSPI_SRC_DIR "./src")
file(GLOB LSPI_HEADERS "${LSPI_HEADERS_DIR}/*")
file(GLOB LSPI_SRC "${LSPI_SRC_DIR}/*")
file(GLOB SPMX_HEADERS "./external/SpmX/include")

add_compile_definitions(SIZEOF_PTR=${CMAKE_SIZEOF_VOID_P})

# message("Finding SpmX")
# find_package(SpmX CONFIG)
# if(MX3d_FOUND)
#    message("SpmX found at ${SpmX_DIR}")
# else()
#    message(FATAL_ERROR "SpmX not found. Use \"git submodule --init --update\" to clone the missing submodules.")
# endif()

include_directories("./external/SpmX/include")
include_directories(${LSPI_HEADERS_DIR})
include("./external/SpmX/CMakeLists.txt")
add_executable(lspi ${LSPI_SRC} ${LSPI_HEADERS} ${SPMX_HEADERS})
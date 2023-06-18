cmake_minimum_required(VERSION 3.24)
project(LspScript)

set(CMAKE_CXX_STANDARD 17)
set(LSPI_HEADERS_DIR "./include")
set(LSPI_SRC_DIR "./src")
file(GLOB LSPI_HEADERS "${LSPI_HEADERS_DIR}/*")
file(GLOB LSPI_SRC "${LSPI_SRC_DIR}/*")

add_compile_definitions(SIZEOF_PTR=${CMAKE_SIZEOF_VOID_P})

include_directories(${LSPI_HEADERS_DIR})
add_executable(lspi ${LSPI_SRC} ${LSPI_HEADERS})

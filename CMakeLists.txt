cmake_minimum_required(VERSION 3.10)
project(Server)

set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(PRIME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Prime)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SERVER_DIR}/include
    ${PRIME_DIR}/include
)

add_executable(client ${SERVER_DIR}/client_win.cxx)
add_executable(server ${SERVER_DIR}/server_win.cxx)
add_executable(prime ${PRIME_DIR}/prime.cxx)
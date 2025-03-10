cmake_minimum_required(VERSION 3.10)
project(Server)

set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SERVER_DIR}/include
)

add_executable(client ${SERVER_DIR}/client.cxx)
add_executable(server ${SERVER_DIR}/server.cxx)
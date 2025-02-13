cmake_minimum_required(VERSION 3.10)
project(protoClientServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Protobuf REQUIRED)
find_package(absl REQUIRED)

#
# Protocol Buffers setup
#
set(PROTO_FILES ${CMAKE_SOURCE_DIR}/proto/protocol.proto)
set(GENERATED_PROTO_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    ${PROTO_FILES}
)

get_filename_component(PROTO_HEADER_DIR ${PROTO_HDRS} DIRECTORY)

#
# Include directories
#
include_directories(
    ${CMAKE_SOURCE_DIR}/include      
    ${PROTO_HEADER_DIR}              
    ${Protobuf_INCLUDE_DIRS}
    ${absl_INCLUDE_DIRS}
)

#
# Create the server executable
#
add_executable(server
    src/server.cpp                         
    include/uart.cpp
    include/uartReadLoop.cpp
    include/initSystem.cpp
    include/messageIO.cpp      
    ${PROTO_SRCS}                    
)

#
# Link libraries
#
target_link_libraries(server PRIVATE
    ${Protobuf_LIBRARIES}
    absl::log
    absl::log_internal_message
    absl::log_internal_check_op
)

#
# Client executable
# 
add_executable(client
    src/client.cpp
    include/uart.cpp
    include/uartReadLoop.cpp
    include/initSystem.cpp
    include/messageIO.cpp      
    ${PROTO_SRCS}
)

target_link_libraries(client PRIVATE
    ${Protobuf_LIBRARIES}
    absl::log
    absl::log_internal_message
    absl::log_internal_check_op
)

#
# Set output directory 
#
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)



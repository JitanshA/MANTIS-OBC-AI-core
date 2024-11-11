cmake_minimum_required(VERSION 3.10)
project(obc_ai_core)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(protobuf CONFIG REQUIRED)

add_library(proto-objects OBJECT "${CMAKE_CURRENT_LIST_DIR}/src/proto/protocol.proto")

target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/src/proto/generated")

target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

protobuf_generate(
    TARGET proto-objects
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/proto"
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)
set(CORE_SOURCES
    src/core/command_handling.cpp
)

set(NETWORK_SOURCES
    src/network/ethernet.cpp
    src/network/uart.cpp
)
set(UTILS_SOURCES
    src/utils/utils.cpp
)

add_executable(obc_ai src/obc_ai.cpp ${NETWORK_SOURCES} ${UTILS_SOURCES} ${CORE_SOURCES})
target_link_libraries(obc_ai 
    PRIVATE 
    proto-objects 
    protobuf::libprotobuf
)
target_include_directories(obc_ai 
    PRIVATE 
    ${PROTO_BINARY_DIR}
    src/network
    src/utils
    src/core
)

add_executable(obc_micro src/obc_micro.cpp ${NETWORK_SOURCES} ${UTILS_SOURCES})
target_link_libraries(obc_micro 
    PRIVATE 
    proto-objects 
    protobuf::libprotobuf
)
target_include_directories(obc_micro 
    PRIVATE 
    ${PROTO_BINARY_DIR}
    src/network
    src/utils
)

add_executable(payload src/payload.cpp ${NETWORK_SOURCES} ${UTILS_SOURCES})
target_link_libraries(payload 
    PRIVATE 
    proto-objects 
    protobuf::libprotobuf
)
target_include_directories(payload 
    PRIVATE 
    ${PROTO_BINARY_DIR}
    src/network
    src/utils
)

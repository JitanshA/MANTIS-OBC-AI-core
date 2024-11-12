cmake_minimum_required(VERSION 3.10)
project(obc_ai_core)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Protobuf CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Protobuf setup
set(PROTO_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/proto/generated")
add_library(proto_objects OBJECT "${CMAKE_CURRENT_LIST_DIR}/proto/protocol.proto")
target_include_directories(proto_objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")
target_link_libraries(proto_objects PUBLIC protobuf::libprotobuf)

protobuf_generate(
    TARGET proto_objects
    IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto"
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

set(CORE_SOURCES
    src/core/uart_async_receiver.cpp
    src/core/command_dispatcher.cpp
    src/core/event_loop.cpp
)

set(COMMAND_SOURCES
    src/commands/command_handlers.cpp
)

set(UTILS_SOURCES
    src/utils/logger.cpp
)

# Executable for obc_ai
add_executable(obc_ai src/apps/obc_ai.cpp ${CORE_SOURCES} ${COMMAND_SOURCES} ${UTILS_SOURCES})
target_link_libraries(obc_ai
    PRIVATE
    proto_objects
    protobuf::libprotobuf
    Boost::system
)
target_include_directories(obc_ai
    PRIVATE
    ${PROTO_BINARY_DIR}
    include/core
    include/utils
    include/commands
)

# Executable for obc_micro (testing application)
add_executable(obc_micro src/apps/obc_micro.cpp ${CORE_SOURCES} ${UTILS_SOURCES})
target_link_libraries(obc_micro
    PRIVATE
    proto_objects
    protobuf::libprotobuf
)
target_include_directories(obc_micro
    PRIVATE
    ${PROTO_BINARY_DIR}
    include/core
    include/utils
    include/commands
)

# Executable for payload (testing application)
add_executable(payload src/apps/payload.cpp ${CORE_SOURCES} ${UTILS_SOURCES})
target_link_libraries(payload
    PRIVATE
    proto_objects
    protobuf::libprotobuf
)
target_include_directories(payload
    PRIVATE
    ${PROTO_BINARY_DIR}
    include/core
    include/utils
    include/commands
)
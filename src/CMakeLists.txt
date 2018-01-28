cmake_minimum_required(VERSION 2.8.12)
project(examples)

# add dlib
add_subdirectory(dlib dlib_build)

# add opencv
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# https://github.com/grpc/grpc/blob/v1.8.x/examples/cpp/route_guide/Makefile
list(APPEND CMAKE_PREFIX_PATH "/opt/grpc" "/opt/protobuf")

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/protos/state.proto)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

add_executable(
    train_object_detector
    train_object_detector.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)
target_link_libraries(
    train_object_detector
    dlib::dlib
    ${OpenCV_LIBS}
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)

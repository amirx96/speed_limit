cmake_minimum_required(VERSION 2.8.12)
project(examples)

# add dlib
add_subdirectory(dlib dlib_build)

# add opencv
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

add_executable(train_object_detector train_object_detector.cpp)
target_link_libraries(train_object_detector dlib::dlib ${OpenCV_LIBS})

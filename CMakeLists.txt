cmake_minimum_required(VERSION 3.22)
project(genicam_processing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#find_package(OpenCV REQUIRED)
#set(OpenCV_DIR "~/Downloads/opencv-4.5.5")
set(mvIMPACT_Acquire_DIR "/opt/mvIMPACT_Acquire")
#set(OpenCV_DIR "C:\\Users\\r97sm\\Downloads\\Programs\\opencv\\build")
#set(mvIMPACT_Acquire_DIR "C:\\Program Files\\MATRIX VISION\\mvIMPACT Acquire")

#include(${OpenCV_DIR}/OpenCVConfig.cmake)
include(${mvIMPACT_Acquire_DIR}/mvIMPACT_AcquireConfig.cmake)

#include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${mvIMPACT_Acquire_INCLUDE_DIRS})

add_executable(main main.cpp utils.cpp)

target_link_libraries(main ${mvIMPACT_Acquire_LIBRARIES})
#target_link_libraries(main ${OpenCV_LIBS})

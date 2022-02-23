cmake_minimum_required(VERSION 3.11)
project(genicam_processing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fopenmp")
set(CVB_DIR "../../cvb")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CVB_DIR}/cmake")

include_directories(Include)

find_package(CVB REQUIRED)

add_executable(main
	main.cpp
	cameraconfig.cpp
)

target_link_libraries(main
	CVB::CVCDriver
	CVB::CVCUtilities
	CVB::CVGenApi
)

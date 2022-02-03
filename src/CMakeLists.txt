cmake_minimum_required(VERSION 3.11)
project(genicam_processing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(OpenMP_DIR "/usr/share/cmake/Modules/")
set(CVB_DIR "/opt/cvb")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${OpenMP_DIR}/cmake" "${CVB_DIR}/cmake")

include_directories(Include /usr/include)

find_package(CVB REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(main
	main.cpp
	cameraconfig.cpp
)

target_link_libraries(main
	OpenMP_CXX_LIBRARY
	CVB::CVCDriver
	CVB::CVCUtilities
	CVB::CVGenApi
)

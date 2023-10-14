cmake_minimum_required(VERSION 3.23)
project(forestFire)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -fopenmp")

add_executable(${PROJECT_NAME} src/main.cpp src/program_options.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)

INCLUDE(FindPkgConfig)

find_package(OpenMP QUIET)

if (OpenMP_CXX_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX_INCLUDE_DIRS)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif ()

set(SDL2_LIB "/Library/Frameworks/SDL2.framework/SDL2")
target_include_directories(${PROJECT_NAME} PRIVATE "/Library/Frameworks/SDL2.framework/Headers")

target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIB})

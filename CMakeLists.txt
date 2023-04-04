cmake_minimum_required(VERSION 3.0)
project(server_repka)
set(CMAKE_CXX_STANDARD 17)
file(GLOB
            CPPS "*.cpp")
add_executable(${PROJECT_NAME} ${CPPS})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} pthread)
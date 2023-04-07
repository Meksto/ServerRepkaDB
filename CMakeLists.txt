cmake_minimum_required(VERSION 3.0)
project(server_repka)
set(CMAKE_CXX_STANDARD 20)
file(GLOB
            CPPS "*.cpp")

set(SPDLOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
set(SPDLOG_VERSION v1.9.2) # Версия библиотеки spdlog

# Загружаем библиотеку spdlog с использованием FetchContent
include(FetchContent)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.9.2 # Измените на нужную версию
)
FetchContent_MakeAvailable(spdlog)


add_executable(${PROJECT_NAME} ${CPPS})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} pthread spdlog)
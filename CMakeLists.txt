cmake_minimum_required(VERSION 3.0.0)
project(server_repka VERSION 0.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
include(CTest)
enable_testing()

file(GLOB
                CPPS "*.cpp"
                HEADH "*.h")

include(FetchContent)


set(SPDLOG_DIR ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
set(SPDLOG_VERSION v1.9.2) # Версия библиотеки spdlog
set(INCLUDES_DIR ${CMAKE_SOURCE_DIR}/include)

# Загружаем библиотеку spdlog с использованием FetchContent
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/spdlog
  GIT_TAG v1.9.2 # Измените на нужную версию
)
FetchContent_MakeAvailable(spdlog)


FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/google/googletest
  GIT_TAG main
)

# Получение и сборка библиотек Google Mock
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# Включение заголовочных файлов gtest и gmock
include_directories(${CMAKE_SOURCE_DIR}/google/googletest/googlemock/include)
include_directories(${CMAKE_SOURCE_DIR}/google/googletest/googletest/include)


add_subdirectory(test)

include_directories(${INCLUDES_DIR})
add_executable(server_repka ${CPPS}) #EXECUTABLE


#find_package(spdlog CONFIG)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES_DIR})
target_link_libraries(${PROJECT_NAME} pthread spdlog::spdlog gtest gmock)
include(CPack)

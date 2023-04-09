cmake_minimum_required(VERSION 3.0.0)
project(server_tests)

file(GLOB
            CPPS "*.cpp"
            "../IRepka.cpp"
)

include_directories(${CMAKE_SOURCE_DIR}/google/googletest/googlemock/include)
include_directories(${CMAKE_SOURCE_DIR}/google/googletest/googletest/include)
include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(${PROJECT_NAME} ${CPPS})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC
                        pthread
                        gtest_main
                        gtest
                        gmock
)
add_test (
    NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME}
)

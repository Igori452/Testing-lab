cmake_minimum_required(VERSION 3.20)
project(GherkinCatchModule)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libraries/gherkin-parser/)
add_subdirectory(libraries/Catch2/)

add_library(gherkin_catch_module STATIC
    src/gherkin-catch-module.cpp
    #src/step_registry.cpp
)

set(APP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../app)

target_include_directories(gherkin_catch_module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/gherkin-parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/gherkin-parser/libs/gherkin-c/include
        ${APP_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/libraries/gherkin-parser/libs/gherkin-c/include
)

target_link_libraries(gherkin_catch_module
    PUBLIC
        Catch2::Catch2WithMain
    PRIVATE
        gherkin-cpp
        gherkin
)

add_executable(gherkin_tests
    tests/jarvis_hull.cpp
    tests/context/context.cpp
    #tests/graham_hull.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(gherkin_tests
    PRIVATE
        gherkin_catch_module
)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/features/ 
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/features/)
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

# Jarvis tests
add_executable(jarvis_tests
    tests/context/context.cpp
    tests/jarvis_hull.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(jarvis_tests PRIVATE gherkin_catch_module)

# Graham tests
add_executable(graham_tests
    tests/context/context.cpp
    tests/graham_hull.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(graham_tests PRIVATE gherkin_catch_module)

# Visualisation tests
add_executable(visualisation_tests
    tests/context/context.cpp
    tests/visualisation.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(visualisation_tests PRIVATE gherkin_catch_module)

# Error handling tests
add_executable(error_tests
    tests/context/context.cpp
    tests/error_handling.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(error_tests PRIVATE gherkin_catch_module)

# performance tests
add_executable(performance_tests
    tests/context/context.cpp
    tests/performance_comparison.cpp
    ${APP_SOURCE_DIR}/ConvexHull.cpp
)

target_link_libraries(performance_tests PRIVATE gherkin_catch_module)

# Копируем features для всех тестов
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/features/ 
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/features/)
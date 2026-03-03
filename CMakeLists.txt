cmake_minimum_required(VERSION 3.16)

if(UNIX AND NOT APPLE)
    set(CMAKE_C_COMPILER clang-15)
    set(CMAKE_CXX_COMPILER clang++-15)
endif()

project(ConvexHull)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(BUILD_APP "Build main application" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_MULL "Build with Mull mutation testing" OFF)
option(BUILD_BDD "Build BDD tests with Cucumber-Cpp" ON) 


if(NOT UNIX)
    set(BUILD_MULL OFF)
endif()

if(BUILD_MULL)
    set(BUILD_APP OFF)
    set(BUILD_TESTS OFF)
    set(BUILD_BDD OFF)

    set(MULL_PLUGIN "/usr/lib/mull-ir-frontend-15")
    
    if(NOT EXISTS "${MULL_PLUGIN}")
        message(WARNING "Mull plugin not found at ${MULL_PLUGIN}")
    endif()

    add_subdirectory(libraries/googletest)
    enable_testing()

    add_library(convex_hull_lib STATIC app/ConvexHull.cpp)

    # Только библиотека компилируется с Mull плагином
    target_compile_options(convex_hull_lib PRIVATE
        $<$<BOOL:${MULL_PLUGIN}>:-fpass-plugin=${MULL_PLUGIN}>
        -g
        -grecord-command-line
        -O0
        -fno-inline
    )

    set(TEST_SOURCES
        tests/test_main.cpp
        tests/test_comprassion.cpp
        tests/test_graham.cpp
        tests/test_jarvis.cpp
        tests/test_common.cpp
        tests/test_mocks.cpp
    )

    add_executable(ConvexHull_mull ${TEST_SOURCES})

    target_include_directories(ConvexHull_mull PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/app
    )

    target_compile_options(ConvexHull_mull PRIVATE
        -g
        -O0
        -fno-inline
    )

    target_link_libraries(ConvexHull_mull
        convex_hull_lib
        gtest_main
        gmock
        pthread
    )
endif()

if(NOT TARGET convex_hull_lib)
    add_library(convex_hull_lib STATIC app/ConvexHull.cpp)
endif()

if(BUILD_APP)
    add_subdirectory(libraries/sfml)

    add_executable(ConvexHull_app
        app/main.cpp
        app/display.cpp
    )

    target_include_directories(ConvexHull_app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/app
    )

    target_link_libraries(ConvexHull_app
        convex_hull_lib
        sfml-graphics
        sfml-window
        sfml-system
    )
endif()

if(BUILD_TESTS AND NOT BUILD_MULL)
    if(NOT TARGET gtest_main)
        add_subdirectory(libraries/googletest)
    endif()
    
    enable_testing()

    set(TEST_SOURCES
        tests/test_main.cpp
        tests/test_comprassion.cpp
        tests/test_graham.cpp
        tests/test_jarvis.cpp
        tests/test_common.cpp
        tests/test_mocks.cpp
    )

    add_executable(ConvexHull_tests ${TEST_SOURCES})

    target_include_directories(ConvexHull_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/app
    )

    target_link_libraries(ConvexHull_tests
        convex_hull_lib
        gtest_main
        gmock
    )
    
    if(UNIX)
        target_link_libraries(ConvexHull_tests pthread)
    endif()

    add_test(NAME ConvexHull_tests COMMAND ConvexHull_tests)
endif()

if(BUILD_BDD AND NOT BUILD_MULL)

    add_subdirectory(libraries/Catch2)

    find_package(Boost REQUIRED COMPONENTS regex system)

    set(GHERKIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gherkin_tests")

    set(STEP_FILES
        jarvis_hull
        graham_hull
        error_handling
        visualization
        performance_comparison
    )
    
    set(BDD_STEP_SOURCES
        ${GHERKIN_DIR}/features/step_definitions/jarvis_hull.cpp
        #${GHERKIN_DIR}/features/step_definitions/graham_hull.cpp
        #${GHERKIN_DIR}/features/step_definitions/error_handling.cpp
        #${GHERKIN_DIR}/features/step_definitions/visualization.cpp
        #${GHERKIN_DIR}/features/step_definitions/performance_comparison.cpp
    )
    
    add_executable(ConvexHull_bdd ${BDD_STEP_SOURCES})
    
    target_link_libraries(ConvexHull_bdd
        convex_hull_lib
        Catch2::Catch2
    )

    target_include_directories(ConvexHull_bdd PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/app
        ${Boost_INCLUDE_DIRS}
    )
endif()
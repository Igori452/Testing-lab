cmake_minimum_required(VERSION 3.16)

option(BUILD_APP "Build main application" OFF)
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_MULL "Build with Mull mutation testing" OFF)
option(BUILD_BDD "Build BDD tests with Cucumber-Cpp" ON) 
option(BUILD_PVS "Build with PVS-Studio static analysis" OFF)  # НОВАЯ ОПЦИЯ

if(UNIX AND NOT APPLE)
    set(CMAKE_C_COMPILER clang-15)
    set(CMAKE_CXX_COMPILER clang++-15)
endif()

project(ConvexHull)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
elseif(BUILD_PVS)
    # PVS-Studio анализ
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_PVS_STUDIO_")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
    add_library(convex_hull_pvs OBJECT app/ConvexHull.cpp)
    target_include_directories(convex_hull_pvs PRIVATE app)
    
    message(STATUS "PVS-Studio analysis configured for ConvexHull.cpp")
else()
    # Обычная сборка
    if(NOT TARGET convex_hull_lib)
        add_library(convex_hull_lib STATIC app/ConvexHull.cpp)
        target_include_directories(convex_hull_lib PUBLIC app)
    endif()
endif()

if(BUILD_APP AND NOT BUILD_MULL AND NOT BUILD_PVS)
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

if(BUILD_TESTS AND NOT BUILD_MULL AND NOT BUILD_PVS)
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
#[[
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MSVC_RUNTIME_LIBRARY
    "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

project(ConvexHull)
set(CMAKE_CXX_STANDARD 17)

if(BUILD_TESTS)
    add_subdirectory(libraries/googletest)
    enable_testing()
endif()

# Опции сборки
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

set(CMAKE_CXX_FLAGS_SAVE ${CMAKE_CXX_FLAGS})

# Отключаем сборку примеров и тестов SFML
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

# Подключаем внешние библиотеки
add_subdirectory(libraries/sfml)
if(BUILD_TESTS)
    add_subdirectory(libraries/googletest)
    enable_testing()
endif()

# Основная библиотека с алгоритмами
add_library(convex_hull_lib STATIC
    ConvexHull.cpp
)

# Основное приложение (с графикой)
add_executable(ConvexHull
    main.cpp
    display.cpp)

target_link_libraries(ConvexHull convex_hull_lib sfml-graphics sfml-window sfml-system)

# Тесты
if(BUILD_TESTS)
    add_executable(ConvexHullTests
        tests/test_main.cpp
        tests/test_comprassion.cpp
        tests/test_graham.cpp
        tests/test_jarvis.cpp
        tests/test_common.cpp
        tests/test_mocks.cpp)
    
    target_link_libraries(ConvexHullTests 
        convex_hull_lib 
        gtest_main 
        gmock
    )
    
    target_include_directories(ConvexHullTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # Регистрируем тесты для CTest
    add_test(NAME ConvexHullTests COMMAND ConvexHullTests)
endif()]]

cmake_minimum_required(VERSION 3.16)

project(ConvexHullMull)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Принудительно используем Clang-15
set(CMAKE_C_COMPILER /usr/bin/clang-15)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-15)

set(CMAKE_CXX_FLAGS_COVERAGE
    "-g -O0 -fprofile-instr-generate -fcoverage-mapping"
    CACHE STRING "Flags used for coverage builds.")

set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "-fprofile-instr-generate"
    CACHE STRING "Linker flags for coverage builds.")

# Флаги для Mull
set(MULL_PLUGIN "/usr/lib/mull-ir-frontend-15")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpass-plugin=${MULL_PLUGIN} -g -grecord-command-line -O0 -fno-inline")

# GoogleTest
add_subdirectory(libraries/googletest)
enable_testing()

# Основная библиотека с алгоритмами (если нужна)
add_library(convex_hull_lib STATIC ConvexHull.cpp)

# Исполняемый файл тестов
add_executable(ConvexHullTests
    tests/test_main.cpp
    tests/test_comprassion.cpp
    tests/test_graham.cpp
    tests/test_jarvis.cpp
    tests/test_common.cpp
    tests/test_mocks.cpp
    ConvexHull.cpp  # если convex_hull_lib не используется
)

target_include_directories(ConvexHullTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ConvexHullTests
    # convex_hull_lib  # раскомментируйте если используете библиотеку
    gtest_main
    gmock
    pthread
)

# Регистрируем тест для CTest
add_test(NAME ConvexHullTests COMMAND ConvexHullTests)

#[[
    cmake .. \
    -DCMAKE_C_COMPILER=clang-15 \
    -DCMAKE_CXX_COMPILER=clang++-15 \
    -DCMAKE_BUILD_TYPE=Coverage \
    -DCMAKE_CXX_FLAGS="-g -O0 -fprofile-instr-generate -fcoverage-mapping" \
    -DCMAKE_EXE_LINKER_FLAGS="-fprofile-instr-generate"


    llvm-cov-15 show ./ConvexHullTests \
    -instr-profile=coverage.profdata \
    -format=html \
    -output-dir=./coverage_report \
    -show-line-counts-or-regions \
    -ignore-filename-regex="(tests|googletest|gmock)/*"

    mull-runner-15 -ide-reporter-show-killed ./ConvexHullTests
]]
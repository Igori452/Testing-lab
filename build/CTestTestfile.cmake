# CMake generated Testfile for 
# Source directory: C:/work/c++/testedProject
# Build directory: C:/work/c++/testedProject/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ConvexHull_tests "C:/work/c++/testedProject/build/ConvexHull_tests.exe")
set_tests_properties(ConvexHull_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/work/c++/testedProject/CMakeLists.txt;139;add_test;C:/work/c++/testedProject/CMakeLists.txt;0;")
subdirs("libraries/sfml")
subdirs("libraries/googletest")

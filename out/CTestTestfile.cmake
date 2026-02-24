# CMake generated Testfile for 
# Source directory: /project
# Build directory: /project/out
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ConvexHullTests "/project/out/ConvexHullTests")
set_tests_properties(ConvexHullTests PROPERTIES  _BACKTRACE_TRIPLES "/project/CMakeLists.txt;119;add_test;/project/CMakeLists.txt;0;")
subdirs("libraries/googletest")

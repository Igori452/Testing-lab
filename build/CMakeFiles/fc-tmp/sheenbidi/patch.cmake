cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

message(VERBOSE "Executing patch step for sheenbidi")

block(SCOPE_FOR VARIABLES)

execute_process(
  WORKING_DIRECTORY "C:/work/c++/testedProject/build/_deps/sheenbidi-src"
  COMMAND_ERROR_IS_FATAL LAST
  COMMAND  [====[C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe]====] [====[-DSHEENBIDI_DIR=C:/work/c++/testedProject/build/_deps/sheenbidi-src]====] [====[-P]====] [====[C:/work/c++/testedProject/libraries/sfml/tools/sheenbidi/PatchSheenBidi.cmake]====]
)

endblock()

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

message(VERBOSE "Executing patch step for mbedtls")

block(SCOPE_FOR VARIABLES)

execute_process(
  WORKING_DIRECTORY "C:/work/c++/testedProject/build/_deps/mbedtls-src"
  COMMAND_ERROR_IS_FATAL LAST
  COMMAND  [====[C:/Program Files/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe]====] [====[-DMBEDTLS_DIR=C:/work/c++/testedProject/build/_deps/mbedtls-src]====] [====[-P]====] [====[C:/work/c++/testedProject/libraries/sfml/tools/mbedtls/PatchMbedTLS.cmake]====]
)

endblock()

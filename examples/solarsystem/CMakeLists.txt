cmake_minimum_required(VERSION 3.8)

set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
set(EXECUTABLE_OUTPUT_PATH "bin")
set(CMAKE_C_STANDARD 17)

project(solarsystem)

set(CPPFLAGS "$ENV{CPPFLAGS} -O3 -Werror -Wextra -Wall -pedantic")
set(CXXFLAGS "$ENV{CXXFLAGS}")

# Work around linting bugs in older compilers
if(NOT DEFINED ENV{TARGET})
    set(CXXFLAGS "${CXXFLAGS} -Weffc++")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPPFLAGS} ${CXXFLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPPFLAGS}")

if((NOT DEFINED ENV{TARGET}) AND (CMAKE_C_COMPILER_ID MATCHES "Clang"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmost")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmost")
endif()

include_directories(include)
add_executable(solarsystem solarsystem.c)

set(HOME "$ENV{HOME}")
set(ARTIFACT solarsystem)
file(TO_NATIVE_PATH "${HOME}/bin" INSTALL_DIR)
file(TO_NATIVE_PATH "${INSTALL_DIR}/${ARTIFACT}" INSTALL_FILE)
install(PROGRAMS $<TARGET_FILE:solarsystem> DESTINATION "${INSTALL_DIR}")
add_custom_target(uninstall COMMAND rm -f "${INSTALL_FILE}")

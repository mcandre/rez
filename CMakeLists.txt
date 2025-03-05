cmake_minimum_required(VERSION 3.8)

set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
set(EXECUTABLE_OUTPUT_PATH "bin")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

project(rez)

file(GLOB_RECURSE CPP_SOURCE_FILES *.cppm *.hpp *.cpp *.h *.c)

if(
    NOT DEFINED ENV{TARGET} OR
    (
        (NOT "$ENV{TARGET}" MATCHES "netbsd") AND
        (NOT "$ENV{TARGET}" MATCHES "openbsd")
    )
)
    find_program(CPPCHECK_PATH "cppcheck")

    if ("${CPPCHECK_PATH}" STREQUAL "CPPCHECK_PATH-NOTFOUND")
        message(FATAL_ERROR "cppcheck not found")
    endif()

    set(
        CPPCHECK
        "cppcheck"
        "-q"
        "--force"
        "--enable=all"
        "--check-level=exhaustive"
        "--inline-suppr"
        "--suppressions-list=../suppressions.cfg"
        "--error-exitcode=1"
        "-U__SANITIZE_ADDRESS__"
    )
    set(CMAKE_C_CPPCHECK "${CPPCHECK}")
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK}")
endif()

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
    set(CMAKE_CXX_CLANG_TIDY clang-tidy --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
    set(CMAKE_C_CLANG_TIDY clang-tidy --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
endif()

include_directories(include)
add_executable(rez src/cmd/rez/main.cpp src/rez.cpp)

set(HOME "$ENV{HOME}")
set(ARTIFACT rez)
file(TO_NATIVE_PATH "${HOME}/bin" INSTALL_DIR)
file(TO_NATIVE_PATH "${INSTALL_DIR}/${ARTIFACT}" INSTALL_FILE)
install(PROGRAMS $<TARGET_FILE:rez> DESTINATION "${INSTALL_DIR}")
add_custom_target(uninstall COMMAND rm -f "${INSTALL_FILE}")

add_custom_target(cpplint COMMAND .venv/bin/cpplint --recursive ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
add_custom_target(clang-format COMMAND clang-format -i ${CPP_SOURCE_FILES})
add_custom_target(unmake COMMAND unmake ${CMAKE_SOURCE_DIR})
add_custom_target(unmake_dry_run COMMAND unmake -n ${CMAKE_SOURCE_DIR})
add_custom_target(lint DEPENDS cpplint clang-format unmake unmake_dry_run)

add_custom_target(doc COMMAND doxygen Doxyfile)

add_custom_target(snyk COMMAND snyk test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_custom_target(audit DEPENDS snyk)

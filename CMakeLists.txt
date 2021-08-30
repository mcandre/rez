cmake_minimum_required(VERSION 3.4)
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")
set(EXECUTABLE_OUTPUT_PATH "bin")
project(rez)

if(MSVC)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${EXECUTABLE_OUTPUT_PATH}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /INCREMENTAL:NO /std:c17 /Ox /WX /W4 /wd4204")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /INCREMENTAL:NO /std:c17 /Ox")
else()
    set(UNIX_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Werror")
    # set(GNU_C_FLAGS "${UNIX_C_FLAGS} -std=gnu17 -Wextra -Wall -pedantic")
    set(GNU_C_FLAGS "${UNIX_C_FLAGS} -std=gnu17")
    set(LLVM_FLAGS "-Wmost")
    set(CLANG_C_FLAGS "${GNU_C_FLAGS} ${LLVM_FLAGS}")

    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(CMAKE_C_FLAGS "${CLANG_C_FLAGS}")

        if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(CMAKE_C_CLANG_TIDY clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
        endif()
    else()
        set(CMAKE_C_FLAGS "${GNU_C_FLAGS}")
    endif()
endif()

include_directories(include)

add_executable(rez src/cmd/rez/main.c src/rez.c)

add_custom_target(cppcheck COMMAND cppcheck -q --force --enable=all --inline-suppr --suppressions-list=suppressions.cfg --error-exitcode=1 -U__SANITIZE_ADDRESS__ -I include src)
add_custom_target(cpplint COMMAND sail include src | xargs -n 1 cpplint)
add_custom_target(vera++ COMMAND sail include src | xargs -n 1 vera++)
add_custom_target(clang-format COMMAND sail include src | xargs -n 1 clang-format -i)

# Missing linter ports
if(MSVC)
    add_custom_target(lint DEPENDS cppcheck)
else()
    add_custom_target(lint DEPENDS cppcheck cpplint vera++ clang-format)
endif()

add_custom_target(doc COMMAND doxygen Doxyfile)
add_custom_target(clean-cmake COMMAND sh clean-cmake.sh)

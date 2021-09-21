cmake_minimum_required(VERSION 3.4)
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(BANNER rez-0.0.5)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(ARTIFACT_EXT)
set(TARGET_FLAG)

if(DEFINED ENV{TARGET})
    if(DEFINED ENV{TARGET} AND "$ENV{TARGET}" MATCHES "w64")
        set(CMAKE_C_COMPILER "$ENV{TARGET}-gcc-9.3-posix")
        set(CMAKE_CXX_COMPILER "$ENV{TARGET}-g++-posix")
        set(ARTIFACT_EXT .exe)
    else()
        if("$ENV{TARGET}" MATCHES "darwin")
            set(CMAKE_C_COMPILER "$ENV{TARGET}20.4-clang")
            set(CMAKE_CXX_COMPILER "$ENV{TARGET}20.4-clang++")
            set(CROSSBUILD "$ENV{TARGET}")
            set(MACOSX_DEPLOYMENT_TARGET "10.15")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}")
        else()
            if("$ENV{TARGET}" MATCHES "bsd")
                set(CMAKE_SYSROOT "/usr/local/$ENV{TARGET}")

                if("$ENV{TARGET}" MATCHES "openbsd")
                    set(CMAKE_CXX_FLAGS "${CLANG_CXX_FLAGS} -pthread")
                elseif("$ENV{TARGET}" MATCHES "dragonflybsd")
                    set(PREFIXES_WITH_LIBRARIES " \
                        -B /usr/local/$ENV{TARGET}/lib \
                        -B /usr/local/$ENV{TARGET}/usr/lib \
                        -B /usr/local/$ENV{TARGET}/usr/lib/gcc80 \
                        -B /usr/local/$ENV{TARGET}/usr/local/cxxfs_gcc8 \
                        -L /usr/local/$ENV{TARGET}/lib \
                        -L /usr/local/$ENV{TARGET}/usr/lib \
                        -L /usr/local/$ENV{TARGET}/usr/lib/gcc80 \
                        -L /usr/local/$ENV{TARGET}/usr/local/cxxfs_gcc8 \
                    ")

                    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PREFIXES_WITH_LIBRARIES}")
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PREFIXES_WITH_LIBRARIES}")

                    include_directories("/usr/local/$ENV{TARGET}/usr/include/c++/8.0")
                    include_directories("/usr/local/$ENV{TARGET}/usr/libdata/gcc80")
                else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
                endif()
            endif()

            set(CMAKE_C_COMPILER_TARGET "$ENV{TARGET}")
            set(CMAKE_CXX_COMPILER_TARGET "$ENV{TARGET}")
            include_directories("/usr/local/$ENV{TARGET}/include/c++/v1")
            include_directories("/usr/$ENV{TARGET}/include/c++/9/$ENV{TARGET}")
            include_directories("/usr/$ENV{TARGET}/include")
            include_directories("/usr/local/$ENV{TARGET}/usr/include")
            include_directories("/usr/local/$ENV{TARGET}/usr/include/c++/v1")

            if(NOT "$ENV{TARGET}" MATCHES "openbsd")
                include_directories("/usr/local/$ENV{TARGET}/usr/include/g++")
            endif()
        endif()
    endif()

    set(EXECUTABLE_OUTPUT_PATH "bin/${BANNER}/$ENV{TARGET}")
endif()

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

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /INCREMENTAL:NO /std:c17 /EHsc /Ox /WX /W4 /wd4204 /Wv:18")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /INCREMENTAL:NO /std:c++17 /EHsc /Ox /WX /W4 /wd4204 /Wv:18")
else()
    set(UNIX_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Werror")
    set(UNIX_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror")

    set(GNU_C_FLAGS "${UNIX_C_FLAGS} -std=gnu17 -Wextra -Wall -pedantic -Wno-unused-command-line-argument")
    set(GNU_CXX_FLAGS "${UNIX_CXX_FLAGS} -std=c++17 -Wextra -Wall -pedantic -Wno-unused-command-line-argument -Weffc++")

    set(LLVM_FLAGS "-Wmost")
    set(CLANG_C_FLAGS "${GNU_C_FLAGS} ${LLVM_FLAGS}")
    set(CLANG_CXX_FLAGS "${GNU_CXX_FLAGS} ${LLVM_FLAGS}")

    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(CMAKE_C_FLAGS "${CLANG_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CLANG_CXX_FLAGS}")

        if(NOT DEFINED ENV{TARGET} AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(CMAKE_C_CLANG_TIDY clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
            set(CMAKE_CXX_CLANG_TIDY clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
        endif()
    else()
        set(CMAKE_C_FLAGS "${GNU_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${GNU_CXX_FLAGS}")
    endif()
endif()

include_directories(include)

add_executable(rez src/cmd/rez/main.cpp src/rez.cpp)
set_target_properties(
    rez PROPERTIES
    OUTPUT_NAME "rez${ARTIFACT_EXT}"
)

if(MSVC)
    set(HOME "$ENV{HOMEDRIVE}$ENV{HOMEPATH}")
    set(ARTIFACT rez.exe)
else()
    set(HOME "$ENV{HOME}")
    set(ARTIFACT rez)
endif()

if(DEFINED ENV{TARGET})
    if("$ENV{TARGET}" MATCHES "w64")
        target_link_libraries(rez ucrt)
    elseif("$ENV{TARGET}" MATCHES "dragonflybsd")
        target_link_libraries(rez stdc++fs)
    elseif("$ENV{TARGET}" MATCHES "netbsd")
        target_link_libraries(rez c++abi unwind)
    endif()
endif()

file(TO_NATIVE_PATH "${HOME}/bin" INSTALL_DIR)
file(TO_NATIVE_PATH "${INSTALL_DIR}/${ARTIFACT}" INSTALL_FILE)

install(PROGRAMS $<TARGET_FILE:rez> DESTINATION "${INSTALL_DIR}")

if(MSVC)
    add_custom_target(uninstall COMMAND del /s /q "${INSTALL_FILE}")
else()
    add_custom_target(uninstall COMMAND rm -f "${INSTALL_FILE}")
endif()

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

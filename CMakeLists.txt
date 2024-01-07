cmake_minimum_required(VERSION 3.4)
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(BANNER rez-0.0.11)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(ARTIFACT_EXT)
set(TARGET_FLAG)

set_property(
    DIRECTORY .
    APPEND PROPERTY
    ADDITIONAL_CLEAN_FILES
    "${EXECUTABLE_OUTPUT_PATH};debug;debug.log;Makefile;build.ninja;.ninja_deps;Testing;CTestTestfile.cmake;CMakeFiles;CMakeCache.txt;cmake_install.cmake;conanbuildinfo.cmake;conanbuildinfo.txt;conaninfo.txt;graph_info.json;conan.lock"
)

if(DEFINED ENV{TARGET})
    set(STATIC_FLAG -static)

    if(DEFINED ENV{TARGET} AND "$ENV{TARGET}" MATCHES "w64")
        set(CMAKE_CXX_COMPILER "$ENV{TARGET}-g++-posix")
        set(CMAKE_C_COMPILER "$ENV{TARGET}-gcc-10-posix")
        set(ARTIFACT_EXT .exe)
    else()
        if("$ENV{TARGET}" MATCHES "darwin")
            set(CMAKE_CXX_COMPILER "$ENV{TARGET}20.4-clang++")
            set(CMAKE_C_COMPILER "$ENV{TARGET}20.4-clang")
            set(CROSSBUILD "$ENV{TARGET}")
            set(CPPFLAGS "-mmacosx-version-min=10.15")
            set(STATIC_FLAG)
        else()
            if("$ENV{TARGET}" MATCHES "bsd")
                set(CMAKE_SYSROOT "/usr/local/$ENV{TARGET}")

                if("$ENV{TARGET}" MATCHES "openbsd")
                    set(CPPFLAGS "${CPPFLAGS} -pthread")
                    set(STATIC_FLAG)
                else()
                    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
                endif()
            endif()

            set(CMAKE_CXX_COMPILER_TARGET "$ENV{TARGET}")
            set(CMAKE_C_COMPILER_TARGET "$ENV{TARGET}")
            include_directories("/usr/local/$ENV{TARGET}/include/c++/v1")
            include_directories("/usr/$ENV{TARGET}/include/c++/9/$ENV{TARGET}")
            include_directories("/usr/local/$ENV{TARGET}/usr/include/c++/v1")
            include_directories("/usr/$ENV{TARGET}/include")
            include_directories("/usr/local/$ENV{TARGET}/usr/include")

            if(NOT "$ENV{TARGET}" MATCHES "openbsd")
                include_directories("/usr/local/$ENV{TARGET}/usr/include/g++")
            endif()
        endif()
    endif()

    set(EXECUTABLE_OUTPUT_PATH "bin/${BANNER}/$ENV{TARGET}")
    set(CPPFLAGS "${CPPFLAGS} ${STATIC_FLAG}")
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
    set(CPPFLAGS "/EHsc /Ox /Wv:18 /INCREMENTAL:NO /WX /W4 /wd4204")
    set(CMAKE_CXX_FLAGS "${CPPFLAGS} ${CMAKE_CXX_FLAGS} /std:c++17")
    set(CMAKE_C_FLAGS "${CPPFLAGS} ${CMAKE_C_FLAGS} /std:c17")
else()
    set(CPPFLAGS "${CPPFLAGS} -O3 -Werror -Wextra -Wall -pedantic")
    set(CMAKE_CXX_FLAGS "${CPPFLAGS} ${CMAKE_CXX_FLAGS} -Weffc++ -std=c++17")
    set(CMAKE_C_FLAGS "${CPPFLAGS} ${CMAKE_C_FLAGS} -std=gnu17")

    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmost")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmost")

        if(NOT DEFINED ENV{TARGET} AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(CMAKE_CXX_CLANG_TIDY clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
            set(CMAKE_C_CLANG_TIDY clang-tidy -header-filter=${CMAKE_SOURCE_DIR})
        endif()
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
        target_link_libraries(rez c++ c++abi unwind)
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

add_custom_target(audit COMMAND safety check)

add_custom_target(cppcheck COMMAND cppcheck -q --force --enable=all --inline-suppr --suppressions-list=suppressions.cfg --error-exitcode=1 -U__SANITIZE_ADDRESS__ -I include src)
add_custom_target(cpplint COMMAND cpplint --recursive include src)
add_custom_target(clang-format COMMAND sail -print0 include src | xargs -0 -n 1 clang-format -i)
add_custom_target(yamllint COMMAND yamllint -s .yamllint .)
add_custom_target(unmake COMMAND unmake .)

# Missing linter ports
if(MSVC)
    add_custom_target(lint DEPENDS cppcheck yamllint unmake)
else()
    add_custom_target(lint DEPENDS cppcheck cpplint clang-format yamllint unmake)
endif()

add_custom_target(doc COMMAND doxygen Doxyfile)

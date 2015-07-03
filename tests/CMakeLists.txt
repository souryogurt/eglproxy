project (TESTS C)
cmake_minimum_required(VERSION 2.8.9)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(NINJA_MODE "Enable all warnings" ON)


find_package(OpenGL REQUIRED)
list(APPEND TESTS_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND TESTS_LIBRARIES ${OPENGL_gl_LIBRARY})
list(APPEND TESTS_INCLUDE_DIRS "../inc")

if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    list(APPEND TESTS_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
endif()

if(NINJA_MODE)
    if (CMAKE_C_COMPILER_ID MATCHES "^GNU$")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c89 -pedantic -Wall -Wextra -Wformat-nonliteral -Wformat-security -Wformat-y2k -Winit-self -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wsync-nand -Wstrict-overflow=5 -Wtrampolines -Wfloat-equal -Wdeclaration-after-statement -Wundef -Wshadow -Wunsafe-loop-optimizations -Wbad-function-cast -Wc++-compat -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wsign-conversion -Wlogical-op -Waggregate-return -Wstrict-prototypes -Wold-style-definition -Wnormalized=nfc -Wredundant-decls -Wnested-externs -Winline -Wvla -Wdisabled-optimization -Wmissing-prototypes -Wmissing-declarations")
    elseif(CMAKE_C_COMPILER_ID MATCHES "^MSVC$")
        if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/Wall" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        else()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Wall")
        endif()
    elseif(CMAKE_C_COMPILER_ID MATCHES "^Clang$")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c89 -pedantic -Weverything")
    endif()
endif()

include_directories(${TESTS_INCLUDE_DIRS})

list(APPEND EGLTEST_SOURCES "egltest.c")
list(APPEND EGLTEST_SOURCES "platform_mock.c")
list(APPEND EGLTEST_SOURCES "../src/egl_proc.c")
list(APPEND EGLTEST_SOURCES "../src/eglproxy.c")
list(APPEND EGLTEST_HEADERS "minunit.h")
list(APPEND EGLTEST_HEADERS "../inc/eglproxy.h")
add_executable(egltest ${EGLTEST_SOURCES} ${EGLTEST_HEADERS})
target_link_libraries(egltest ${TESTS_LIBRARIES})

add_custom_target(check COMMAND egltest DEPENDS egltest)

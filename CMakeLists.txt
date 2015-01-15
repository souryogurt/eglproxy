project (EGLPROXY C)
cmake_minimum_required(VERSION 2.8.9)

set(EGLPROXY_VERSION_MAJOR "0")
set(EGLPROXY_VERSION_MINOR "1")
set(EGLPROXY_VERSION "${EGLPROXY_VERSION_MAJOR}.${EGLPROXY_VERSION_MINOR}")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

list(APPEND EGLPROXY_SOURCES "src/eglproxy.c")
list(APPEND EGLPROXY_SOURCES "src/egl_proc.c")
list(APPEND EGLPROXY_HEADERS "inc/eglproxy.h")
list(APPEND EGLPROXY_INCLUDE_DIRS "inc")

find_package(OpenGL REQUIRED)
list(APPEND EGLPROXY_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
list(APPEND EGLPROXY_LIBRARIES ${OPENGL_gl_LIBRARY})

if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    list(APPEND EGLPROXY_INCLUDE_DIRS ${X11_X11_INCLUDE_PATH})
    list(APPEND EGLPROXY_SOURCES "src/egl_glx.c")
elseif(WIN32)
    list(APPEND EGLPROXY_SOURCES "src/egl_wgl.c")
endif()

include_directories(${EGLPROXY_INCLUDE_DIRS})
add_library(eglproxy ${EGLPROXY_SOURCES} ${EGLPROXY_HEADERS})
target_link_libraries(eglproxy ${EGLPROXY_LIBRARIES})
cmake_minimum_required(VERSION 2.6)

project(glues)
add_definitions(-DLIBRARYBUILD -O2 -D__USE_SDL_GLES__=1)

include_directories(include)
include_directories(source)
include_directories(source/libnurbs/internals source/libnurbs/interface source/libnurbs/nurbtess)

aux_source_directory(source SRC)
aux_source_directory(source/libtess TESS_SRC)
file(GLOB_RECURSE NURBS_SRC source/libnurbs/*.cc)
add_library(GLU SHARED ${SRC} ${TESS_SRC} ${NURBS_SRC})
target_link_libraries(GLU GL m)

if(CMAKE_SHARED_LIBRARY_SUFFIX MATCHES ".so")
    set_target_properties(GLU PROPERTIES SUFFIX ".so.1")
endif()


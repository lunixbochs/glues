cmake_minimum_required(VERSION 2.6)

project(glues)
add_definitions(-std=c99)

include_directories(include)
include_directories(source)

add_definitions(-D__USE_SDL_GLES__=1)
aux_source_directory(source SRC)
aux_source_directory(source/libtess TESS_SRC)
aux_source_directory(libnurbs/*/ NURBS_SRC)
add_library(GLU SHARED ${SRC} ${TESS_SRC} ${NURBS_SRC})
target_link_libraries(GLU GL m)

if(CMAKE_SHARED_LIBRARY_SUFFIX MATCHES ".so")
    set_target_properties(GLU PROPERTIES SUFFIX ".so.1")
endif()


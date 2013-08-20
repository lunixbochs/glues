cmake_minimum_required(VERSION 2.6)

project(glues)
add_definitions(-DLIBRARYBUILD -D__USE_SDL_GLES__=1)

include_directories(include)
include_directories(source)

aux_source_directory(source SRC)
aux_source_directory(source/libtess TESS_SRC)
file(GLOB_RECURSE NURBS_SRC source/libnurbs/*.cc)
add_library(GLUES_CM SHARED ${SRC} ${TESS_SRC} ${NURBS_SRC})
target_link_libraries(GLUES_CM m)


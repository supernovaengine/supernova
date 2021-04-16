if(DEFINED NO_LUA_INIT)
    add_definitions("-DNO_LUA_INIT=${NO_LUA_INIT}")
    unset(NO_LUA_INIT CACHE)
endif()
if(DEFINED NO_CPP_INIT)
    add_definitions("-DNO_CPP_INIT=${NO_CPP_INIT}")
    unset(NO_CPP_INIT CACHE)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(libs/json)

include_directories(libs/sokol)
include_directories(libs/lua)
include_directories(libs/luaintf)
include_directories(libs/stb)
include_directories(libs/tinyobjloader)
include_directories(libs/tinygltf)

add_subdirectory(libs/sokol)
add_subdirectory(libs/lua)
add_subdirectory(libs/luaintf)
add_subdirectory(libs/stb)
add_subdirectory(libs/tinyobjloader)
add_subdirectory(libs/tinygltf)

file(GLOB_RECURSE SUPERNOVA_SRCS core/*.cpp)
file(GLOB RENDER_SOKOL_SRC renders/sokol/*.cpp)

add_library(
    supernova

    STATIC

    ${RENDER_SOKOL_SRC}
    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova

    sokol lua luaintf stb tinyobjloader tinygltf

    #supernova-renders
    #soloud box2d tinyxml2
)
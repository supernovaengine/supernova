#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories(shaders)

include_directories(libs)
include_directories(libs/luabridge3)
include_directories(libs/json)

include_directories(libs/sokol)
include_directories(libs/lua)
include_directories(libs/stb)
include_directories(libs/soloud/include)
include_directories(libs/tinyobjloader)
include_directories(libs/tinygltf)
include_directories(libs/tinyxml2)

add_subdirectory(libs/sokol)
add_subdirectory(libs/lua)
add_subdirectory(libs/stb)
add_subdirectory(libs/soloud)
add_subdirectory(libs/tinyobjloader)
add_subdirectory(libs/tinygltf)
add_subdirectory(libs/tinyxml2)

if(DEFINED NO_LUA_INIT)
    add_definitions("-DNO_LUA_INIT=${NO_LUA_INIT}")
    unset(NO_LUA_INIT CACHE)
endif()
if(DEFINED NO_CPP_INIT)
    add_definitions("-DNO_CPP_INIT=${NO_CPP_INIT}")
    unset(NO_CPP_INIT CACHE)
endif()

#add_definitions("-DPROJECT_ROOT=\"${PROJECT_ROOT}\"")

file(GLOB_RECURSE SUPERNOVA_SRCS core/*.cpp)
file(GLOB RENDER_SOKOL_SRC renders/sokol/*.cpp)

add_library(
    supernova

    STATIC

    ${RENDER_SOKOL_SRC}
    ${SUPERNOVA_SRCS}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        supernova 

        PUBLIC 

        "-pthread"
    )
endif()

target_link_libraries(
    supernova

    sokol lua stb tinyobjloader tinygltf tinyxml2 soloud
    #box2d
)

set_target_properties(
    supernova

    PROPERTIES

    CXX_STANDARD 17
)
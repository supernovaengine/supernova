#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

option(SUPERNOVA_SHARED "Build supernova as a shared library" OFF)

if(SUPERNOVA_SHARED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(SUPERNOVA_LIB_TYPE SHARED)
else()
    set(SUPERNOVA_LIB_TYPE STATIC)
endif()

# Add OpenGL handling for macOS
if(APPLE)
    find_package(OpenGL REQUIRED)
endif()

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
include_directories(libs/box2d/include)
include_directories(libs/joltphysics)

add_subdirectory(libs/sokol)
add_subdirectory(libs/lua)
add_subdirectory(libs/stb)
add_subdirectory(libs/soloud)
add_subdirectory(libs/tinyobjloader)
add_subdirectory(libs/tinygltf)
add_subdirectory(libs/tinyxml2)
add_subdirectory(libs/box2d)
add_subdirectory(libs/joltphysics)

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

    ${SUPERNOVA_LIB_TYPE}

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

# Base libraries
set(SUPERNOVA_LIBS sokol lua stb tinyobjloader tinygltf tinyxml2 soloud box2d joltphysics)

# Add OpenGL framework for macOS
if(APPLE)
    list(APPEND SUPERNOVA_LIBS "-framework OpenGL")
endif()

target_link_libraries(
    supernova

    ${SUPERNOVA_LIBS}
)

set_target_properties(
    supernova

    PROPERTIES

    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    CXX_STANDARD 17
)

if(SUPERNOVA_SHARED)
    target_compile_definitions(supernova 
        PRIVATE 
        SUPERNOVA_EXPORTS
        PUBLIC
        SUPERNOVA_SHARED
    )
endif()
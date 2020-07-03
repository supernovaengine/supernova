if(DEFINED NO_LUA_INIT)
    add_definitions("-DNO_LUA_INIT=${NO_LUA_INIT}")
endif()
if(DEFINED NO_CPP_INIT)
    add_definitions("-DNO_CPP_INIT=${NO_CPP_INIT}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB_RECURSE SUPERNOVA_SRCS *.cpp)


add_library(
    supernova

    STATIC

    ${SUPERNOVA_SRCS}
)

target_link_libraries(
    supernova

    supernova-renders
    lua luaintf tinyobjloader soloud stb box2d tinygltf tinyxml2
)
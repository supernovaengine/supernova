add_definitions("-frtti -fexceptions")
# add_definitions("-Wall -Wextra")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/lua")
add_subdirectory (libs/lua)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/luaintf")
add_subdirectory (libs/luaintf)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyobjloader")
add_subdirectory (libs/tinyobjloader)

if (COMPILE_ZLIB)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/zlib")
    add_subdirectory (libs/zlib)
endif (COMPILE_ZLIB)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/soloud/include")
add_subdirectory (libs/soloud)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/stb")
add_subdirectory (libs/stb)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/json")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/tinygltf")
add_subdirectory (libs/tinygltf)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/tinyxml2")
add_subdirectory (libs/tinyxml2)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/libs/box2d")
add_subdirectory (libs/box2d)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/renders")
add_subdirectory (renders)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory (core)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions("-frtti -fexceptions -Wall -Wextra")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/lua")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/luaintf")
add_subdirectory (engine/libs/lua)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/tinyobjloader")
add_subdirectory (engine/libs/tinyobjloader)

if (COMPILE_ZLIB)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/zlib")
    add_subdirectory (engine/libs/zlib)
endif (COMPILE_ZLIB)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/libpng")
add_subdirectory (engine/libs/libpng)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/core")
add_subdirectory (engine/core)

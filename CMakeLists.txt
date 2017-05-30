add_definitions("-frtti -fexceptions")
# add_definitions("-Wall -Wextra")

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/lua")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/luaintf")
add_subdirectory (engine/libs/lua)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/tinyobjloader")
add_subdirectory (engine/libs/tinyobjloader)

if (COMPILE_ZLIB)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/zlib")
    add_subdirectory (engine/libs/zlib)
endif (COMPILE_ZLIB)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/mpg123/src/libmpg123")
add_subdirectory (engine/libs/mpg123)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/soloud/include")
add_subdirectory (engine/libs/soloud)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/stb")
add_subdirectory (engine/libs/stb)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/core")
add_subdirectory (engine/core)

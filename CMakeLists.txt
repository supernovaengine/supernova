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

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/libpng")
add_subdirectory (engine/libs/libpng)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/libtiff")
add_subdirectory (engine/libs/libtiff)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/libjpeg")
add_subdirectory (engine/libs/libjpeg)

if (COMPILE_OPENAL)
    include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/openal-soft/include")
    add_subdirectory (engine/libs/openal-soft)
endif (COMPILE_OPENAL)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/libs/mpg123/src/libmpg123")
add_subdirectory (engine/libs/mpg123)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/engine/core")
add_subdirectory (engine/core)

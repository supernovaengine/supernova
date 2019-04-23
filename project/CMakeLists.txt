include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/action")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/action/keyframe")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/action/particleinit")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/action/particlemod")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/audio")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/buffer")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/file")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/image")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/math")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/physics")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/platform")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/render")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/ui")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/../engine/core/util")

file(GLOB_RECURSE SUPERNOVA_PROJECT_SRCS *.cpp)

add_library(
    supernova-project

    STATIC

    ${SUPERNOVA_PROJECT_SRCS}
)

target_link_libraries(
    supernova-project
    supernova
)
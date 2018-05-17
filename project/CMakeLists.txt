file(GLOB_RECURSE SUPERNOVA_PROJECT_SRCS *.cpp)

add_library(
    supernova-project

    STATIC

    ${SUPERNOVA_PROJECT_SRCS}
)

target_link_libraries(
    supernova-project
    supernova-core
)
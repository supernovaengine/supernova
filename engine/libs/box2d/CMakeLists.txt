# Box2D - https://github.com/erincatto/box2d/

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src")

set(BOX2D_SOURCE_FILES
	./src/aabb.c
	./src/aabb.h
	./src/allocate.c
	./src/allocate.h
	./src/array.c
	./src/array.h
	./src/bitset.c
	./src/bitset.h
	./src/block_array.c
	./src/block_array.h
	./src/body.c
	./src/body.h
	./src/broad_phase.c
	./src/broad_phase.h
	./src/constraint_graph.c
	./src/constraint_graph.h
	./src/contact.c
	./src/contact.h
	./src/contact_solver.c
	./src/contact_solver.h
	./src/core.c
	./src/core.h
	./src/ctz.h
	./src/distance.c
	./src/distance_joint.c
	./src/dynamic_tree.c
	./src/geometry.c
	./src/hull.c
	./src/id_pool.c
	./src/id_pool.h
	./src/island.c
	./src/island.h
	./src/joint.c
	./src/joint.h
	./src/manifold.c
	./src/math_functions.c
	./src/motor_joint.c
	./src/mouse_joint.c
	./src/prismatic_joint.c
	./src/revolute_joint.c
	./src/shape.c
	./src/shape.h
	./src/solver.c
	./src/solver.h
	./src/solver_set.c
	./src/solver_set.h
	./src/stack_allocator.c
	./src/stack_allocator.h
	./src/table.c
	./src/table.h
	./src/timer.c
	./src/types.c
	./src/weld_joint.c
	./src/wheel_joint.c
	./src/world.c
	./src/world.h
)

set(BOX2D_API_FILES
	./include/box2d/base.h
	./include/box2d/box2d.h
	./include/box2d/collision.h
	./include/box2d/id.h
	./include/box2d/math_functions.h
	./include/box2d/types.h
)

add_library(

    box2d

    STATIC

    ${BOX2D_SOURCE_FILES}
	${BOX2D_API_FILES}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(
        box2d 

        PUBLIC 

        "-pthread"
    )
endif()
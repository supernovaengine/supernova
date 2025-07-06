# Box2D - https://github.com/erincatto/box2d/

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/src")

set(BOX2D_SOURCE_FILES
	./src/aabb.c
	./src/aabb.h
	./src/array.c
	./src/array.h
	./src/bitset.c
	./src/bitset.h
	./src/body.c
	./src/body.h
	./src/broad_phase.c
	./src/broad_phase.h
	./src/constants.h
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

find_package(Threads REQUIRED)
target_link_libraries(box2d PRIVATE Threads::Threads)

if(EMSCRIPTEN AND EMSCRIPTEN_THREAD_SUPPORT)
	target_compile_options(box2d PRIVATE -pthread)
	#target_compile_definitions(box2d PRIVATE B2_MAX_WORKER_COUNT=0)
endif()

# part of Box2D CMakeLists.txt configuration

# Deterministic math
# https://box2d.org/posts/2024/08/determinism/
if (MINGW OR APPLE OR UNIX)
	add_compile_options(-ffp-contract=off)
endif()


# part of Box2D src/CMakeLists.txt configuration

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set_target_properties(
	box2d

	PROPERTIES

	C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)

if (BOX2D_PROFILE)
	target_compile_definitions(box2d PRIVATE BOX2D_PROFILE)

	FetchContent_Declare(
		tracy
		GIT_REPOSITORY https://github.com/wolfpld/tracy.git
		GIT_TAG v0.11.1
		GIT_SHALLOW TRUE
		GIT_PROGRESS TRUE
	)
	FetchContent_MakeAvailable(tracy)

	target_link_libraries(box2d PUBLIC TracyClient)
endif()

if (BOX2D_VALIDATE)
	target_compile_definitions(box2d PRIVATE BOX2D_VALIDATE)
endif()

if (BOX2D_ENABLE_SIMD)
	target_compile_definitions(box2d PRIVATE BOX2D_ENABLE_SIMD)
endif()

if (MSVC)
	message(STATUS "Box2D on MSVC")	
	if (BUILD_SHARED_LIBS)
		# this is needed by DLL users to import Box2D symbols
		target_compile_definitions(box2d INTERFACE BOX2D_DLL)
	endif()

	# Visual Studio won't load the natvis unless it is in the project
	target_sources(box2d PRIVATE ./src/box2d.natvis)

	# Enable asserts in release with debug info
	target_compile_definitions(box2d PUBLIC "$<$<CONFIG:RELWITHDEBINFO>:B2_ENABLE_ASSERT>")

	# Atomics are still considered experimental in Visual Studio 17.8
	target_compile_options(box2d PRIVATE /experimental:c11atomics)

	# All warnings
	target_compile_options(box2d PRIVATE /W4)

	if (BOX2D_AVX2)
		message(STATUS "Box2D using AVX2")	
		target_compile_definitions(box2d PRIVATE BOX2D_AVX2)
		target_compile_options(box2d PRIVATE /arch:AVX2)
	endif()

elseif (MINGW)
	message(STATUS "Box2D on MinGW")
	if (BOX2D_AVX2)
		message(STATUS "Box2D using AVX2")	
		target_compile_definitions(box2d PRIVATE BOX2D_AVX2)
		target_compile_options(box2d PRIVATE -mavx2)
	else()
	endif()
elseif (APPLE)
	message(STATUS "Box2D on Apple")
elseif (EMSCRIPTEN)
	message(STATUS "Box2D on Emscripten")
	target_compile_options(box2d PRIVATE -msimd128 -msse2)
elseif (UNIX)
	message(STATUS "Box2D using Unix")
	if ("${CMAKE_HOST_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
		# raspberry pi
		# -mfpu=neon
		# target_compile_options(box2d PRIVATE)
	else()
		# x64
		if (BOX2D_AVX2)
			message(STATUS "Box2D using AVX2")
			# FMA -mfma -mavx -march=native
			target_compile_definitions(box2d PRIVATE BOX2D_AVX2)
			target_compile_options(box2d PRIVATE -mavx2)
		else()
		endif()
	endif()
endif()
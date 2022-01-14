cmake_minimum_required(VERSION 3.10)

project(mtop)

add_subdirectory(display)
add_subdirectory(process)

add_executable(mtop 	
	opt.c
	mtop.c
	error.c
	draw.c
	xutil.c
)

SET(GCC_COMPILE_FLAGS "-g -O2 -Wall")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_compile_options(-g)

target_link_libraries(mtop
	PUBLIC display
	PUBLIC process
	-lprocps
)

target_include_directories(mtop PUBLIC
	"."
	"./**"
	# "process"
	# "display"
)

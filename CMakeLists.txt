cmake_minimum_required(VERSION 3.10)

project(cmtop)

add_subdirectory(display)
add_subdirectory(process)

add_executable(cmtop 	
	opt.c
	cmtop.c
	error.c
	mtxline.c
	draw.c
	xutil.c
)

SET(GCC_COMPILE_FLAGS "-g -O2 -Wall")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")

add_compile_options(-g)

target_link_libraries(cmtop
	PUBLIC display
	PUBLIC process
	-lprocps
)

target_include_directories(cmtop PUBLIC
	"."
	"./**"
	# "process"
	# "display"
)
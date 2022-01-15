cmake_minimum_required(VERSION 3.10)

project(mtop)

add_subdirectory(src/display)
add_subdirectory(src/process)

add_executable(mtop 	
	src/opt.c
	src/mtop.c
	src/error.c
	src/draw.c
	src/xutil.c
)

SET(GCC_COMPILE_FLAGS "-g -O2 -Wall")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_compile_options(-g)

target_link_libraries(mtop
	PUBLIC display
	PUBLIC process
	-lprocps
)

target_include_directories(mtop PUBLIC
	"src"
	"src/**"
	# "process"
	# "display"
)

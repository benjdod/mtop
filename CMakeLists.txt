cmake_minimum_required(VERSION 3.10)

project(mtop)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_compile_definitions(
	MTOP_MULTITHREAD
)

add_compile_options(
	$<$<CONFIG:DEBUG>:-pg>
	$<$<CONFIG:DEBUG>:-Og>
	$<$<CONFIG:RELEASE>:-O3>
	-Wall
)

add_subdirectory(src/display)
add_subdirectory(src/process)

add_executable(mtop 	
	src/opt.c
	src/mtop.c
	src/error.c
	src/xutil.c
)

target_link_libraries(mtop
	PUBLIC display
	PUBLIC process
	-lprocps
	-lm
	-lpthread
)

target_include_directories(mtop PUBLIC
	"src"
	"src/**"
	# "process"
	# "display"
)

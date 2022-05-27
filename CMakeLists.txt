cmake_minimum_required(VERSION 3.10)

SET(CMAKE_DISABLE_SOURCE_CHANGES ON)
SET(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(mtop)

SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_compile_definitions(
	MTOP_MULTITHREAD
)

add_compile_options(
	$<$<CONFIG:DEBUG>:-pg>
	$<$<CONFIG:DEBUG>:-g>
	$<$<CONFIG:DEBUG>:-Og>
	$<$<CONFIG:RELEASE>:-O3>
	-Wall
)

add_executable(mtop 	
	src/opt.c
	src/mtop.c
	src/error.c
	src/xutil.c
)

add_subdirectory(src/display)
add_subdirectory(src/process)

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
)

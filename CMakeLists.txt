cmake_minimum_required(VERSION 3.10)

project(cmatrix-top)


add_executable(cmtop 	
	cmtop.c
	screen.c
	tty.c
	error.c
	mtxline.c
	proc.c	
	procbst.c
)

SET(GCC_COMPILE_FLAGS "-ggdb")
SET(CMAKE_C_FLAGS "${GCC_COMPILE_FLAGS}")

target_link_libraries(cmtop
	-lprocps	
)

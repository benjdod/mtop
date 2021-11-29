cmake_minimum_required(VERSION 3.10)

project(cmatrix-top)


add_executable(cmtop 	
	cmtop.c
	screen.c
	tty.c
	error.c
	mtxline.c
	proc.c	
)

target_link_libraries(cmtop
	-lprocps	
)

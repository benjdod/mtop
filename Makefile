# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bjd/projects/cmatrix-top

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bjd/projects/cmatrix-top

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/bjd/projects/cmatrix-top/CMakeFiles /home/bjd/projects/cmatrix-top/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/bjd/projects/cmatrix-top/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named cmtop

# Build rule for target.
cmtop: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 cmtop
.PHONY : cmtop

# fast build rule for target.
cmtop/fast:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/build
.PHONY : cmtop/fast

cmtop.o: cmtop.c.o

.PHONY : cmtop.o

# target to build an object file
cmtop.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/cmtop.c.o
.PHONY : cmtop.c.o

cmtop.i: cmtop.c.i

.PHONY : cmtop.i

# target to preprocess a source file
cmtop.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/cmtop.c.i
.PHONY : cmtop.c.i

cmtop.s: cmtop.c.s

.PHONY : cmtop.s

# target to generate assembly for a file
cmtop.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/cmtop.c.s
.PHONY : cmtop.c.s

error.o: error.c.o

.PHONY : error.o

# target to build an object file
error.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/error.c.o
.PHONY : error.c.o

error.i: error.c.i

.PHONY : error.i

# target to preprocess a source file
error.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/error.c.i
.PHONY : error.c.i

error.s: error.c.s

.PHONY : error.s

# target to generate assembly for a file
error.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/error.c.s
.PHONY : error.c.s

mtxline.o: mtxline.c.o

.PHONY : mtxline.o

# target to build an object file
mtxline.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/mtxline.c.o
.PHONY : mtxline.c.o

mtxline.i: mtxline.c.i

.PHONY : mtxline.i

# target to preprocess a source file
mtxline.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/mtxline.c.i
.PHONY : mtxline.c.i

mtxline.s: mtxline.c.s

.PHONY : mtxline.s

# target to generate assembly for a file
mtxline.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/mtxline.c.s
.PHONY : mtxline.c.s

proc.o: proc.c.o

.PHONY : proc.o

# target to build an object file
proc.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/proc.c.o
.PHONY : proc.c.o

proc.i: proc.c.i

.PHONY : proc.i

# target to preprocess a source file
proc.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/proc.c.i
.PHONY : proc.c.i

proc.s: proc.c.s

.PHONY : proc.s

# target to generate assembly for a file
proc.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/proc.c.s
.PHONY : proc.c.s

procbst.o: procbst.c.o

.PHONY : procbst.o

# target to build an object file
procbst.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/procbst.c.o
.PHONY : procbst.c.o

procbst.i: procbst.c.i

.PHONY : procbst.i

# target to preprocess a source file
procbst.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/procbst.c.i
.PHONY : procbst.c.i

procbst.s: procbst.c.s

.PHONY : procbst.s

# target to generate assembly for a file
procbst.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/procbst.c.s
.PHONY : procbst.c.s

screen.o: screen.c.o

.PHONY : screen.o

# target to build an object file
screen.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/screen.c.o
.PHONY : screen.c.o

screen.i: screen.c.i

.PHONY : screen.i

# target to preprocess a source file
screen.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/screen.c.i
.PHONY : screen.c.i

screen.s: screen.c.s

.PHONY : screen.s

# target to generate assembly for a file
screen.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/screen.c.s
.PHONY : screen.c.s

tty.o: tty.c.o

.PHONY : tty.o

# target to build an object file
tty.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/tty.c.o
.PHONY : tty.c.o

tty.i: tty.c.i

.PHONY : tty.i

# target to preprocess a source file
tty.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/tty.c.i
.PHONY : tty.c.i

tty.s: tty.c.s

.PHONY : tty.s

# target to generate assembly for a file
tty.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/tty.c.s
.PHONY : tty.c.s

xutil.o: xutil.c.o

.PHONY : xutil.o

# target to build an object file
xutil.c.o:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/xutil.c.o
.PHONY : xutil.c.o

xutil.i: xutil.c.i

.PHONY : xutil.i

# target to preprocess a source file
xutil.c.i:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/xutil.c.i
.PHONY : xutil.c.i

xutil.s: xutil.c.s

.PHONY : xutil.s

# target to generate assembly for a file
xutil.c.s:
	$(MAKE) -f CMakeFiles/cmtop.dir/build.make CMakeFiles/cmtop.dir/xutil.c.s
.PHONY : xutil.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... cmtop"
	@echo "... edit_cache"
	@echo "... cmtop.o"
	@echo "... cmtop.i"
	@echo "... cmtop.s"
	@echo "... error.o"
	@echo "... error.i"
	@echo "... error.s"
	@echo "... mtxline.o"
	@echo "... mtxline.i"
	@echo "... mtxline.s"
	@echo "... proc.o"
	@echo "... proc.i"
	@echo "... proc.s"
	@echo "... procbst.o"
	@echo "... procbst.i"
	@echo "... procbst.s"
	@echo "... screen.o"
	@echo "... screen.i"
	@echo "... screen.s"
	@echo "... tty.o"
	@echo "... tty.i"
	@echo "... tty.s"
	@echo "... xutil.o"
	@echo "... xutil.i"
	@echo "... xutil.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/bin/cmake.exe

# The command to remove a file.
RM = /usr/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/g/algorithm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/g/algorithm/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test-socket-server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test-socket-server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test-socket-server.dir/flags.make

CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o: CMakeFiles/test-socket-server.dir/flags.make
CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o: ../test/test-socket-server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/g/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o   -c /cygdrive/g/algorithm/test/test-socket-server.c

CMakeFiles/test-socket-server.dir/test/test-socket-server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test-socket-server.dir/test/test-socket-server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/g/algorithm/test/test-socket-server.c > CMakeFiles/test-socket-server.dir/test/test-socket-server.c.i

CMakeFiles/test-socket-server.dir/test/test-socket-server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test-socket-server.dir/test/test-socket-server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/g/algorithm/test/test-socket-server.c -o CMakeFiles/test-socket-server.dir/test/test-socket-server.c.s

# Object files for target test-socket-server
test__socket__server_OBJECTS = \
"CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o"

# External object files for target test-socket-server
test__socket__server_EXTERNAL_OBJECTS =

test-socket-server.exe: CMakeFiles/test-socket-server.dir/test/test-socket-server.c.o
test-socket-server.exe: CMakeFiles/test-socket-server.dir/build.make
test-socket-server.exe: src/core/libcore.a
test-socket-server.exe: src/algorithm/libstl.a
test-socket-server.exe: CMakeFiles/test-socket-server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/g/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test-socket-server.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-socket-server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test-socket-server.dir/build: test-socket-server.exe

.PHONY : CMakeFiles/test-socket-server.dir/build

CMakeFiles/test-socket-server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test-socket-server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test-socket-server.dir/clean

CMakeFiles/test-socket-server.dir/depend:
	cd /cygdrive/g/algorithm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/g/algorithm /cygdrive/g/algorithm /cygdrive/g/algorithm/cmake-build-debug /cygdrive/g/algorithm/cmake-build-debug /cygdrive/g/algorithm/cmake-build-debug/CMakeFiles/test-socket-server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test-socket-server.dir/depend


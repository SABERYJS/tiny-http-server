# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /cygdrive/c/Users/Administrator/.CLion2019.1/system/cygwin_cmake/bin/cmake.exe

# The command to remove a file.
RM = /cygdrive/c/Users/Administrator/.CLion2019.1/system/cygwin_cmake/bin/cmake.exe -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cygdrive/e/code/algorithm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cygdrive/e/code/algorithm/cmake-build-debug

# Include any dependencies generated for this target.
include src/core/CMakeFiles/core.dir/depend.make

# Include the progress variables for this target.
include src/core/CMakeFiles/core.dir/progress.make

# Include the compile flags for this target's objects.
include src/core/CMakeFiles/core.dir/flags.make

src/core/CMakeFiles/core.dir/buffer.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/buffer.c.o: ../src/core/buffer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/core/CMakeFiles/core.dir/buffer.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/buffer.c.o   -c /cygdrive/e/code/algorithm/src/core/buffer.c

src/core/CMakeFiles/core.dir/buffer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/buffer.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/buffer.c > CMakeFiles/core.dir/buffer.c.i

src/core/CMakeFiles/core.dir/buffer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/buffer.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/buffer.c -o CMakeFiles/core.dir/buffer.c.s

src/core/CMakeFiles/core.dir/client.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/client.c.o: ../src/core/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object src/core/CMakeFiles/core.dir/client.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/client.c.o   -c /cygdrive/e/code/algorithm/src/core/client.c

src/core/CMakeFiles/core.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/client.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/client.c > CMakeFiles/core.dir/client.c.i

src/core/CMakeFiles/core.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/client.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/client.c -o CMakeFiles/core.dir/client.c.s

src/core/CMakeFiles/core.dir/command_line.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/command_line.c.o: ../src/core/command_line.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object src/core/CMakeFiles/core.dir/command_line.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/command_line.c.o   -c /cygdrive/e/code/algorithm/src/core/command_line.c

src/core/CMakeFiles/core.dir/command_line.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/command_line.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/command_line.c > CMakeFiles/core.dir/command_line.c.i

src/core/CMakeFiles/core.dir/command_line.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/command_line.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/command_line.c -o CMakeFiles/core.dir/command_line.c.s

src/core/CMakeFiles/core.dir/event.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/event.c.o: ../src/core/event.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object src/core/CMakeFiles/core.dir/event.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/event.c.o   -c /cygdrive/e/code/algorithm/src/core/event.c

src/core/CMakeFiles/core.dir/event.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/event.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/event.c > CMakeFiles/core.dir/event.c.i

src/core/CMakeFiles/core.dir/event.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/event.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/event.c -o CMakeFiles/core.dir/event.c.s

src/core/CMakeFiles/core.dir/http.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/http.c.o: ../src/core/http.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object src/core/CMakeFiles/core.dir/http.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/http.c.o   -c /cygdrive/e/code/algorithm/src/core/http.c

src/core/CMakeFiles/core.dir/http.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/http.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/http.c > CMakeFiles/core.dir/http.c.i

src/core/CMakeFiles/core.dir/http.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/http.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/http.c -o CMakeFiles/core.dir/http.c.s

src/core/CMakeFiles/core.dir/memory.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/memory.c.o: ../src/core/memory.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object src/core/CMakeFiles/core.dir/memory.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/memory.c.o   -c /cygdrive/e/code/algorithm/src/core/memory.c

src/core/CMakeFiles/core.dir/memory.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/memory.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/memory.c > CMakeFiles/core.dir/memory.c.i

src/core/CMakeFiles/core.dir/memory.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/memory.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/memory.c -o CMakeFiles/core.dir/memory.c.s

src/core/CMakeFiles/core.dir/net.c.o: src/core/CMakeFiles/core.dir/flags.make
src/core/CMakeFiles/core.dir/net.c.o: ../src/core/net.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object src/core/CMakeFiles/core.dir/net.c.o"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/core.dir/net.c.o   -c /cygdrive/e/code/algorithm/src/core/net.c

src/core/CMakeFiles/core.dir/net.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/core.dir/net.c.i"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /cygdrive/e/code/algorithm/src/core/net.c > CMakeFiles/core.dir/net.c.i

src/core/CMakeFiles/core.dir/net.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/core.dir/net.c.s"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /cygdrive/e/code/algorithm/src/core/net.c -o CMakeFiles/core.dir/net.c.s

# Object files for target core
core_OBJECTS = \
"CMakeFiles/core.dir/buffer.c.o" \
"CMakeFiles/core.dir/client.c.o" \
"CMakeFiles/core.dir/command_line.c.o" \
"CMakeFiles/core.dir/event.c.o" \
"CMakeFiles/core.dir/http.c.o" \
"CMakeFiles/core.dir/memory.c.o" \
"CMakeFiles/core.dir/net.c.o"

# External object files for target core
core_EXTERNAL_OBJECTS =

src/core/libcore.a: src/core/CMakeFiles/core.dir/buffer.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/client.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/command_line.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/event.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/http.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/memory.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/net.c.o
src/core/libcore.a: src/core/CMakeFiles/core.dir/build.make
src/core/libcore.a: src/core/CMakeFiles/core.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cygdrive/e/code/algorithm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking C static library libcore.a"
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && $(CMAKE_COMMAND) -P CMakeFiles/core.dir/cmake_clean_target.cmake
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/core.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/core/CMakeFiles/core.dir/build: src/core/libcore.a

.PHONY : src/core/CMakeFiles/core.dir/build

src/core/CMakeFiles/core.dir/clean:
	cd /cygdrive/e/code/algorithm/cmake-build-debug/src/core && $(CMAKE_COMMAND) -P CMakeFiles/core.dir/cmake_clean.cmake
.PHONY : src/core/CMakeFiles/core.dir/clean

src/core/CMakeFiles/core.dir/depend:
	cd /cygdrive/e/code/algorithm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cygdrive/e/code/algorithm /cygdrive/e/code/algorithm/src/core /cygdrive/e/code/algorithm/cmake-build-debug /cygdrive/e/code/algorithm/cmake-build-debug/src/core /cygdrive/e/code/algorithm/cmake-build-debug/src/core/CMakeFiles/core.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/core/CMakeFiles/core.dir/depend

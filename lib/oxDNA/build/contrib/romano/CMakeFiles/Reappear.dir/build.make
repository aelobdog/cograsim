# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/avinash/Downloads/oxDNA

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/avinash/Downloads/oxDNA/build

# Include any dependencies generated for this target.
include contrib/romano/CMakeFiles/Reappear.dir/depend.make

# Include the progress variables for this target.
include contrib/romano/CMakeFiles/Reappear.dir/progress.make

# Include the compile flags for this target's objects.
include contrib/romano/CMakeFiles/Reappear.dir/flags.make

contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o: contrib/romano/CMakeFiles/Reappear.dir/flags.make
contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o: ../contrib/romano/src/Backends/MCMoves/Reappear.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/avinash/Downloads/oxDNA/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o"
	cd /home/avinash/Downloads/oxDNA/build/contrib/romano && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o -c /home/avinash/Downloads/oxDNA/contrib/romano/src/Backends/MCMoves/Reappear.cpp

contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.i"
	cd /home/avinash/Downloads/oxDNA/build/contrib/romano && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/avinash/Downloads/oxDNA/contrib/romano/src/Backends/MCMoves/Reappear.cpp > CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.i

contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.s"
	cd /home/avinash/Downloads/oxDNA/build/contrib/romano && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/avinash/Downloads/oxDNA/contrib/romano/src/Backends/MCMoves/Reappear.cpp -o CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.s

# Object files for target Reappear
Reappear_OBJECTS = \
"CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o"

# External object files for target Reappear
Reappear_EXTERNAL_OBJECTS =

../contrib/romano/Reappear.so: contrib/romano/CMakeFiles/Reappear.dir/src/Backends/MCMoves/Reappear.cpp.o
../contrib/romano/Reappear.so: contrib/romano/CMakeFiles/Reappear.dir/build.make
../contrib/romano/Reappear.so: contrib/romano/CMakeFiles/Reappear.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/avinash/Downloads/oxDNA/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../../../contrib/romano/Reappear.so"
	cd /home/avinash/Downloads/oxDNA/build/contrib/romano && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Reappear.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
contrib/romano/CMakeFiles/Reappear.dir/build: ../contrib/romano/Reappear.so

.PHONY : contrib/romano/CMakeFiles/Reappear.dir/build

contrib/romano/CMakeFiles/Reappear.dir/clean:
	cd /home/avinash/Downloads/oxDNA/build/contrib/romano && $(CMAKE_COMMAND) -P CMakeFiles/Reappear.dir/cmake_clean.cmake
.PHONY : contrib/romano/CMakeFiles/Reappear.dir/clean

contrib/romano/CMakeFiles/Reappear.dir/depend:
	cd /home/avinash/Downloads/oxDNA/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/avinash/Downloads/oxDNA /home/avinash/Downloads/oxDNA/contrib/romano /home/avinash/Downloads/oxDNA/build /home/avinash/Downloads/oxDNA/build/contrib/romano /home/avinash/Downloads/oxDNA/build/contrib/romano/CMakeFiles/Reappear.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : contrib/romano/CMakeFiles/Reappear.dir/depend

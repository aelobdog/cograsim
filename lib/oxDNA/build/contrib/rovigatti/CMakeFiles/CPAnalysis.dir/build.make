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
include contrib/rovigatti/CMakeFiles/CPAnalysis.dir/depend.make

# Include the progress variables for this target.
include contrib/rovigatti/CMakeFiles/CPAnalysis.dir/progress.make

# Include the compile flags for this target's objects.
include contrib/rovigatti/CMakeFiles/CPAnalysis.dir/flags.make

contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o: contrib/rovigatti/CMakeFiles/CPAnalysis.dir/flags.make
contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o: ../contrib/rovigatti/src/Observables/CPAnalysis.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/avinash/Downloads/oxDNA/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o"
	cd /home/avinash/Downloads/oxDNA/build/contrib/rovigatti && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o -c /home/avinash/Downloads/oxDNA/contrib/rovigatti/src/Observables/CPAnalysis.cpp

contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.i"
	cd /home/avinash/Downloads/oxDNA/build/contrib/rovigatti && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/avinash/Downloads/oxDNA/contrib/rovigatti/src/Observables/CPAnalysis.cpp > CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.i

contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.s"
	cd /home/avinash/Downloads/oxDNA/build/contrib/rovigatti && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/avinash/Downloads/oxDNA/contrib/rovigatti/src/Observables/CPAnalysis.cpp -o CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.s

# Object files for target CPAnalysis
CPAnalysis_OBJECTS = \
"CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o"

# External object files for target CPAnalysis
CPAnalysis_EXTERNAL_OBJECTS =

../contrib/rovigatti/CPAnalysis.so: contrib/rovigatti/CMakeFiles/CPAnalysis.dir/src/Observables/CPAnalysis.cpp.o
../contrib/rovigatti/CPAnalysis.so: contrib/rovigatti/CMakeFiles/CPAnalysis.dir/build.make
../contrib/rovigatti/CPAnalysis.so: contrib/rovigatti/CMakeFiles/CPAnalysis.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/avinash/Downloads/oxDNA/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../../../contrib/rovigatti/CPAnalysis.so"
	cd /home/avinash/Downloads/oxDNA/build/contrib/rovigatti && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CPAnalysis.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
contrib/rovigatti/CMakeFiles/CPAnalysis.dir/build: ../contrib/rovigatti/CPAnalysis.so

.PHONY : contrib/rovigatti/CMakeFiles/CPAnalysis.dir/build

contrib/rovigatti/CMakeFiles/CPAnalysis.dir/clean:
	cd /home/avinash/Downloads/oxDNA/build/contrib/rovigatti && $(CMAKE_COMMAND) -P CMakeFiles/CPAnalysis.dir/cmake_clean.cmake
.PHONY : contrib/rovigatti/CMakeFiles/CPAnalysis.dir/clean

contrib/rovigatti/CMakeFiles/CPAnalysis.dir/depend:
	cd /home/avinash/Downloads/oxDNA/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/avinash/Downloads/oxDNA /home/avinash/Downloads/oxDNA/contrib/rovigatti /home/avinash/Downloads/oxDNA/build /home/avinash/Downloads/oxDNA/build/contrib/rovigatti /home/avinash/Downloads/oxDNA/build/contrib/rovigatti/CMakeFiles/CPAnalysis.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : contrib/rovigatti/CMakeFiles/CPAnalysis.dir/depend


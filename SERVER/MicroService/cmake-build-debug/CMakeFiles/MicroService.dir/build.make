# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/olga/Projekte/Code/RNVS/MicroService

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/MicroService.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/MicroService.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/MicroService.dir/flags.make

CMakeFiles/MicroService.dir/src/MicroServer.c.o: CMakeFiles/MicroService.dir/flags.make
CMakeFiles/MicroService.dir/src/MicroServer.c.o: ../src/MicroServer.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/MicroService.dir/src/MicroServer.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/MicroService.dir/src/MicroServer.c.o   -c /Users/olga/Projekte/Code/RNVS/MicroService/src/MicroServer.c

CMakeFiles/MicroService.dir/src/MicroServer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/MicroService.dir/src/MicroServer.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/olga/Projekte/Code/RNVS/MicroService/src/MicroServer.c > CMakeFiles/MicroService.dir/src/MicroServer.c.i

CMakeFiles/MicroService.dir/src/MicroServer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/MicroService.dir/src/MicroServer.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/olga/Projekte/Code/RNVS/MicroService/src/MicroServer.c -o CMakeFiles/MicroService.dir/src/MicroServer.c.s

# Object files for target MicroService
MicroService_OBJECTS = \
"CMakeFiles/MicroService.dir/src/MicroServer.c.o"

# External object files for target MicroService
MicroService_EXTERNAL_OBJECTS =

MicroService: CMakeFiles/MicroService.dir/src/MicroServer.c.o
MicroService: CMakeFiles/MicroService.dir/build.make
MicroService: CMakeFiles/MicroService.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable MicroService"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MicroService.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/MicroService.dir/build: MicroService

.PHONY : CMakeFiles/MicroService.dir/build

CMakeFiles/MicroService.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/MicroService.dir/cmake_clean.cmake
.PHONY : CMakeFiles/MicroService.dir/clean

CMakeFiles/MicroService.dir/depend:
	cd /Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/olga/Projekte/Code/RNVS/MicroService /Users/olga/Projekte/Code/RNVS/MicroService /Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug /Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug /Users/olga/Projekte/Code/RNVS/MicroService/cmake-build-debug/CMakeFiles/MicroService.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/MicroService.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.24.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.24.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/claireliu/Developer/Databases/p1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/claireliu/Developer/Databases/p1/build

# Include any dependencies generated for this target.
include CMakeFiles/database_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/database_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/database_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/database_test.dir/flags.make

CMakeFiles/database_test.dir/database_test.cpp.o: CMakeFiles/database_test.dir/flags.make
CMakeFiles/database_test.dir/database_test.cpp.o: /Users/claireliu/Developer/Databases/p1/database_test.cpp
CMakeFiles/database_test.dir/database_test.cpp.o: CMakeFiles/database_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/claireliu/Developer/Databases/p1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/database_test.dir/database_test.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/database_test.dir/database_test.cpp.o -MF CMakeFiles/database_test.dir/database_test.cpp.o.d -o CMakeFiles/database_test.dir/database_test.cpp.o -c /Users/claireliu/Developer/Databases/p1/database_test.cpp

CMakeFiles/database_test.dir/database_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/database_test.dir/database_test.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/claireliu/Developer/Databases/p1/database_test.cpp > CMakeFiles/database_test.dir/database_test.cpp.i

CMakeFiles/database_test.dir/database_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/database_test.dir/database_test.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/claireliu/Developer/Databases/p1/database_test.cpp -o CMakeFiles/database_test.dir/database_test.cpp.s

CMakeFiles/database_test.dir/Database.cpp.o: CMakeFiles/database_test.dir/flags.make
CMakeFiles/database_test.dir/Database.cpp.o: /Users/claireliu/Developer/Databases/p1/Database.cpp
CMakeFiles/database_test.dir/Database.cpp.o: CMakeFiles/database_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/claireliu/Developer/Databases/p1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/database_test.dir/Database.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/database_test.dir/Database.cpp.o -MF CMakeFiles/database_test.dir/Database.cpp.o.d -o CMakeFiles/database_test.dir/Database.cpp.o -c /Users/claireliu/Developer/Databases/p1/Database.cpp

CMakeFiles/database_test.dir/Database.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/database_test.dir/Database.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/claireliu/Developer/Databases/p1/Database.cpp > CMakeFiles/database_test.dir/Database.cpp.i

CMakeFiles/database_test.dir/Database.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/database_test.dir/Database.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/claireliu/Developer/Databases/p1/Database.cpp -o CMakeFiles/database_test.dir/Database.cpp.s

# Object files for target database_test
database_test_OBJECTS = \
"CMakeFiles/database_test.dir/database_test.cpp.o" \
"CMakeFiles/database_test.dir/Database.cpp.o"

# External object files for target database_test
database_test_EXTERNAL_OBJECTS =

database_test: CMakeFiles/database_test.dir/database_test.cpp.o
database_test: CMakeFiles/database_test.dir/Database.cpp.o
database_test: CMakeFiles/database_test.dir/build.make
database_test: lib/libgtest_main.a
database_test: lib/libgtest.a
database_test: CMakeFiles/database_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/claireliu/Developer/Databases/p1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable database_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/database_test.dir/link.txt --verbose=$(VERBOSE)
	/usr/local/Cellar/cmake/3.24.2/bin/cmake -D TEST_TARGET=database_test -D TEST_EXECUTABLE=/Users/claireliu/Developer/Databases/p1/build/database_test -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/Users/claireliu/Developer/Databases/p1/build -D TEST_EXTRA_ARGS= -D TEST_PROPERTIES= -D TEST_PREFIX= -D TEST_SUFFIX= -D TEST_FILTER= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=database_test_TESTS -D CTEST_FILE=/Users/claireliu/Developer/Databases/p1/build/database_test[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -D TEST_XML_OUTPUT_DIR= -P /usr/local/Cellar/cmake/3.24.2/share/cmake/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
CMakeFiles/database_test.dir/build: database_test
.PHONY : CMakeFiles/database_test.dir/build

CMakeFiles/database_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/database_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/database_test.dir/clean

CMakeFiles/database_test.dir/depend:
	cd /Users/claireliu/Developer/Databases/p1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/claireliu/Developer/Databases/p1 /Users/claireliu/Developer/Databases/p1 /Users/claireliu/Developer/Databases/p1/build /Users/claireliu/Developer/Databases/p1/build /Users/claireliu/Developer/Databases/p1/build/CMakeFiles/database_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/database_test.dir/depend


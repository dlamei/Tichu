# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp

# Include any dependencies generated for this target.
include CMakeFiles/sockpp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sockpp.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sockpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sockpp.dir/flags.make

# Object files for target sockpp
sockpp_OBJECTS =

# External object files for target sockpp
sockpp_EXTERNAL_OBJECTS = \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/acceptor.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/connector.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/datagram_socket.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/exception.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/inet_address.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/inet6_address.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/socket.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/stream_socket.cpp.o" \
"/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/src/CMakeFiles/sockpp-objs.dir/unix/unix_address.cpp.o"

libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/acceptor.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/connector.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/datagram_socket.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/exception.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/inet_address.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/inet6_address.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/socket.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/stream_socket.cpp.o
libsockpp.so.0.7.0: src/CMakeFiles/sockpp-objs.dir/unix/unix_address.cpp.o
libsockpp.so.0.7.0: CMakeFiles/sockpp.dir/build.make
libsockpp.so.0.7.0: CMakeFiles/sockpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX shared library libsockpp.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sockpp.dir/link.txt --verbose=$(VERBOSE)
	$(CMAKE_COMMAND) -E cmake_symlink_library libsockpp.so.0.7.0 libsockpp.so.0 libsockpp.so

libsockpp.so.0: libsockpp.so.0.7.0
	@$(CMAKE_COMMAND) -E touch_nocreate libsockpp.so.0

libsockpp.so: libsockpp.so.0.7.0
	@$(CMAKE_COMMAND) -E touch_nocreate libsockpp.so

# Rule to build all files generated by this target.
CMakeFiles/sockpp.dir/build: libsockpp.so
.PHONY : CMakeFiles/sockpp.dir/build

CMakeFiles/sockpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sockpp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sockpp.dir/clean

CMakeFiles/sockpp.dir/depend:
	cd /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp /home/everywherdo/Software_Engeneering/cse-tichu-example-project/sockpp/CMakeFiles/sockpp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sockpp.dir/depend

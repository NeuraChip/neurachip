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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ktb/scratch/src/DRAMsim3-1.0.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ktb/scratch/src/DRAMsim3-1.0.0/build

# Include any dependencies generated for this target.
include CMakeFiles/dramsim3.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/dramsim3.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/dramsim3.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dramsim3.dir/flags.make

CMakeFiles/dramsim3.dir/src/bankstate.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/bankstate.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/bankstate.cc
CMakeFiles/dramsim3.dir/src/bankstate.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dramsim3.dir/src/bankstate.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/bankstate.cc.o -MF CMakeFiles/dramsim3.dir/src/bankstate.cc.o.d -o CMakeFiles/dramsim3.dir/src/bankstate.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/bankstate.cc

CMakeFiles/dramsim3.dir/src/bankstate.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/bankstate.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/bankstate.cc > CMakeFiles/dramsim3.dir/src/bankstate.cc.i

CMakeFiles/dramsim3.dir/src/bankstate.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/bankstate.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/bankstate.cc -o CMakeFiles/dramsim3.dir/src/bankstate.cc.s

CMakeFiles/dramsim3.dir/src/channel_state.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/channel_state.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/channel_state.cc
CMakeFiles/dramsim3.dir/src/channel_state.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/dramsim3.dir/src/channel_state.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/channel_state.cc.o -MF CMakeFiles/dramsim3.dir/src/channel_state.cc.o.d -o CMakeFiles/dramsim3.dir/src/channel_state.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/channel_state.cc

CMakeFiles/dramsim3.dir/src/channel_state.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/channel_state.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/channel_state.cc > CMakeFiles/dramsim3.dir/src/channel_state.cc.i

CMakeFiles/dramsim3.dir/src/channel_state.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/channel_state.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/channel_state.cc -o CMakeFiles/dramsim3.dir/src/channel_state.cc.s

CMakeFiles/dramsim3.dir/src/command_queue.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/command_queue.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/command_queue.cc
CMakeFiles/dramsim3.dir/src/command_queue.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/dramsim3.dir/src/command_queue.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/command_queue.cc.o -MF CMakeFiles/dramsim3.dir/src/command_queue.cc.o.d -o CMakeFiles/dramsim3.dir/src/command_queue.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/command_queue.cc

CMakeFiles/dramsim3.dir/src/command_queue.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/command_queue.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/command_queue.cc > CMakeFiles/dramsim3.dir/src/command_queue.cc.i

CMakeFiles/dramsim3.dir/src/command_queue.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/command_queue.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/command_queue.cc -o CMakeFiles/dramsim3.dir/src/command_queue.cc.s

CMakeFiles/dramsim3.dir/src/common.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/common.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/common.cc
CMakeFiles/dramsim3.dir/src/common.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/dramsim3.dir/src/common.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/common.cc.o -MF CMakeFiles/dramsim3.dir/src/common.cc.o.d -o CMakeFiles/dramsim3.dir/src/common.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/common.cc

CMakeFiles/dramsim3.dir/src/common.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/common.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/common.cc > CMakeFiles/dramsim3.dir/src/common.cc.i

CMakeFiles/dramsim3.dir/src/common.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/common.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/common.cc -o CMakeFiles/dramsim3.dir/src/common.cc.s

CMakeFiles/dramsim3.dir/src/configuration.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/configuration.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/configuration.cc
CMakeFiles/dramsim3.dir/src/configuration.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/dramsim3.dir/src/configuration.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/configuration.cc.o -MF CMakeFiles/dramsim3.dir/src/configuration.cc.o.d -o CMakeFiles/dramsim3.dir/src/configuration.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/configuration.cc

CMakeFiles/dramsim3.dir/src/configuration.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/configuration.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/configuration.cc > CMakeFiles/dramsim3.dir/src/configuration.cc.i

CMakeFiles/dramsim3.dir/src/configuration.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/configuration.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/configuration.cc -o CMakeFiles/dramsim3.dir/src/configuration.cc.s

CMakeFiles/dramsim3.dir/src/controller.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/controller.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/controller.cc
CMakeFiles/dramsim3.dir/src/controller.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/dramsim3.dir/src/controller.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/controller.cc.o -MF CMakeFiles/dramsim3.dir/src/controller.cc.o.d -o CMakeFiles/dramsim3.dir/src/controller.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/controller.cc

CMakeFiles/dramsim3.dir/src/controller.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/controller.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/controller.cc > CMakeFiles/dramsim3.dir/src/controller.cc.i

CMakeFiles/dramsim3.dir/src/controller.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/controller.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/controller.cc -o CMakeFiles/dramsim3.dir/src/controller.cc.s

CMakeFiles/dramsim3.dir/src/dram_system.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/dram_system.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/dram_system.cc
CMakeFiles/dramsim3.dir/src/dram_system.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/dramsim3.dir/src/dram_system.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/dram_system.cc.o -MF CMakeFiles/dramsim3.dir/src/dram_system.cc.o.d -o CMakeFiles/dramsim3.dir/src/dram_system.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/dram_system.cc

CMakeFiles/dramsim3.dir/src/dram_system.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/dram_system.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/dram_system.cc > CMakeFiles/dramsim3.dir/src/dram_system.cc.i

CMakeFiles/dramsim3.dir/src/dram_system.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/dram_system.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/dram_system.cc -o CMakeFiles/dramsim3.dir/src/dram_system.cc.s

CMakeFiles/dramsim3.dir/src/hmc.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/hmc.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/hmc.cc
CMakeFiles/dramsim3.dir/src/hmc.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/dramsim3.dir/src/hmc.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/hmc.cc.o -MF CMakeFiles/dramsim3.dir/src/hmc.cc.o.d -o CMakeFiles/dramsim3.dir/src/hmc.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/hmc.cc

CMakeFiles/dramsim3.dir/src/hmc.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/hmc.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/hmc.cc > CMakeFiles/dramsim3.dir/src/hmc.cc.i

CMakeFiles/dramsim3.dir/src/hmc.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/hmc.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/hmc.cc -o CMakeFiles/dramsim3.dir/src/hmc.cc.s

CMakeFiles/dramsim3.dir/src/refresh.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/refresh.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/refresh.cc
CMakeFiles/dramsim3.dir/src/refresh.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/dramsim3.dir/src/refresh.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/refresh.cc.o -MF CMakeFiles/dramsim3.dir/src/refresh.cc.o.d -o CMakeFiles/dramsim3.dir/src/refresh.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/refresh.cc

CMakeFiles/dramsim3.dir/src/refresh.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/refresh.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/refresh.cc > CMakeFiles/dramsim3.dir/src/refresh.cc.i

CMakeFiles/dramsim3.dir/src/refresh.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/refresh.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/refresh.cc -o CMakeFiles/dramsim3.dir/src/refresh.cc.s

CMakeFiles/dramsim3.dir/src/simple_stats.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/simple_stats.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/simple_stats.cc
CMakeFiles/dramsim3.dir/src/simple_stats.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/dramsim3.dir/src/simple_stats.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/simple_stats.cc.o -MF CMakeFiles/dramsim3.dir/src/simple_stats.cc.o.d -o CMakeFiles/dramsim3.dir/src/simple_stats.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/simple_stats.cc

CMakeFiles/dramsim3.dir/src/simple_stats.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/simple_stats.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/simple_stats.cc > CMakeFiles/dramsim3.dir/src/simple_stats.cc.i

CMakeFiles/dramsim3.dir/src/simple_stats.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/simple_stats.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/simple_stats.cc -o CMakeFiles/dramsim3.dir/src/simple_stats.cc.s

CMakeFiles/dramsim3.dir/src/timing.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/timing.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/timing.cc
CMakeFiles/dramsim3.dir/src/timing.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/dramsim3.dir/src/timing.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/timing.cc.o -MF CMakeFiles/dramsim3.dir/src/timing.cc.o.d -o CMakeFiles/dramsim3.dir/src/timing.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/timing.cc

CMakeFiles/dramsim3.dir/src/timing.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/timing.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/timing.cc > CMakeFiles/dramsim3.dir/src/timing.cc.i

CMakeFiles/dramsim3.dir/src/timing.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/timing.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/timing.cc -o CMakeFiles/dramsim3.dir/src/timing.cc.s

CMakeFiles/dramsim3.dir/src/memory_system.cc.o: CMakeFiles/dramsim3.dir/flags.make
CMakeFiles/dramsim3.dir/src/memory_system.cc.o: /home/ktb/scratch/src/DRAMsim3-1.0.0/src/memory_system.cc
CMakeFiles/dramsim3.dir/src/memory_system.cc.o: CMakeFiles/dramsim3.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/dramsim3.dir/src/memory_system.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dramsim3.dir/src/memory_system.cc.o -MF CMakeFiles/dramsim3.dir/src/memory_system.cc.o.d -o CMakeFiles/dramsim3.dir/src/memory_system.cc.o -c /home/ktb/scratch/src/DRAMsim3-1.0.0/src/memory_system.cc

CMakeFiles/dramsim3.dir/src/memory_system.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dramsim3.dir/src/memory_system.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ktb/scratch/src/DRAMsim3-1.0.0/src/memory_system.cc > CMakeFiles/dramsim3.dir/src/memory_system.cc.i

CMakeFiles/dramsim3.dir/src/memory_system.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dramsim3.dir/src/memory_system.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ktb/scratch/src/DRAMsim3-1.0.0/src/memory_system.cc -o CMakeFiles/dramsim3.dir/src/memory_system.cc.s

# Object files for target dramsim3
dramsim3_OBJECTS = \
"CMakeFiles/dramsim3.dir/src/bankstate.cc.o" \
"CMakeFiles/dramsim3.dir/src/channel_state.cc.o" \
"CMakeFiles/dramsim3.dir/src/command_queue.cc.o" \
"CMakeFiles/dramsim3.dir/src/common.cc.o" \
"CMakeFiles/dramsim3.dir/src/configuration.cc.o" \
"CMakeFiles/dramsim3.dir/src/controller.cc.o" \
"CMakeFiles/dramsim3.dir/src/dram_system.cc.o" \
"CMakeFiles/dramsim3.dir/src/hmc.cc.o" \
"CMakeFiles/dramsim3.dir/src/refresh.cc.o" \
"CMakeFiles/dramsim3.dir/src/simple_stats.cc.o" \
"CMakeFiles/dramsim3.dir/src/timing.cc.o" \
"CMakeFiles/dramsim3.dir/src/memory_system.cc.o"

# External object files for target dramsim3
dramsim3_EXTERNAL_OBJECTS =

/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/bankstate.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/channel_state.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/command_queue.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/common.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/configuration.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/controller.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/dram_system.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/hmc.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/refresh.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/simple_stats.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/timing.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/src/memory_system.cc.o
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/build.make
/home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so: CMakeFiles/dramsim3.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking CXX shared library /home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dramsim3.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dramsim3.dir/build: /home/ktb/scratch/src/DRAMsim3-1.0.0/libdramsim3.so
.PHONY : CMakeFiles/dramsim3.dir/build

CMakeFiles/dramsim3.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dramsim3.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dramsim3.dir/clean

CMakeFiles/dramsim3.dir/depend:
	cd /home/ktb/scratch/src/DRAMsim3-1.0.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ktb/scratch/src/DRAMsim3-1.0.0 /home/ktb/scratch/src/DRAMsim3-1.0.0 /home/ktb/scratch/src/DRAMsim3-1.0.0/build /home/ktb/scratch/src/DRAMsim3-1.0.0/build /home/ktb/scratch/src/DRAMsim3-1.0.0/build/CMakeFiles/dramsim3.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dramsim3.dir/depend


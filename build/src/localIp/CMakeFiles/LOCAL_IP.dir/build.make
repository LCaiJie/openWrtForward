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
CMAKE_SOURCE_DIR = /home/caijie/project/OpenWrt

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/caijie/project/OpenWrt/build

# Include any dependencies generated for this target.
include src/localIp/CMakeFiles/LOCAL_IP.dir/depend.make

# Include the progress variables for this target.
include src/localIp/CMakeFiles/LOCAL_IP.dir/progress.make

# Include the compile flags for this target's objects.
include src/localIp/CMakeFiles/LOCAL_IP.dir/flags.make

src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.o: src/localIp/CMakeFiles/LOCAL_IP.dir/flags.make
src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.o: ../src/localIp/localIp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/caijie/project/OpenWrt/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.o"
	cd /home/caijie/project/OpenWrt/build/src/localIp && /home/caijie/arm/lede-toolchain-ar71xx-generic_gcc-5.4.0_musl.Linux-x86_64/toolchain-mips_24kc_gcc-5.4.0_musl/bin/mips-openwrt-linux-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/LOCAL_IP.dir/localIp.c.o   -c /home/caijie/project/OpenWrt/src/localIp/localIp.c

src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/LOCAL_IP.dir/localIp.c.i"
	cd /home/caijie/project/OpenWrt/build/src/localIp && /home/caijie/arm/lede-toolchain-ar71xx-generic_gcc-5.4.0_musl.Linux-x86_64/toolchain-mips_24kc_gcc-5.4.0_musl/bin/mips-openwrt-linux-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/caijie/project/OpenWrt/src/localIp/localIp.c > CMakeFiles/LOCAL_IP.dir/localIp.c.i

src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/LOCAL_IP.dir/localIp.c.s"
	cd /home/caijie/project/OpenWrt/build/src/localIp && /home/caijie/arm/lede-toolchain-ar71xx-generic_gcc-5.4.0_musl.Linux-x86_64/toolchain-mips_24kc_gcc-5.4.0_musl/bin/mips-openwrt-linux-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/caijie/project/OpenWrt/src/localIp/localIp.c -o CMakeFiles/LOCAL_IP.dir/localIp.c.s

# Object files for target LOCAL_IP
LOCAL_IP_OBJECTS = \
"CMakeFiles/LOCAL_IP.dir/localIp.c.o"

# External object files for target LOCAL_IP
LOCAL_IP_EXTERNAL_OBJECTS =

src/localIp/libLOCAL_IP.a: src/localIp/CMakeFiles/LOCAL_IP.dir/localIp.c.o
src/localIp/libLOCAL_IP.a: src/localIp/CMakeFiles/LOCAL_IP.dir/build.make
src/localIp/libLOCAL_IP.a: src/localIp/CMakeFiles/LOCAL_IP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/caijie/project/OpenWrt/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libLOCAL_IP.a"
	cd /home/caijie/project/OpenWrt/build/src/localIp && $(CMAKE_COMMAND) -P CMakeFiles/LOCAL_IP.dir/cmake_clean_target.cmake
	cd /home/caijie/project/OpenWrt/build/src/localIp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LOCAL_IP.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/localIp/CMakeFiles/LOCAL_IP.dir/build: src/localIp/libLOCAL_IP.a

.PHONY : src/localIp/CMakeFiles/LOCAL_IP.dir/build

src/localIp/CMakeFiles/LOCAL_IP.dir/clean:
	cd /home/caijie/project/OpenWrt/build/src/localIp && $(CMAKE_COMMAND) -P CMakeFiles/LOCAL_IP.dir/cmake_clean.cmake
.PHONY : src/localIp/CMakeFiles/LOCAL_IP.dir/clean

src/localIp/CMakeFiles/LOCAL_IP.dir/depend:
	cd /home/caijie/project/OpenWrt/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/caijie/project/OpenWrt /home/caijie/project/OpenWrt/src/localIp /home/caijie/project/OpenWrt/build /home/caijie/project/OpenWrt/build/src/localIp /home/caijie/project/OpenWrt/build/src/localIp/CMakeFiles/LOCAL_IP.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/localIp/CMakeFiles/LOCAL_IP.dir/depend


# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2024.2.2(2)\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2024.2.2(2)\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug"

# Include any dependencies generated for this target.
include external/glad-3.3/CMakeFiles/glad.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include external/glad-3.3/CMakeFiles/glad.dir/compiler_depend.make

# Include the progress variables for this target.
include external/glad-3.3/CMakeFiles/glad.dir/progress.make

# Include the compile flags for this target's objects.
include external/glad-3.3/CMakeFiles/glad.dir/flags.make

external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj: external/glad-3.3/CMakeFiles/glad.dir/flags.make
external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj: external/glad-3.3/CMakeFiles/glad.dir/includes_C.rsp
external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj: C:/Users/Chris/Desktop/CG\ Project/From\ Github/CG_Project/lab2/lab2/external/glad-3.3/src/gl.c
external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj: external/glad-3.3/CMakeFiles/glad.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj"
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj -MF CMakeFiles\glad.dir\src\gl.c.obj.d -o CMakeFiles\glad.dir\src\gl.c.obj -c "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\external\glad-3.3\src\gl.c"

external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/glad.dir/src/gl.c.i"
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\external\glad-3.3\src\gl.c" > CMakeFiles\glad.dir\src\gl.c.i

external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/glad.dir/src/gl.c.s"
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && C:\msys64\mingw64\bin\cc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\external\glad-3.3\src\gl.c" -o CMakeFiles\glad.dir\src\gl.c.s

# Object files for target glad
glad_OBJECTS = \
"CMakeFiles/glad.dir/src/gl.c.obj"

# External object files for target glad
glad_EXTERNAL_OBJECTS =

libglad.a: external/glad-3.3/CMakeFiles/glad.dir/src/gl.c.obj
libglad.a: external/glad-3.3/CMakeFiles/glad.dir/build.make
libglad.a: external/glad-3.3/CMakeFiles/glad.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library ..\..\libglad.a"
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && $(CMAKE_COMMAND) -P CMakeFiles\glad.dir\cmake_clean_target.cmake
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\glad.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/glad-3.3/CMakeFiles/glad.dir/build: libglad.a
.PHONY : external/glad-3.3/CMakeFiles/glad.dir/build

external/glad-3.3/CMakeFiles/glad.dir/clean:
	cd /d C:\Users\Chris\Desktop\CGPROJ~1\FROMGI~1\CG_PRO~1\lab2\lab2\CMAKE-~1\external\glad-3.3 && $(CMAKE_COMMAND) -P CMakeFiles\glad.dir\cmake_clean.cmake
.PHONY : external/glad-3.3/CMakeFiles/glad.dir/clean

external/glad-3.3/CMakeFiles/glad.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2" "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\external\glad-3.3" "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug" "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug\external\glad-3.3" "C:\Users\Chris\Desktop\CG Project\From Github\CG_Project\lab2\lab2\cmake-build-debug\external\glad-3.3\CMakeFiles\glad.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : external/glad-3.3/CMakeFiles/glad.dir/depend


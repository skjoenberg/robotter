# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /home/sebastian/robot/Exam

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sebastian/robot/Exam/build

# Include any dependencies generated for this target.
include CMakeFiles/exercise5.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/exercise5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/exercise5.dir/flags.make

CMakeFiles/exercise5.dir/exercise5.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/exercise5.cc.o: ../exercise5.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/exercise5.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/exercise5.cc.o -c /home/sebastian/robot/Exam/exercise5.cc

CMakeFiles/exercise5.dir/exercise5.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/exercise5.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/exercise5.cc > CMakeFiles/exercise5.dir/exercise5.cc.i

CMakeFiles/exercise5.dir/exercise5.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/exercise5.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/exercise5.cc -o CMakeFiles/exercise5.dir/exercise5.cc.s

CMakeFiles/exercise5.dir/exercise5.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/exercise5.cc.o.requires

CMakeFiles/exercise5.dir/exercise5.cc.o.provides: CMakeFiles/exercise5.dir/exercise5.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/exercise5.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/exercise5.cc.o.provides

CMakeFiles/exercise5.dir/exercise5.cc.o.provides.build: CMakeFiles/exercise5.dir/exercise5.cc.o

CMakeFiles/exercise5.dir/camera.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/camera.cc.o: ../camera.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/camera.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/camera.cc.o -c /home/sebastian/robot/Exam/camera.cc

CMakeFiles/exercise5.dir/camera.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/camera.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/camera.cc > CMakeFiles/exercise5.dir/camera.cc.i

CMakeFiles/exercise5.dir/camera.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/camera.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/camera.cc -o CMakeFiles/exercise5.dir/camera.cc.s

CMakeFiles/exercise5.dir/camera.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/camera.cc.o.requires

CMakeFiles/exercise5.dir/camera.cc.o.provides: CMakeFiles/exercise5.dir/camera.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/camera.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/camera.cc.o.provides

CMakeFiles/exercise5.dir/camera.cc.o.provides.build: CMakeFiles/exercise5.dir/camera.cc.o

CMakeFiles/exercise5.dir/particles.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/particles.cc.o: ../particles.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/particles.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/particles.cc.o -c /home/sebastian/robot/Exam/particles.cc

CMakeFiles/exercise5.dir/particles.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/particles.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/particles.cc > CMakeFiles/exercise5.dir/particles.cc.i

CMakeFiles/exercise5.dir/particles.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/particles.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/particles.cc -o CMakeFiles/exercise5.dir/particles.cc.s

CMakeFiles/exercise5.dir/particles.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/particles.cc.o.requires

CMakeFiles/exercise5.dir/particles.cc.o.provides: CMakeFiles/exercise5.dir/particles.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/particles.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/particles.cc.o.provides

CMakeFiles/exercise5.dir/particles.cc.o.provides.build: CMakeFiles/exercise5.dir/particles.cc.o

CMakeFiles/exercise5.dir/random_numbers.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/random_numbers.cc.o: ../random_numbers.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/random_numbers.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/random_numbers.cc.o -c /home/sebastian/robot/Exam/random_numbers.cc

CMakeFiles/exercise5.dir/random_numbers.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/random_numbers.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/random_numbers.cc > CMakeFiles/exercise5.dir/random_numbers.cc.i

CMakeFiles/exercise5.dir/random_numbers.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/random_numbers.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/random_numbers.cc -o CMakeFiles/exercise5.dir/random_numbers.cc.s

CMakeFiles/exercise5.dir/random_numbers.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/random_numbers.cc.o.requires

CMakeFiles/exercise5.dir/random_numbers.cc.o.provides: CMakeFiles/exercise5.dir/random_numbers.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/random_numbers.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/random_numbers.cc.o.provides

CMakeFiles/exercise5.dir/random_numbers.cc.o.provides.build: CMakeFiles/exercise5.dir/random_numbers.cc.o

CMakeFiles/exercise5.dir/resampling.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/resampling.cc.o: ../resampling.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/resampling.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/resampling.cc.o -c /home/sebastian/robot/Exam/resampling.cc

CMakeFiles/exercise5.dir/resampling.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/resampling.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/resampling.cc > CMakeFiles/exercise5.dir/resampling.cc.i

CMakeFiles/exercise5.dir/resampling.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/resampling.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/resampling.cc -o CMakeFiles/exercise5.dir/resampling.cc.s

CMakeFiles/exercise5.dir/resampling.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/resampling.cc.o.requires

CMakeFiles/exercise5.dir/resampling.cc.o.provides: CMakeFiles/exercise5.dir/resampling.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/resampling.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/resampling.cc.o.provides

CMakeFiles/exercise5.dir/resampling.cc.o.provides.build: CMakeFiles/exercise5.dir/resampling.cc.o

CMakeFiles/exercise5.dir/robot.cc.o: CMakeFiles/exercise5.dir/flags.make
CMakeFiles/exercise5.dir/robot.cc.o: ../robot.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/sebastian/robot/Exam/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/exercise5.dir/robot.cc.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/exercise5.dir/robot.cc.o -c /home/sebastian/robot/Exam/robot.cc

CMakeFiles/exercise5.dir/robot.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/exercise5.dir/robot.cc.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/sebastian/robot/Exam/robot.cc > CMakeFiles/exercise5.dir/robot.cc.i

CMakeFiles/exercise5.dir/robot.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/exercise5.dir/robot.cc.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/sebastian/robot/Exam/robot.cc -o CMakeFiles/exercise5.dir/robot.cc.s

CMakeFiles/exercise5.dir/robot.cc.o.requires:
.PHONY : CMakeFiles/exercise5.dir/robot.cc.o.requires

CMakeFiles/exercise5.dir/robot.cc.o.provides: CMakeFiles/exercise5.dir/robot.cc.o.requires
	$(MAKE) -f CMakeFiles/exercise5.dir/build.make CMakeFiles/exercise5.dir/robot.cc.o.provides.build
.PHONY : CMakeFiles/exercise5.dir/robot.cc.o.provides

CMakeFiles/exercise5.dir/robot.cc.o.provides.build: CMakeFiles/exercise5.dir/robot.cc.o

# Object files for target exercise5
exercise5_OBJECTS = \
"CMakeFiles/exercise5.dir/exercise5.cc.o" \
"CMakeFiles/exercise5.dir/camera.cc.o" \
"CMakeFiles/exercise5.dir/particles.cc.o" \
"CMakeFiles/exercise5.dir/random_numbers.cc.o" \
"CMakeFiles/exercise5.dir/resampling.cc.o" \
"CMakeFiles/exercise5.dir/robot.cc.o"

# External object files for target exercise5
exercise5_EXTERNAL_OBJECTS =

exercise5: CMakeFiles/exercise5.dir/exercise5.cc.o
exercise5: CMakeFiles/exercise5.dir/camera.cc.o
exercise5: CMakeFiles/exercise5.dir/particles.cc.o
exercise5: CMakeFiles/exercise5.dir/random_numbers.cc.o
exercise5: CMakeFiles/exercise5.dir/resampling.cc.o
exercise5: CMakeFiles/exercise5.dir/robot.cc.o
exercise5: CMakeFiles/exercise5.dir/build.make
exercise5: /usr/local/lib/libopencv_videostab.so.2.4.11
exercise5: /usr/local/lib/libopencv_video.so.2.4.11
exercise5: /usr/local/lib/libopencv_ts.a
exercise5: /usr/local/lib/libopencv_superres.so.2.4.11
exercise5: /usr/local/lib/libopencv_stitching.so.2.4.11
exercise5: /usr/local/lib/libopencv_photo.so.2.4.11
exercise5: /usr/local/lib/libopencv_ocl.so.2.4.11
exercise5: /usr/local/lib/libopencv_objdetect.so.2.4.11
exercise5: /usr/local/lib/libopencv_nonfree.so.2.4.11
exercise5: /usr/local/lib/libopencv_ml.so.2.4.11
exercise5: /usr/local/lib/libopencv_legacy.so.2.4.11
exercise5: /usr/local/lib/libopencv_imgproc.so.2.4.11
exercise5: /usr/local/lib/libopencv_highgui.so.2.4.11
exercise5: /usr/local/lib/libopencv_gpu.so.2.4.11
exercise5: /usr/local/lib/libopencv_flann.so.2.4.11
exercise5: /usr/local/lib/libopencv_features2d.so.2.4.11
exercise5: /usr/local/lib/libopencv_core.so.2.4.11
exercise5: /usr/local/lib/libopencv_contrib.so.2.4.11
exercise5: /usr/local/lib/libopencv_calib3d.so.2.4.11
exercise5: /usr/local/lib/libopencv_nonfree.so.2.4.11
exercise5: /usr/local/lib/libopencv_ocl.so.2.4.11
exercise5: /usr/local/lib/libopencv_gpu.so.2.4.11
exercise5: /usr/local/lib/libopencv_photo.so.2.4.11
exercise5: /usr/local/lib/libopencv_objdetect.so.2.4.11
exercise5: /usr/local/lib/libopencv_legacy.so.2.4.11
exercise5: /usr/local/lib/libopencv_video.so.2.4.11
exercise5: /usr/local/lib/libopencv_ml.so.2.4.11
exercise5: /usr/local/lib/libopencv_calib3d.so.2.4.11
exercise5: /usr/local/lib/libopencv_features2d.so.2.4.11
exercise5: /usr/local/lib/libopencv_highgui.so.2.4.11
exercise5: /usr/local/lib/libopencv_imgproc.so.2.4.11
exercise5: /usr/local/lib/libopencv_flann.so.2.4.11
exercise5: /usr/local/lib/libopencv_core.so.2.4.11
exercise5: CMakeFiles/exercise5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable exercise5"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/exercise5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/exercise5.dir/build: exercise5
.PHONY : CMakeFiles/exercise5.dir/build

CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/exercise5.cc.o.requires
CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/camera.cc.o.requires
CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/particles.cc.o.requires
CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/random_numbers.cc.o.requires
CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/resampling.cc.o.requires
CMakeFiles/exercise5.dir/requires: CMakeFiles/exercise5.dir/robot.cc.o.requires
.PHONY : CMakeFiles/exercise5.dir/requires

CMakeFiles/exercise5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/exercise5.dir/cmake_clean.cmake
.PHONY : CMakeFiles/exercise5.dir/clean

CMakeFiles/exercise5.dir/depend:
	cd /home/sebastian/robot/Exam/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sebastian/robot/Exam /home/sebastian/robot/Exam /home/sebastian/robot/Exam/build /home/sebastian/robot/Exam/build /home/sebastian/robot/Exam/build/CMakeFiles/exercise5.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/exercise5.dir/depend


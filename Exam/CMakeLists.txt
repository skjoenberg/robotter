CMAKE_MINIMUM_REQUIRED (VERSION 2.8.9)
PROJECT(main CXX)
#add_library(main file1.c file2.c)


#FIND_PACKAGE(OpenCV REQUIRED core highgui imgcodecs)
FIND_PACKAGE(OpenCV REQUIRED)


INCLUDE_DIRECTORIES(/usr/local/include/player-2.0/)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

MESSAGE( STATUS "OpenCV_VERSION = ${OpenCV_VERSION}")
MESSAGE( STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
MESSAGE( STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")

add_executable(checkerboard_generator checkerboard_generator.cc)
target_link_libraries(checkerboard_generator ${OpenCV_LIBS})

add_executable(camera_calibrator camera_calibrator.cc)
target_link_libraries(camera_calibrator ${OpenCV_LIBS})


SET(SRCS
  camera.cc
  particles.cc
  random_numbers.cc
  resampling.cc
  robot.cc
)

SET(HEADERS
  main.h
  camera.h
  particles.h
  random_numbers.h
  resampling.h
  robot.h
)

SET(DUMMY_MAIN
  main.cc
)

#ignore warnings
set_source_files_properties(
  ${main.cc}
  ${particles.cc}
  ${ui.cc}
  COMPILE_FLAGS "-w"
)

add_executable(main ${DUMMY_MAIN} ${SRCS} ${HEADERS})
target_link_libraries(main playerc++)
target_link_libraries(main ${OpenCV_LIBS})


SET(SOLUTION_MAIN
  solutions/solution.cc
)

#add_executable(solution ${SOLUTION_MAIN} ${SRCS} ${HEADERS})
#target_link_libraries(solution ${OpenCV_LIBS})
#SET_PROPERTY(TARGET solution APPEND PROPERTY INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}")

# Only works with CMake 3.3 and onwards
#target_include_directories(solution PUBLIC ${PROJECT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.10)
project(osm_pathfinding)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR /usr/local/Cellar/opencv/4.11.0/lib/cmake/opencv4)
set(TINYXML2_LIBRARIES /usr/local/Cellar/tinyxml2/10.0.0/lib/libtinyxml2.dylib)

find_package(OpenCV REQUIRED)
find_package(tinyxml2 REQUIRED)

add_executable(osm_pathfinding
   src/main.cpp
   src/osm_parser.cpp
   src/graph.cpp
   src/visualizer.cpp
)

target_include_directories(osm_pathfinding PRIVATE
   ${OpenCV_INCLUDE_DIRS}
   /usr/local/Cellar/tinyxml2/10.0.0/include
   include
)

target_link_libraries(osm_pathfinding PRIVATE
   ${OpenCV_LIBS}
   ${TINYXML2_LIBRARIES}
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/resources)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/output)
file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)
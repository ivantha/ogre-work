cmake_minimum_required(VERSION 3.5)
project(OGRE-PROJECT1 VERSION 1.0.0)

set(OGRE_BUILD_DEPENDENCIES=OFF)
set(CMAKE_CXX_STANDARD 11)

add_executable(Assignment Assignment.cpp PartA.cpp PartA.h PartB.cpp PartB.h)

# specify which version and components you need
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem Terrain)

# zlib
find_package(ZLIB)

# copy resource.cfg next to our binaries where OGRE looks for it
file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(Assignment ${OGRE_LIBRARIES})
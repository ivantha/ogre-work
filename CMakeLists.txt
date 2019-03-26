cmake_minimum_required(VERSION 3.5)
project(OGRE-PROJECT1 VERSION 1.0.0)

set(OGRE_BUILD_DEPENDENCIES=OFF)
set(CMAKE_CXX_STANDARD 11)

set(OGRE_DIR    "/Users/ivantha/Git/ogre/build/sdk/CMake")

#set(ZZLIB    "/Users/ivantha/Git/ogre/build/sdk/lib/libzzip.dylib")
#set(FREETYPE "/Users/ivantha/Git/ogre/build/Dependencies/lib/libfreetype.dylib")

add_executable(exec_name A.cpp)

# specify which version and components you need
find_package(OGRE 1.11 REQUIRED COMPONENTS Bites RTShaderSystem)

# copy resource.cfg next to our binaries where OGRE looks for it
file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${CMAKE_BINARY_DIR})

#target_link_libraries(exec_name  ${ZZLIB} ${FREETYPE} ${OGRE_LIBRARIES})
target_link_libraries(exec_name  ${ZZLIB} ${OGRE_LIBRARIES})
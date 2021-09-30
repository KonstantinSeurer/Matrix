# Install script for directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/spirv-tools" TYPE FILE FILES
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/include/spirv-tools/libspirv.h"
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/include/spirv-tools/libspirv.hpp"
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/include/spirv-tools/optimizer.hpp"
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/include/spirv-tools/linker.hpp"
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/include/spirv-tools/instrument.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/SPIRV-Tools.pc"
    "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/SPIRV-Tools-shared.pc"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/external/cmake_install.cmake")
  include("/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/source/cmake_install.cmake")
  include("/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/tools/cmake_install.cmake")
  include("/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/cmake_install.cmake")
  include("/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/examples/cmake_install.cmake")

endif()


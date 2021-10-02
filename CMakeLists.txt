
project(MatrixCore)
cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE MATRIX_CORE_LIBRARY_SOURCE_FILES src/*.cpp)

set(MATRIX_CORE_DYNAMIC_DEPENDENCIES "")

set(MATRIX_CORE_DEPENDENCY_INCLUDES
	dependencies/glfw/include
	dependencies/json/include
	dependencies/shaderc/libshaderc/include
	dependencies/stb
)

find_package(Vulkan)
list(APPEND MATRIX_CORE_DYNAMIC_DEPENDENCIES Vulkan::Vulkan)

if(NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
	list(APPEND MATRIX_CORE_DEPENDENCY_INCLUDES ${Vulkan_INCLUDE_DIRS})
endif()

if(NOT Vulkan_FOUND)
	message(STATUS "Failed to locate Vulkan SDK!")
endif()

if(UNIX)
	find_package(Threads REQUIRED)
	list(APPEND MATRIX_CORE_DYNAMIC_DEPENDENCIES Threads::Threads)
endif()

if(UNIX)
	set(MATRIX_CORE_STATIC_DEPENDENCIES
		${CMAKE_SOURCE_DIR}/dependencies/bin/linux/glfw/src/libglfw3.a
		${CMAKE_SOURCE_DIR}/dependencies/bin/linux/shaderc/libshaderc/libshaderc_combined.a
	)
endif()

add_library(MatrixCoreStatic STATIC ${MATRIX_CORE_LIBRARY_SOURCE_FILES})
target_link_libraries(MatrixCoreStatic PUBLIC ${MATRIX_CORE_DYNAMIC_DEPENDENCIES} ${MATRIX_CORE_STATIC_DEPENDENCIES} ${CMAKE_DL_LIBS})
target_include_directories(MatrixCoreStatic PUBLIC ${MATRIX_CORE_DEPENDENCY_INCLUDES} include)

file(GLOB_RECURSE MATRIX_DEMO_SOURCE_FILES src/demos/*.cpp)

add_executable(MatrixDemos ${MATRIX_DEMO_SOURCE_FILES})
target_link_libraries(MatrixDemos PUBLIC ${MATRIX_CORE_DYNAMIC_DEPENDENCIES} MatrixCoreStatic ${CMAKE_DL_LIBS})
target_include_directories(MatrixDemos PUBLIC ${MATRIX_CORE_DEPENDENCY_INCLUDES} include)

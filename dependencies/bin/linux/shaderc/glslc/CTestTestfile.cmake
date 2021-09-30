# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(glslc_file "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc/glslc_file_test")
set_tests_properties(glslc_file PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/cmake/utils.cmake;133;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;58;shaderc_add_tests;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;0;")
add_test(glslc_resource_parse "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc/glslc_resource_parse_test")
set_tests_properties(glslc_resource_parse PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/cmake/utils.cmake;133;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;58;shaderc_add_tests;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;0;")
add_test(glslc_stage "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc/glslc_stage_test")
set_tests_properties(glslc_stage PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/cmake/utils.cmake;133;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;58;shaderc_add_tests;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/CMakeLists.txt;0;")
subdirs("test")

# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(spirv-tools-test_spirv_unit_tests "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/test_spirv_unit_tests")
set_tests_properties(spirv-tools-test_spirv_unit_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;78;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;159;add_spvtools_unittest;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;0;")
add_test(spirv-tools-test_c_interface "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/test_c_interface")
set_tests_properties(spirv-tools-test_c_interface PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;78;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;164;add_spvtools_unittest;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;0;")
add_test(spirv-tools-test_c_interface_shared "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/test_c_interface_shared")
set_tests_properties(spirv-tools-test_c_interface_shared PROPERTIES  ENVIRONMENT "PATH=/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/source" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;78;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;169;add_spvtools_unittest;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;0;")
add_test(spirv-tools-test_cpp_interface "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/test_cpp_interface")
set_tests_properties(spirv-tools-test_cpp_interface PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;78;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;175;add_spvtools_unittest;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;0;")
add_test(spirv-tools-test_timer "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/test_timer")
set_tests_properties(spirv-tools-test_timer PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;78;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;181;add_spvtools_unittest;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/CMakeLists.txt;0;")
subdirs("link")
subdirs("lint")
subdirs("opt")
subdirs("reduce")
subdirs("fuzz")
subdirs("tools")
subdirs("util")
subdirs("val")
subdirs("fuzzers")

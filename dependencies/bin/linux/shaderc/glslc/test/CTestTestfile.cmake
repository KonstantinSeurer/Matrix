# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(shaderc_expect_unittests "/usr/bin/python3" "-m" "unittest" "expect_unittest.py")
set_tests_properties(shaderc_expect_unittests PROPERTIES  WORKING_DIRECTORY "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;15;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;0;")
add_test(shaderc_glslc_test_framework_unittests "/usr/bin/python3" "-m" "unittest" "glslc_test_framework_unittest.py")
set_tests_properties(shaderc_glslc_test_framework_unittests PROPERTIES  WORKING_DIRECTORY "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;18;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;0;")
add_test(glslc_tests "/usr/bin/python3" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/glslc_test_framework.py" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/glslc/glslc" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/tools/spirv-dis" "--test-dir" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test")
set_tests_properties(glslc_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;23;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/glslc/test/CMakeLists.txt;0;")

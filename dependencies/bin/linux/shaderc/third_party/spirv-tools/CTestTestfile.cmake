# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(spirv-tools-copyrights "/usr/bin/python3" "utils/check_copyright.py")
set_tests_properties(spirv-tools-copyrights PROPERTIES  WORKING_DIRECTORY "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/CMakeLists.txt;348;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/CMakeLists.txt;0;")
subdirs("external")
subdirs("source")
subdirs("tools")
subdirs("test")
subdirs("examples")

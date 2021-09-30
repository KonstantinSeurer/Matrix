# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/tools/opt
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/test/tools/opt
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(spirv_opt_cli_tools_tests "/usr/bin/python3" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/tools/opt/../spirv_test_framework.py" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/tools/spirv-opt" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/tools/spirv-as" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/tools/spirv-dis" "--test-dir" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/tools/opt")
set_tests_properties(spirv_opt_cli_tools_tests PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/tools/opt/CMakeLists.txt;17;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/test/tools/opt/CMakeLists.txt;0;")

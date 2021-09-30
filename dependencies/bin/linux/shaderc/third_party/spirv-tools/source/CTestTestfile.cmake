# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/source
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(spirv-tools-symbol-exports-SPIRV-Tools-shared "/usr/bin/python3" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/utils/check_symbol_exports.py" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/source/libSPIRV-Tools-shared.so")
set_tests_properties(spirv-tools-symbol-exports-SPIRV-Tools-shared PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/CMakeLists.txt;249;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;371;spvtools_check_symbol_exports;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;378;spirv_tools_default_target_options;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;0;")
add_test(spirv-tools-symbol-exports-SPIRV-Tools-static "/usr/bin/python3" "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/utils/check_symbol_exports.py" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/spirv-tools/source/libSPIRV-Tools.a")
set_tests_properties(spirv-tools-symbol-exports-SPIRV-Tools-static PROPERTIES  _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/CMakeLists.txt;249;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;371;spvtools_check_symbol_exports;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;387;spirv_tools_default_target_options;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/spirv-tools/source/CMakeLists.txt;0;")
subdirs("opt")
subdirs("reduce")
subdirs("fuzz")
subdirs("link")
subdirs("lint")

# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/glslang
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/glslang
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(glslang-testsuite "bash" "runtests" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/glslang/localResults" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/glslang/StandAlone/glslangValidator" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/glslang/StandAlone/spirv-remap")
set_tests_properties(glslang-testsuite PROPERTIES  WORKING_DIRECTORY "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/glslang/Test/" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/glslang/CMakeLists.txt;355;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/glslang/CMakeLists.txt;0;")
subdirs("External")
subdirs("glslang")
subdirs("OGLCompilersDLL")
subdirs("StandAlone")
subdirs("SPIRV")
subdirs("hlsl")
subdirs("gtests")

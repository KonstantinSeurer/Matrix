# CMake generated Testfile for 
# Source directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/effcee/examples
# Build directory: /home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/effcee/examples
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(effcee-example "/usr/bin/python3" "effcee-example-driver.py" "/home/konstantin/Desktop/matrix/Matrix/dependencies/bin/linux/shaderc/third_party/effcee/examples/effcee-example" "example_data.txt" "CHECK: Hello" "CHECK-SAME: world" "CHECK-NEXT: Bees" "CHECK-NOT: Sting" "CHECK: Honey")
set_tests_properties(effcee-example PROPERTIES  WORKING_DIRECTORY "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/effcee/examples" _BACKTRACE_TRIPLES "/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/effcee/examples/CMakeLists.txt;15;add_test;/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/third_party/effcee/examples/CMakeLists.txt;0;")

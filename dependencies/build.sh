
cd ../../../shaderc
./utils/git-sync-deps
cmake -B ../bin/linux/shaderc -GNinja -DCMAKE_BUILD_TYPE=Release
cd ../bin/linux/shaderc
ninja

cd ../../../glfw
cmake -B ../bin/linux/glfw -GNinja
cd ../bin/linux/glfw
ninja

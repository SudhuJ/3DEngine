target="Targets/$1"
mkdir -p $target

source ./flow/bin/activate

conan install . --output-folder=$target --build=missing -s build_type=$1 -c tools.system.package_manager:sudo=True -c tools.system.package_manager:mode=install

# Reconfigure with ASan OFF
cmake -S . -B Targets/Debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="Targets/Debug/conan_toolchain.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_ASAN=OFF

# fix for clangd LSP not finding compile_commands.json
ln -sf "$target/compile_commands.json" compile_commands.json

cmake --build $target --config $1


echo Clone vcpkg:
git clone https://github.com/microsoft/vcpkg vcpkg

echo Bootstrap vcpkg:
cd vcpkg
call bootstrap-vcpkg.bat
.\vcpkg install openssl:x64-windows
cd ..

echo Configure CMake build:
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release

echo Build project in build directory:
cmake --build build --config Release

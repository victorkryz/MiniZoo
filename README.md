# MiniZoo

MiniZoo is a cross-platform C++17 console application for managing a small zoo.
It is built with CMake and is designed for Linux and Windows with Visual Studio.

The project uses POCO for AES cryptography, SQLite database access, and
foundation utilities. CMake downloads the required POCO 1.14.2 components
automatically through `FetchContent` during the first configuration.

## Runtime data

Each time MiniZoo runs, it creates a fresh `minizoo.db` SQLite database file in
the process's current working directory. This is the directory from which
MiniZoo is launched, and it may differ from the executable's directory.

## Requirements

- CMake 3.20 or newer
- A C++17 compiler
- OpenSSL development libraries
- Internet access during the initial CMake configuration

## Build on Windows with Visual Studio

Install CMake and an OpenSSL development package discoverable by CMake.

From a Developer Command Prompt or PowerShell:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\bin\Release\MiniZoo.exe
```

or use `build-windows.bat` that pulls vcpkg into current directory, bootstraps it, installs openssl, configures CMake build and builds the project:  

```powershell
build-windows.bat
```

## Build on Ubuntu

Install the required tools:

```bash
sudo apt update
sudo apt install build-essential cmake git libssl-dev
```

Configure, build, and run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bin/MiniZoo
```


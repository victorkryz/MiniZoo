cmake_minimum_required(VERSION 3.20)

project(MiniZoo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include("${CMAKE_CURRENT_LIST_DIR}/cmake/PocoDependencies.cmake")

add_executable(
    MiniZoo
    src/crypto.cpp
    src/database.cpp
    src/main.cpp
)

target_link_libraries(
    MiniZoo
    PRIVATE
        Poco::Foundation
        Poco::Util
        Poco::Data
        Poco::DataSQLite
        Poco::Crypto
)

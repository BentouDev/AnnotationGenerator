#!/bin/bash
export CXX=clang++-5.0
export CC=clang-5.0
$CXX --version
cmake --version
conan --version
conan create . bentoudev/yage -s compiler=clang -s build_type=Release
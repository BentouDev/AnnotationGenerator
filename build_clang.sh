#!/bin/bash
export CXX=clang++-5.0
export CC=clang-5.0
$CXX --version
cmake --version
conan --version
conan create . bentoudev/yage -s compiler=clang -s build_type=Release
conan remote add yage https://api.bintray.com/conan/bentoudev/yage
conan user bentoudev -r yage -p $1
conan upload Agnes/0.1@bentoudev/yage --all -r=yage
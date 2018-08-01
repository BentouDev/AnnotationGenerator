#!/bin/bash
conan remote add yage https://api.bintray.com/conan/bentoudev/yage
conan user -p $1 -r yage bentoudev
conan upload Agnes/0.1@bentoudev/yage --all -r=yage
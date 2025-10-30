#!/bin/bash

mkdir -p build && cd build && cmake .. -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_CXX_FLAGS="-fPIC" -DBUILD_SHARED_LIBS=ON && make
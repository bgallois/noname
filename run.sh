#!/bin/bash

if [ "$1" = "release" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE:String=Release ../src
  make
  ./QBackup
fi

if [ "$1" = "debug" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE:String=Debug ../src
  make
  ./QBackup
fi

if [ "$1" = "minsizerel" ]; then
  set -e
  mkdir -p build
  cd build
  cmake -DCMAKE_BUILD_TYPE:String=MinSizeRel ../src
  make
  ./QBackup
fi

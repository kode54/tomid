#!/bin/sh
cat json_header 8820.json json_footer > 8820json.h

clang++ -Os -std=c++11 2mid.cpp midi_processing/*.cpp -o 2mid

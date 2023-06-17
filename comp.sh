#!/bin/bash

set -e
echo "building pmt"

g++ -o pmt pmt.cpp structure.hpp structure.cpp exec.hpp exec.cpp -fdiagnostics-color=always -Werror -Wfatal-errors -g -DBOOST_BIND_GLOBAL_PLACEHOLDERS #-lboost_program_options

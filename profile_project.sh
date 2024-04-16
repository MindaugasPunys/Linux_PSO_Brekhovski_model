#!/bin/bash
cd build
cmake ..
make

# run binnary
./PSO_MODEL

# run gprof to profile the code
gprof ./PSO_MODEL gmon.out > profile_report.txt
echo "Profiling report generated: profile_report.txt"

# Profile instructions with valgrind
# valgrind --tool=callgrind ./PSO_MODEL
# echo "Callgrind output generated: callgrind.out"

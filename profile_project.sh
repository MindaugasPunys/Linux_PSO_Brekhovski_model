#!/bin/bash
cd build
cmake ..
make

# Step 2: Run the executable (assuming it's named 'PSO_MODEL')
./PSO_MODEL

# Step 3: Generate and analyze profiling data with gprof
gprof --inlines ./PSO_MODEL gmon.out > profile_report.txt
echo "Profiling report generated: profile_report.txt"

# Step 4: Profile instructions with valgrind
valgrind --tool=callgrind ./PSO_MODEL
echo "Callgrind output generated: callgrind.out"

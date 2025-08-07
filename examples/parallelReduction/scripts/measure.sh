#!/bin/bash

start=1000
end=10000000
runs=50
step=$(( (end - start) / (runs - 1) ))

for ((i=0; i<runs; i++)); do
    param=$(( start + i * step ))
    output=$(./parallelReduction -s $param)
    gpuComputation=$(echo "$output" | grep "Only computation" | awk '{print $3}')
    gpuMemory=$(echo "$output" | grep "Only memory" | awk '{print $3}')
    cpuTime=$(echo "$output" | grep "^Time:" -A 1 | grep "Time:" | tail -n 1 | awk '{print $2}')
    echo "$param $gpuComputation $gpuMemory $cpuTime"
done

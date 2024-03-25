# Assignment 1 Taint and Very Busy Expression Analysis

## How to Run
To compile tasks
```bash
chmod +x compile
# to compile all cpp scripts
./compile task*.cpp
# to compile a single cpp script
./compile task1.cpp
```

To generate `*.ll` and `*.dot` files from `.c` files in test director
```bash
# create the corresponding .ll and .dot files from .c files in test directry
bash prepare.sh
```

## Tasks
### Task 1
Theory and algorithm description.
### Task 2
Taint analysis for mini C programs without loop.
```bash
./task2 ./tests/task2_1.ll

./task2 ./tests/task2_2.ll

./task2. /tests/task2_3.ll
```

### Task 3
Generate tainting paths for mini C programs without loop.
```bash
./task3 ./tests/task3_1.ll

./task3 ./tests/task3_2.ll
```

### Task 4
Taint analysis for mini C programs with loops.
```bash
./task4 ./tests/task4_1.ll

./task4 ./tests/task4_2.ll
```

### Task 5
Very busy express analysis for mini C programs with loops.
```bash
./task5 ./tests/task5_1.ll

./task5 ./tests/task5_2.ll

./task5 ./tests/task5_3.ll
```

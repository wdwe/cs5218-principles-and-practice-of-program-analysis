# Assignment 2 Abstract Interpretation

Analyse variables' range of values at different program points and the max differences between them.

To facilitate the implementation of the passes, two classes are created.

Class `Abst`, standing for Abstract Domain, is created to simulate the computation in the abstract
domain. In particular, it has two static members `Abst::MAX` and `Abst::MIN` . By default, they are the
same as `INT_MAX` and `INT_MIN` . However, when widening is required to ensure finite lattice height, they can be set to a smaller
number (in the code, `200` and `-200` are used). Any number larger than `Abst::MAX` is set to `Abst::INF`
and any number smaller than `Abst::MIN` is set to `Abst::NEG_INF`.

Various arithmetic and boolean operation for this domain are also implemented.

Class Interval helps with the actual abstraction. Various arithmetic operations are defined for this
class too.

At the same time, various predicate’s transfer functions for `Interval` are also defined so path sensitive interval
analysis can be carried out.

## How to Run
To compile tasks
```bash
chmod +x compile
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
Theory and algorithm description see the pdf file.
### Task 2 - Difference analysis
For mini C programs without loop.
```bash
./task2_diff tests/task2_diff_example_1.ll
```

### Task 2 - Interval analysis
Path-sensitive interval analysis for mini C programs without loop.
```bash
./task2_interval tests/task2_interval_1.ll

./task2_interval tests/task2_interval_2.ll
```

### Task 3 - Difference analysis with widening
For mini C programs with loops.
```bash
./task3_diff test/task3_diff_example_2.ll
```

### Task 3 - Interval analysis with widening
Path-sensitive interval analysis for mini C programs with loops.
```bash
./task3_interval tests/task3_interval_example_3.ll
```

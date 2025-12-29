# tkoz-all-v1

This is an experiment to put "all" of my C/C++ projects (maybe other languages
too) into a single repo with a setup inspired by the one in my workplace. It is
intended to be a bit of a playground for serious projects to eventually emerge.

# todo

- gcov/llvm-cov code coverage
  - may need more testing, clang requires an executable that links all libraries
- clang-tidy and cppcheck
- compile with instructions like avx2/avx512

# ideas

This is an incomplete list of possible project ideas I am interested in
exploring. While exploration may result in lower quality code at first,
something of high quality, high maintability, and high performance is a goal.

## fractals

- flame fractal renderer (IFS and nonlinear generalization)
  - GPU implementations may be challenging but possible
- escape time fractal renderer
  - these should extend reasonably well to a GPU implementation
- flame fractal editor GUI
- fractal videos

## puzzles

- logic puzzle solvers
  - grid puzzles like sudoku, heyawake, slitherlink, etc
- logic puzzle generators with difficulty estimation
- interactive and automated proof creation for grid puzzles
  - inspired by LEGUP on BramHub
- GUI for computer assisted puzzle creation
  - create puzzles with human creativity rather than machine randomness
- twisty puzzles such as rubiks cube

## hard problems

- solvers for NP complete/hard problems
- NP completeness reductions
- (integer) linear programming
  - linear programming with exact rational arithmetic
  - optimizations such as cutting planes
  - branch and bound for integer constraints
- nonlinear programming
- backtracking search and constraint propagation
- approximation algorithms

## mathematics

- large integer arithmetic
- high precision calculation of constants like pi, e, sqrt(2)
  - inspired by y-cruncher
- integer factorization
  - optimized trial division and pollard rho
  - elliptic curves and number field sieve
  - exploration of other lesser used algorithms
- number theoretic computations
- brute force search and conjecture verification
  - collatz conjecture
  - search for a factor of `2^(2^127 - 1) - 1`
- symbolic computation
- proof verication and automation

## enumeration

- counting isomorphically distinct graphs
- truncatable primes
- solved sudokus

## prog lang

- interpreters/compilers for esoteric languages
- turing machines and lambda calculus
- my own functional or object oriented language

## simulation

- game of life
- physics simulation

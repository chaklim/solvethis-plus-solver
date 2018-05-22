# SolveThis+ / 1d1q  Solver
An mini game solver using A* search algorithm

## Input
```
[# of rows (r)] [# of columns (c)]
[
  r * c numbers represents:
    0: blank
    1: initial movable block position(s)
    2: final block position(s)
    3: overlapped movable block & final block position(s)
   -1: black block position(s)
]
```

## Sample Input
```
8 8
 0  0  0  0  0  0  2  0
 0  0  0  0  0  2  0  2
 0  0  0  0  0  0  2  0
-1 -1  0 -1 -1  0 -1 -1
 0  0  0  0  0  0  0  0
 0  0  1  0  0  0  0  0
 0  1  0  1  0  0  0  0
 0  0  1  0  0  0  0  0
```

## Usage
```
make solver

./solver < sampleInput/test_1.in
```

## Expected Output
```
number of steps: 16
U U R R D L U U R U R U R R U R 
game end
```

## Explanation of expected output
```
number of steps: minimum # of steps 
U: move upwards
R: move towards right
L: move towards left
D: move downwards
```

## Limitation
- At most 8x8 (because using unsigned long long represents game state)
- Three unsolved input in unsolved folder

# Note
If you want to print every state from start to end, you can change const variable isPrintSolutionWithState to true.
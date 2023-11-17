# Tic-Tac-Toe

My implementation of the Tic-Tac-Toe game in C++. Wrote this a while ago, only now uploading it.

This uses standard minimax with bit manip.

- `index.cpp` - main file
- `minified.cpp` - minified version of `index.cpp` without I/O

## Running
```bash
g++ index.cpp -O3 -o index.out
./index.out < file.in
```

## Input
The input is a 3x3 grid, with `X` representing the player, `O` representing the computer, and `-` representing an empty cell. Each line should be terminated with a newline character.

> Example first move
```
X--
---
---
```


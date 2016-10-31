# Challenge
https://www.reddit.com/r/dailyprogrammer/comments/59mnxa/20161027_challenge_289_intermediate_metro_trip

Implemented a specialized directed weighted graph along with a shortest path algorithm, taken from http://algs4.cs.princeton.edu/home/

# Goals
* compile time safety
* clarity
* no external dependencies
* learn C++ basics

# Compiling
```
g++ -std=c++14 metro_planner.cpp -o bin/metroplan
```

# Running
`./bin/metroplan basic.txt` or `./bin/metroplan input.txt`
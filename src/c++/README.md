# DSGC - C++ implementation

## Building

To build this, all you have to run is:

```
g++ dsgc.cpp -std=c++11 -c
```

This will product an object file. Consult Google if you want to produce other things. This code should also compile with `clang++` if that suits your fancy. And yes, this project does require C++ 11 or later.

## Linking

`dsgc.hpp` has all the exported functions of DSGC. `#include` it into any source file that needs it. When you compile, be sure to include `dsgc.cpp` or `dsgc.o` in your invocation of `g++` or `clang++`.
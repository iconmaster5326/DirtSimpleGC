# DSGC - C implementation

## Building

To build this, all you have to run is:

```
gcc dsgc.c -c
```

This will product an object file. Consult Google if you want to produce other things. This code should also compile with `clang` if that suits your fancy.

## Linking

`dsgc.h` has all the exported functions of DSGC. `#include` it into any source file that needs it. When you compile, be sure to include `dsgc.c` or `dsgc.o` in your invocation of `gcc` or `clang`.
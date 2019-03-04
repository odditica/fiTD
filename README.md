# fiTD 1.0 [![Build Status](https://travis-ci.org/Blokatt/fiTD.svg?branch=master)](https://travis-ci.org/Blokatt/fiTD)
*A basic console-based tower defense game engine.*

*Originally created by **Jan Vorisek** as the final BI-PA2 seminar assigment at **FIT CTU** in 2018.*

![](preview.gif)

Features:
---
- ncurses-powered renderer
- Standard TD game mechanics, multiple types of AI behaviour
- Fully customisable game properties (single plaintext file)
- Tutorial
- Per-map game saving/loading

Building
---
Dependencies:
- ncurses
- Doxygen (documentation)
- cmake
- Catch2 (tests)


**Compile (./build/):**

```
cmake .
make
```

**Generate documentation (./doc/):**

```
cmake .
make doc
```

**Clean all**

`
make clean
`

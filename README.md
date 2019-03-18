# fiTD 1.0.1 [![Build Status](https://travis-ci.org/Blokatt/fiTD.svg?branch=master)](https://travis-ci.org/Blokatt/fiTD)
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

Planned:
---
- More game content + gameplay fine-tuning
- Sound
- Level editor

Building
---
Dependencies:
- CMake
- Catch2 (including CMake scripts, more info [here](https://github.com/catchorg/Catch2/blob/master/docs/cmake-integration.md#installing-catch2-from-git-repository))
- ncurses
- Doxygen (documentation)

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

Running tests
---

`
make test
`

Running the game
---

```
cd build
./fitd
```

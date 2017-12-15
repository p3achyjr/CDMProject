# Overview

In this project, I translated the URM presented in the notes using macros into a macro-free version using just inc and dec instructions. The full definition of the URM is in urm.cpp

I attempted to do cycle analysis and optimizations, but none were successful in the end. As such, this project supports a "fast execution" mode, which essentially cheats the inefficiencies of a 2-instruction register machine by assuming that the macros are atomic.

# Descriptions of Each Translation

## Copy


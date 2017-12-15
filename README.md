# Overview

In this project, I translated the URM presented in the notes using macros into a macro-free version using just inc and dec instructions. The full definition of the URM is in urm.cpp

I attempted to do cycle analysis and optimizations, but none were successful in the end. As such, this project supports a "fast execution" mode, which essentially cheats the inefficiencies of a 2-instruction register machine by assuming that the macros are atomic.

# Descriptions of Each Translation

## Copy r s k

copy r to s, r' by repeatedly decrementing r, while incrementing r' and r'' simultaneously.

Finally, move r' back to r.

## Zero r k l

This is pretty self-explanatory.

## Pop r s k

Repeatedly divide r in half until we see that the parity of the quotient is odd. Each time we divide in half, increment s. When we reach our termination condition, s will contain the correct value.

## Read r t s k

Copy r to r'. Divide r' in half repeatedly, and each time the parity is odd, decrement t. When t hits 0, divide r' in half until we see the next 1, incrementing s at each step.

## Write r t s k

This was by far the trickiest one to do. I used 4 auxiliary registers. To accomplish this, I would repeatedly pop elements off of r, and form a stack of seen elements in the register rstk. Similarly, every time we encounter a 1, we decrement t. When t hits 0, divide r in half until we see the next 1. At this point, multiply r by 2 s number of times. Then pop numbers off of rstk and push them back onto the sequence number in r.
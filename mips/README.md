# MIPS

These are codes for Computer architecture class assignments. The assembly
files are MIPS, intended to run on SPIM.

`#1` is just simple assembly code; `a1.asm` performs Celsius-to-Fahrenheit
conversion, and `a2.asm` calculates value of AX^2+BX+C where each variable
has a hard coded value.

`The Assignment` solves the nearest-neighbor, using a segment tree because
segment tree was part of the requirements. The requirement was rather vague
about how that segment tree should be used, so its usage in this program is
somewhat strange. Anyways, `a.asm` contains hand-written assmbly that fulfills
the requirement, `input.txt` contains points the program handles, and `x.py` is
there to verify that the output of `a.asm` is indeed correct.
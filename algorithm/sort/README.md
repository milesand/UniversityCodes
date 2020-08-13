# Building and running

Run `make (insertion|quick|merge|heap)` to build. Binary with the same name
will be placed in `bin`.

Binaries take following arguments:

- `-i <iteration>`: Iterations to perform for averaging run time. Defaults to 16.
- `-n <length>`: Length of array to be sorted. Defaults to 131072.
- `-s <seed>`: Seed to be used for `srand`. Defaults to result of `time`.

# Results

Result obtained from system with Intel i7-7700, 16GB ram.

## Insertion sort
```
Running with len = 131072, seed = ..., iterations = 16
# 1: Sort complete in 9.857021 seconds
# 2: Sort complete in 9.859596 seconds
# 3: Sort complete in 9.891581 seconds
# 4: Sort complete in 9.901086 seconds
# 5: Sort complete in 9.849671 seconds
# 6: Sort complete in 9.900047 seconds
# 7: Sort complete in 9.817508 seconds
# 8: Sort complete in 9.858312 seconds
# 9: Sort complete in 9.866954 seconds
#10: Sort complete in 9.861536 seconds
#11: Sort complete in 9.878541 seconds
#12: Sort complete in 9.866457 seconds
#13: Sort complete in 9.872478 seconds
#14: Sort complete in 9.812285 seconds
#15: Sort complete in 9.863189 seconds
#16: Sort complete in 9.823288 seconds
Average time: 9.861222 seconds
```

## Quicksort
```
Running with len = 131072, seed = ..., iterations = 16
# 1: Sort complete in 0.009188 seconds
# 2: Sort complete in 0.009307 seconds
# 3: Sort complete in 0.009070 seconds
# 4: Sort complete in 0.009020 seconds
# 5: Sort complete in 0.008835 seconds
# 6: Sort complete in 0.008880 seconds
# 7: Sort complete in 0.009208 seconds
# 8: Sort complete in 0.008910 seconds
# 9: Sort complete in 0.008913 seconds
#10: Sort complete in 0.008736 seconds
#11: Sort complete in 0.008869 seconds
#12: Sort complete in 0.008915 seconds
#13: Sort complete in 0.008950 seconds
#14: Sort complete in 0.009201 seconds
#15: Sort complete in 0.008806 seconds
#16: Sort complete in 0.008967 seconds
Average time: 0.008986 seconds
```

## Mergesort
```
Running with len = 131072, seed = ..., iterations = 16
# 1: Sort complete in 0.008060 seconds
# 2: Sort complete in 0.008321 seconds
# 3: Sort complete in 0.007941 seconds
# 4: Sort complete in 0.007771 seconds
# 5: Sort complete in 0.007861 seconds
# 6: Sort complete in 0.007823 seconds
# 7: Sort complete in 0.007904 seconds
# 8: Sort complete in 0.007867 seconds
# 9: Sort complete in 0.007764 seconds
#10: Sort complete in 0.007931 seconds
#11: Sort complete in 0.007809 seconds
#12: Sort complete in 0.007880 seconds
#13: Sort complete in 0.008005 seconds
#14: Sort complete in 0.007759 seconds
#15: Sort complete in 0.007989 seconds
#16: Sort complete in 0.007944 seconds
Average time: 0.007914 seconds
```

## Heapsort
```
Running with len = 131072, seed = ..., iterations = 16
# 1: Sort complete in 0.013941 seconds
# 2: Sort complete in 0.013612 seconds
# 3: Sort complete in 0.013695 seconds
# 4: Sort complete in 0.013564 seconds
# 5: Sort complete in 0.013352 seconds
# 6: Sort complete in 0.013393 seconds
# 7: Sort complete in 0.013356 seconds
# 8: Sort complete in 0.013399 seconds
# 9: Sort complete in 0.013350 seconds
#10: Sort complete in 0.013638 seconds
#11: Sort complete in 0.013538 seconds
#12: Sort complete in 0.013497 seconds
#13: Sort complete in 0.013339 seconds
#14: Sort complete in 0.013335 seconds
#15: Sort complete in 0.013505 seconds
#16: Sort complete in 0.013336 seconds
Average time: 0.013491 seconds
```

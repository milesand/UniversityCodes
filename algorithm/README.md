# Algorithm

Algorithm class assignments.

There isn't much to explain here. `prime` checks whether some number is prime,
`select` implements quickselect, `sort` implements various sorting algorithms.

## `twentyfour`

This one was a bit more interesting. The assignment was to solve a 5 by 5
sliding puzzle, like the fifteen-puzzle. Googling revealed that the memory was
the limiting factor here, and we were asked to optimize for moves, not execution
time; But it would still have to finish quick enough.

After considering options, I implemented
[IDA*](https://en.wikipedia.org/wiki/Iterative_deepening_A*), basically A* with
cost threshold that increments over tries. This was because it was simple
enough to implement quickly, and seemed to cover more cases than plain A*.

It later turned out all test cases were solvable via plain A*, which was quite
a letdown.

Code included is in Rust, but the actually handed-in version was a C++ port of
this code because the requirements included C++. I believe the C++ version
included a workaround for case where IDA* runs out of memory too, but I am not
certain what it was.

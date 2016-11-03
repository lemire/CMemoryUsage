#CMemoryUsage

Header files to help you track memory usage in C and C++.

## Requirement

Tested under macOS and Linux.

## Usage in C

In C, add the header early in your program. Use ``malloc``, ``free``, and ``calloc`` as you would normally do.
At all times, the ``malloced_memory_usage`` (global variable) keeps track
of how much memory was allocated.


```C
#include "cmemcounter.h"
#include <stdlib.h>

int main() {
  char * x = (char *) malloc(10);
  malloced_memory_usage == 10; // should be true
}
```


## Usage (C++)

In C++, simply create the STL container with the ``MemoryCountingAllocator`` memory allocator. The global variable ``memory_usage`` will track your memory usage.


```C++
#include "memtrackingallocator.h"

typedef std::vector<int,MemoryCountingAllocator<int> >  vector;

int main() {
  vector vec(10);
  memory_usage >= 10 * sizeof(int); // should be true
}
```


## Limitations

These headers should not be used in production code.

We do not even attempt to measure the overhead due to memory allocations,
so that the numbers are actually underestimates. We do not necessarily
intercept all memory allocations.

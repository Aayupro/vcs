# Git-Style Version Control Engine

A fully functional version control system built in C++ from scratch —
no STL containers, no external libraries. Implements the core algorithms
that power Git: Myers diff, LCA-based three-way merge, and mark-and-sweep GC.

## What it does
- Branch, merge, checkout, and snapshot arbitrary files
- Diff any two versions with Myers O(ND) shortest edit script
- Auto-detect merge base via binary-lifting LCA in O(log N)
- Three-way merge with <<<===>>> conflict markers
- Full-text search across all file versions (inverted index)
- Filename autocomplete (trie)
- Undo/redo every operation (write-ahead log)
- Garbage collect orphaned version nodes (mark-and-sweep)

## Compile and run
g++ -std=c++17 -Wall -O2 -o ttfs ttfs_v2.cpp
./ttfs
# type 'demo' to run the built-in walkthrough
# type 'help' to see all commands

## Algorithms implemented from scratch
| Algorithm | Complexity | Purpose |
|---|---|---|
| Myers Diff | O(ND) | Git-style diff between any two versions |
| LCA Binary Lifting | O(N log N) pre / O(log N) query | Merge-base detection |
| Three-Way Merge | O(ND) | Conflict-aware branch merging |
| Mark-and-Sweep GC | O(V+E) | Reclaim orphaned version nodes |
| Trie | O(L) | Filename prefix autocomplete |
| Inverted Index | O(W) index / O(1) search | Full-text search across versions |
| Write-Ahead Log | O(1) | Reversible undo/redo for all mutations |

## Data structures built from scratch (zero STL)
- CustomVector — doubling dynamic array
- CustomHashMap — separate chaining, prime buckets, LF 0.75 rehash
- MaxHeap — array-backed binary heap with custom comparators
- Trie — 128-child ASCII trie with recursive deletion
- InvertedIndex — djb2-hashed word → posting list map
- WriteAheadLog — doubly-linked reversible operation log

## Example session
create main.cpp
write main.cpp "int main() { return 0; }"
write main.cpp "int main() { cout << 'hello'; return 0; }"
diff main.cpp 1 2
branch main.cpp 1 "int main() { helper(); return 0; }"
merge main.cpp 3
tree main.cpp
gc

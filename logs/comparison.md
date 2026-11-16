| Metric | Before | After |
|---|---:|---:|
| gcc -fanalyzer warnings | 3 | 0 |
| cppcheck findings (error/warning) | 3 (1) | 0 (0) |
| flawfinder hits (risk level 4-5) | 8 (3) | 9 (0) |
| valgrind errors (all scenarios) | 36 | 0 |
| valgrind aborted by heap corruption | 1 | 0 |
| valgrind definitely lost (bytes) | 912 | 0 |
| ASan/LSan/UBSan reports | 7 | 0 |

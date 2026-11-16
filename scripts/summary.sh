#!/usr/bin/env bash
# Кількість знахідок кожного інструмента для етапу (before/after)
STAGE=${1:-before}
D=logs/$STAGE
gcc_w=$(grep -c 'warning:' "$D/gcc-analyzer.log")
cpp_w=$(grep -cE ': (error|warning|style|performance|portability):' "$D/cppcheck.log")
cpp_e=$(grep -cE ': (error|warning):' "$D/cppcheck.log")
ff=$(grep -oP 'Hits = \K[0-9]+' "$D/flawfinder.log")
ff4=$(grep -cE '^src/.*\[[45]\]' "$D/flawfinder.log")
vg_err=$(grep -oP 'ERROR SUMMARY: \K[0-9]+' "$D/valgrind.log" | awk '{s+=$1} END {print s+0}')
vg_crash=$(grep -c 'Heap block lo/hi size mismatch' "$D/valgrind.log")
vg_lost=$(grep -oP 'definitely lost: \K[0-9,]+' "$D/valgrind.log" | tr -d , | awk '{s+=$1} END {print s+0}')
asan=$(grep -cE 'ERROR: AddressSanitizer|ERROR: LeakSanitizer|runtime error:' "$D/asan.log")
printf '%-44s %s\n' "Stage:" "$STAGE"
printf '%-44s %s\n' "gcc -fanalyzer warnings:" "$gcc_w"
printf '%-44s %s\n' "cppcheck findings (error/warning):" "$cpp_w ($cpp_e)"
printf '%-44s %s\n' "flawfinder hits (risk level 4-5):" "$ff ($ff4)"
printf '%-44s %s\n' "valgrind errors (all scenarios):" "$vg_err"
printf '%-44s %s\n' "valgrind aborted by heap corruption:" "$vg_crash"
printf '%-44s %s\n' "valgrind definitely lost (bytes):" "$vg_lost"
printf '%-44s %s\n' "ASan/LSan/UBSan reports:" "$asan"

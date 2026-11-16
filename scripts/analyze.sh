#!/usr/bin/env bash
# Статичний і динамічний аналіз. Журнали зберігаються в logs/<етап>/
STAGE=${1:-before}
OUT=logs/$STAGE
mkdir -p "$OUT"

echo "[1/5] gcc -Wall -Wextra -fanalyzer"
gcc -std=c11 -Wall -Wextra -fanalyzer -c -o /dev/null src/contacts.c > "$OUT/gcc-analyzer.log" 2>&1

echo "[2/5] cppcheck"
cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem --suppress=checkersReport \
  --template='{file}:{line}: {severity}: {message} [{id}]' src/ > "$OUT/cppcheck.log" 2>&1

echo "[3/5] flawfinder"
flawfinder --minlevel=1 src/ > "$OUT/flawfinder.log" 2>&1

echo "[4/5] valgrind memcheck"
bash scripts/scenarios.sh "valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./contacts" \
  > "$OUT/valgrind.log" 2>&1

echo "[5/5] AddressSanitizer + UBSan"
ASAN_OPTIONS=detect_leaks=1 bash scripts/scenarios.sh "./contacts-asan" > "$OUT/asan.log" 2>&1

bash scripts/summary.sh "$STAGE" | tee "$OUT/summary.txt"

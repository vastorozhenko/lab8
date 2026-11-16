#!/usr/bin/env bash
# Порівняльна таблиця результатів до і після виправлень -> logs/comparison.md
{
  echo '| Metric | Before | After |'
  echo '|---|---:|---:|'
  paste -d'|' <(tail -n +2 logs/before/summary.txt) <(tail -n +2 logs/after/summary.txt) |
    awk -F'|' '{ m=$1; sub(/:.*/, "", m); b=$1; sub(/^[^:]*: */, "", b); a=$2; sub(/^[^:]*: */, "", a);
                 printf "| %s | %s | %s |\n", m, b, a }'
} | tee logs/comparison.md

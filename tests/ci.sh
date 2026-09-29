#!/usr/bin/env bash
#
# OpenKey engine CI gate: build, run unit tests, enforce the 85% line
# coverage floor on the engine translation units.
#
# Usage: ./ci.sh   (run from tests/)
#
set -euo pipefail

cd "$(dirname "$0")"

ENGINE_DIR="$(cd ../Sources/OpenKey/engine && pwd)"
BUILD_DIR="build"
FILES="Engine Vietnamese Macro ConvertTool SmartSwitchKey"

echo "==> Building and running unit tests"
make test

echo "==> Measuring line coverage"
find "${BUILD_DIR}" -name '*.gcda' -delete
"${BUILD_DIR}/ut_engine" > /dev/null

REPORT="coverage_report.txt"
{
  echo "OpenKey engine unit-test coverage report"
  echo "Tests: $(grep -E '^Total tests:' <(make test 2>/dev/null | tail -1) || true)"
  echo "----------------------------------------------"
  printf '%-16s %7s  %s\n' "File" "Coverage" "Lines"
} > "${REPORT}"

total_exec=0
total_lines=0
for o in ${FILES}; do
  line=$(cd "${BUILD_DIR}" && gcov -o "./ut_engine-${o}.gcno" "${ENGINE_DIR}/${o}.cpp" 2>/dev/null \
    | grep -A1 "File '.*/${o}.cpp'" | grep "Lines executed")
  # gcov prints e.g. "Lines executed:82.54% of 1128"
  pct=$(printf '%s' "${line}" | sed -n 's/.*executed:\([0-9.]*\)%.*/\1/p')
  n=$(printf '%s' "${line}" | sed -n 's/.*of \([0-9]*\)$/\1/p')
  exec_=$(awk -v p="${pct}" -v n="${n}" 'BEGIN { printf "%d", (p*n/100.0)+0.5 }')
  total_exec=$((total_exec + exec_))
  total_lines=$((total_lines + n))
  printf '%-16s %6.2f%%  (%d/%d lines)\n' "${o}" "${pct}" "${exec_}" "${n}"
  printf '%-16s %6.2f%%  (%d/%d lines)\n' "${o}" "${pct}" "${exec_}" "${n}" >> "${REPORT}"
done

total_pct=$(awk -v e="${total_exec}" -v l="${total_lines}" 'BEGIN { printf "%.2f", (100.0*e/l) }')
echo "----------------------------------------------"
printf 'TOTAL             %6.2f%%  (%d/%d lines)\n' "${total_pct}" "${total_exec}" "${total_lines}"
printf 'TOTAL             %6.2f%%  (%d/%d lines)\n' "${total_pct}" "${total_exec}" "${total_lines}" >> "${REPORT}"

awk -v total="${total_pct}" -v floor=85.0 'BEGIN {
  if (total < floor) {
    printf "FAIL: total coverage %.2f%% is below the %.1f%% floor\n", total, floor
    exit 1
  }
  printf "PASS: total coverage %.2f%% meets the %.1f%% floor\n", total, floor
}'
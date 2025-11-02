#!/bin/bash
set -e

cd "$(dirname "$0")/build"

echo "🧪 Running tests..."
ctest --output-on-failure || true

echo "📊 Collecting coverage data..."
lcov --capture --directory . --output-file coverage.info

echo "🧹 Filtering external files..."
lcov --remove coverage.info \
    '/usr/*' \
    '*/test/*' \
    '*/external/*' \
    '*/vcpkg/*' \
    '*/build/_deps/*' \
    --output-file coverage_filtered.info

echo "📈 Generating HTML report..."
genhtml coverage_filtered.info --output-directory coverage_html

echo ""
echo "✅ Coverage report generated!"
echo "📂 File: $(pwd)/coverage_html/index.html"
echo ""

lcov --summary coverage_filtered.info

xdg-open coverage_html/index.html 2>/dev/null || true

#!/usr/bin/env bash
set -euo pipefail

BENCH_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$(cd "$BENCH_DIR/../build" && pwd)"
SURFRECON="$BUILD_DIR/surfrecon"
SURFTRIMMER="$BUILD_DIR/surftrimmer"
SURFREMESH="$BUILD_DIR/surfremesh"

if [ ! -x "$SURFRECON" ]; then
    echo "ERROR: surfrecon not found at $SURFRECON. Build first."
    exit 1
fi

echo "PoissonSurfer CLI Benchmarks"
echo "============================"
echo ""

# -------- surfrecon scaling --------
echo "--- surfrecon: depth scaling (sphere) ---"
printf "%-8s %-12s %-12s %-12s %-12s\n" "Depth" "Wall(s)" "Verts" "Polys" "FileSize"
printf "%s\n" "--------------------------------------------------------"

for depth in 4 5 6 7 8; do
    infile="/tmp/bench_sphere.ply"
    outfile="/tmp/bench_sphere_out_d${depth}.ply"

    # Generate sphere PLY if not present
    if [ ! -f "$infile" ]; then
        python3 -c "
import struct, math, random, sys
random.seed(42)
n = 5000
with open('$infile', 'w') as f:
    f.write('ply\nformat ascii 1.0\nelement vertex %d\n' % n)
    f.write('property float x\nproperty float y\nproperty float z\n')
    f.write('property float nx\nproperty float ny\nproperty float nz\n')
    f.write('end_header\n')
    for i in range(n):
        u = random.random()
        v = random.random()
        theta = 2.0 * math.pi * u
        phi = math.acos(2.0 * v - 1.0)
        x = 0.3 * math.sin(phi) * math.cos(theta) + 0.5
        y = 0.3 * math.sin(phi) * math.sin(theta) + 0.5
        z = 0.3 * math.cos(phi) + 0.5
        nx = math.sin(phi) * math.cos(theta)
        ny = math.sin(phi) * math.sin(theta)
        nz = math.cos(phi)
        f.write('%f %f %f %f %f %f\n' % (x,y,z,nx,ny,nz))
" 2>/dev/null
    fi

    start=$(python3 -c "import time; print(time.time())")
    "$SURFRECON" --in "$infile" --out "$outfile" --depth "$depth" --verbose 2>/dev/null || true
    end=$(python3 -c "import time; print(time.time())")
    elapsed=$(python3 -c "print($end - $start)")

    verts=0
    polys=0
    if [ -f "$outfile" ]; then
        verts=$(grep -c "end_header" "$outfile" 2>/dev/null || true)
        # count vertex lines (lines after header that have 3 float groups = position)
        header_lines=$(grep -n "end_header" "$outfile" | cut -d: -f1)
        if [ -n "$header_lines" ]; then
            total=$(wc -l < "$outfile")
            verts=$(( total - header_lines ))
        fi
        fsize=$(stat -f%z "$outfile" 2>/dev/null || stat --format=%s "$outfile" 2>/dev/null || echo 0)
    fi
    printf "%-8s %-12.3f %-12s %-12s %-12s\n" "$depth" "$elapsed" "$verts" "$polys" "$(numfmt --to=iec $fsize 2>/dev/null || echo "${fsize}B")"
done

# cleanup
rm -f /tmp/bench_sphere.ply /tmp/bench_sphere_out_d*.ply

echo ""
echo "CLI benchmarks complete."

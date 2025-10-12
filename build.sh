#!/bin/bash
set -e

echo "Building libvss-types..."

# Create build directory
mkdir -p build
cd build

# Configure
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DVSS_TYPES_BUILD_TESTS=ON \
    -DVSS_TYPES_BUILD_EXAMPLES=ON

# Build
make -j$(nproc)

echo ""
echo "Build complete! To install:"
echo "  cd build && sudo make install"
echo ""
echo "To run tests:"
echo "  cd build && ctest"
echo ""
echo "To run examples:"
echo "  cd build && ./examples/basic_example"
echo "  cd build && ./examples/struct_example"

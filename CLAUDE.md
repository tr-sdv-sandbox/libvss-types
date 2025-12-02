# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

libvss-types is a backend-agnostic C++17 library providing type-safe VSS (Vehicle Signal Specification) type definitions. Namespace: `vss::types`. Main include: `#include <vss/types/types.hpp>`.

## Build Commands

```bash
# Dev environment (optional)
nix develop

# Quick build
./build.sh

# Manual build
mkdir -p build && cd build
cmake .. -DVSS_TYPES_BUILD_TESTS=ON -DVSS_TYPES_BUILD_EXAMPLES=ON
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Run single test
./tests/test_value
./tests/test_struct
./tests/test_quality
./tests/test_struct_advanced
./tests/test_vss_integration
```

## Architecture

### Headers (include/vss/types/)

- **value.hpp** - `Value` variant type (primitives, arrays, structs via `std::shared_ptr<StructValue>`)
- **struct.hpp** - `StructValue`, `StructDefinition`, `FieldDefinition`, `StructRegistry`
- **quality.hpp** - `SignalQuality` enum, `QualifiedValue<T>` template
- **types.hpp** - Main header that includes all others

### Sources (src/)

Each header has a corresponding .cpp with non-inline implementations.

## Testing

Tests use GTest. The VSS integration test fetches nlohmann_json automatically if not found (JSON is test-only, not a library dependency).

# libvss-types Tests

## Test Suite

| Test | Coverage |
|------|----------|
| `test_value.cpp` | Value types, type introspection, compatibility |
| `test_struct.cpp` | Struct definitions, registry, validation |
| `test_struct_advanced.cpp` | Nested structs, struct arrays |
| `test_quality.cpp` | Signal quality indicators |
| `test_vss_integration.cpp` | VSS specification compliance |

## VSS Integration Test

Validates that libvss-types supports the complete VSS specification including all primitive types, arrays, and structs (VSS 4.0).

### Key Points

- **JSON parsing is test-only** - The library itself has no JSON dependency
- Struct definitions come from runtime metadata in production, not file parsing
- `nlohmann/json` is automatically fetched via CMake FetchContent

### Running

```bash
cd build
cmake ..
make test_vss_integration
./tests/test_vss_integration
```

### Test Coverage

- All VSS primitive types (bool, int32, uint32, int64, uint64, float, double, string)
- All array types
- Struct types with nested structs
- Struct arrays
- Type validation and compatibility

### Test Configuration

`vss_test.json` contains comprehensive VSS examples:
- Primitive and array sensors for all types
- `Position` struct (coordinates)
- `DeliveryInfo` struct with nested `Position`
- `Route` struct with `Waypoint` array

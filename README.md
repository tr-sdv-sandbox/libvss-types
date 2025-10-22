# libvss-types

Backend-agnostic VSS type definitions for C++17+.

Provides type-safe VSS (Vehicle Signal Specification) types for use across multiple VSS implementations.

## Features

- VSS 4.0 support (structs, arrays, nested types)
- Signal quality indicators (VALID/INVALID/NOT_AVAILABLE)
- Type-safe value variant with compile-time and runtime checking
- No dependencies on protobuf, gRPC, or KUKSA
- Minimal dependencies, header-only where possible

## Scope

This library provides:
- Type definitions (`Value`, `ValueType`, `StructDefinition`)
- Signal quality indicators (`SignalQuality`, `QualifiedValue`)
- Type utilities (conversions, compatibility checks)

This library does NOT provide:
- Networking or communication
- Signal processing or transformations
- Backend-specific implementations

## Quick Start

### Installation

```bash
cd libvss-types
mkdir build && cd build
cmake ..
make
sudo make install
```

### CMake Integration

```cmake
find_package(vss-types REQUIRED)

target_link_libraries(your_target
    PRIVATE vss::types
)
```

### Basic Usage

```cpp
#include <vss/types/types.hpp>

using namespace vss::types;

// Simple values
Value speed = 120.5f;
Value door_locked = true;

// Array values
Value tire_pressures = std::vector<float>{2.1f, 2.2f, 2.1f, 2.2f};

// Struct values (VSS 4.0)
StructValue delivery{"DeliveryInfo"};
delivery.set_field("Address", std::string("123 Main St"));
delivery.set_field("Receiver", std::string("John Doe"));

// Qualified values (with quality indicators)
QualifiedValue<float> temp{22.5f, SignalQuality::VALID};
if (temp.is_valid()) {
    std::cout << "Temperature: " << *temp.value << "°C\n";
}
```

## Core Types

### Value Variant

```cpp
using Value = std::variant<
    std::monostate,  // Empty/unset
    // Primitives
    bool,
    int8_t, int16_t, int32_t, int64_t,
    uint8_t, uint16_t, uint32_t, uint64_t,
    float, double, std::string,
    // Arrays
    std::vector<bool>,
    std::vector<int8_t>, std::vector<int16_t>, std::vector<int32_t>, std::vector<int64_t>,
    std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint64_t>,
    std::vector<float>, std::vector<double>, std::vector<std::string>,
    // Structs (VSS 4.0)
    std::shared_ptr<StructValue>,
    std::vector<std::shared_ptr<StructValue>>
>;
```

### ValueType Enum

```cpp
enum class ValueType {
    UNSPECIFIED = 0,
    // Primitives: STRING, BOOL,
    //             INT8, INT16, INT32, INT64,
    //             UINT8, UINT16, UINT32, UINT64,
    //             FLOAT, DOUBLE
    // Arrays: STRING_ARRAY, BOOL_ARRAY,
    //         INT8_ARRAY, INT16_ARRAY, INT32_ARRAY, INT64_ARRAY,
    //         UINT8_ARRAY, UINT16_ARRAY, UINT32_ARRAY, UINT64_ARRAY,
    //         FLOAT_ARRAY, DOUBLE_ARRAY
    // Structs: STRUCT, STRUCT_ARRAY
};
```

### Signal Quality

```cpp
enum class SignalQuality {
    UNKNOWN,        // Quality not specified
    VALID,          // Signal is valid and reliable
    INVALID,        // Sensor error, out of range, etc.
    NOT_AVAILABLE   // Sensor not present or disconnected
};
```

### Qualified Values

```cpp
template<typename T>
struct QualifiedValue {
    std::optional<T> value;
    SignalQuality quality;
    std::chrono::system_clock::time_point timestamp;

    bool is_valid() const;
    bool is_invalid() const;
    bool is_not_available() const;
    bool is_stale() const;
    std::chrono::milliseconds age() const;
};

// Also available: DynamicQualifiedValue (uses Value variant)
```

## Struct Support (VSS 4.0)

### Define Struct Types

```cpp
StructDefinition position("Position", "Geographic position");
position.add_field(FieldDefinition("Latitude", ValueType::DOUBLE))
        .add_field(FieldDefinition("Longitude", ValueType::DOUBLE))
        .add_field(FieldDefinition("Altitude", ValueType::DOUBLE));

StructRegistry registry;
registry.register_struct(std::move(position));
```

### Create Struct Values

```cpp
StructValue pos{"Position"};
pos.set_field("Latitude", 37.7749);
pos.set_field("Longitude", -122.4194);
pos.set_field("Altitude", 16.0);
```

### Validate Structs

```cpp
auto error = validate_struct(pos, registry);
if (error.has_value()) {
    std::cerr << "Validation failed: " << *error << "\n";
}
```

### Nested Structs

```cpp
// Struct can contain other structs
StructDefinition delivery_info("DeliveryInfo");
delivery_info.add_field(FieldDefinition("Address", ValueType::STRING))
             .add_field(FieldDefinition("Location", ValueType::STRUCT));

// Location field points to another struct
auto location_field = delivery_info.get_field("Location");
location_field->struct_type_name = "Position";
```

## Type Utilities

### Type Introspection

```cpp
Value v = 120.5f;
ValueType type = get_value_type(v);  // ValueType::FLOAT

const char* type_str = value_type_to_string(type);  // "FLOAT"

auto parsed = value_type_from_string("float");  // ValueType::FLOAT
```

### Type Compatibility

```cpp
// float ↔ double are compatible
are_types_compatible(ValueType::FLOAT, ValueType::DOUBLE);  // true

// int32 ↔ int64 are compatible
are_types_compatible(ValueType::INT32, ValueType::INT64);  // true

// uint32 ↔ uint64 are compatible
are_types_compatible(ValueType::UINT32, ValueType::UINT64);  // true

// Incompatible types
are_types_compatible(ValueType::FLOAT, ValueType::INT32);  // false
are_types_compatible(ValueType::INT32, ValueType::UINT32);  // false (signed ↔ unsigned)
```

### Type Checks

```cpp
is_primitive(ValueType::FLOAT);       // true
is_array(ValueType::FLOAT_ARRAY);     // true
is_struct(ValueType::STRUCT);         // true
is_empty(value);                      // true if value is std::monostate
```

## Examples

See the `examples/` directory for complete examples:

- `basic_example.cpp` - Basic value types, quality indicators, type utilities
- `struct_example.cpp` - VSS 4.0 struct usage, validation, nested structs

Build and run:
```bash
cd build
./examples/basic_example
./examples/struct_example
```

## Testing

```bash
cd build
cmake .. -DVSS_TYPES_BUILD_TESTS=ON
make
ctest
```

Test suite includes:
- **Value types** - Type conversions, introspection, compatibility
- **Structs** - Definitions, registry, validation, nested structs, arrays
- **Quality** - Signal quality indicators, qualified values
- **VSS integration** - Validates complete VSS spec support (all types + structs)

The VSS integration test uses a test-only JSON parser to validate type completeness. The library itself has no JSON dependency - struct definitions come from runtime metadata in production.

## Rationale

### Why a separate library?
- Single source of truth for VSS types
- Shared across all VSS C++ implementations
- Consistent semantics and behavior

### Why not use vss-tools protobuf output?

vss-tools generates **static tree schemas** for the entire VSS model:
```protobuf
message Vehicle {
  Speed speed = 1;
  Cabin cabin = 2;
  // ... thousands of signals
}
```

This is for **compile-time type safety** in generated code.

libvss-types provides **dynamic value transport**:
```cpp
Value value = 120.5f;  // Can be any VSS type at runtime
```

This is for **runtime signal handling** in brokers and transformers.

**Different use cases, different tools.**

## Design Decisions

**std::variant for Value type**
- Type-safe at compile time
- No heap allocations for primitives
- No vtable overhead

**shared_ptr for structs**
- Avoids circular dependency (struct can contain struct)
- std::variant requires complete types
- Allows nested structs without recursion limits

**Backend-agnostic**
- No protobuf, gRPC, or KUKSA dependencies
- Conversions handled by client libraries

## Roadmap
- [ ] Signal metadata (min/max, unit, description)
- [ ] Type coercion utilities (safe conversions)
- [ ] Protobuf interop helpers??

## Contributing

New features should:
1. Be fundamental to VSS type representation
2. Have no external dependencies
3. Be well-tested

## License

Apache 2.0

## See Also

- [VSS Specification](https://covesa.github.io/vehicle_signal_specification/) - Official VSS documentation
- [vss-tools](https://github.com/COVESA/vss-tools) - VSS tooling

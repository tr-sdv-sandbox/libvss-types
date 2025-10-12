# libvss-types

**Backend-agnostic VSS type definitions for C++**

A shared foundation library providing type-safe, runtime-flexible VSS (Vehicle Signal Specification) types for C++17+. Designed to be used by multiple VSS implementations including libVSSDAG, kuksa-cpp-client, and other VSS tools.

## Features

- ✅ **VSS 4.0 Support** - Full support for structs, arrays, and nested types
- ✅ **Signal Quality** - Sophisticated error handling (VALID/INVALID/NOT_AVAILABLE/STALE/OUT_OF_RANGE)
- ✅ **Type-Safe** - Compile-time and runtime type checking
- ✅ **Backend-Agnostic** - No dependencies on protobuf, gRPC, or KUKSA
- ✅ **Lightweight** - Header-only where possible, minimal dependencies
- ✅ **Well-Tested** - Comprehensive test suite with Google Test

## Design Philosophy

**libvss-types is deliberately minimal:**
- ✅ Type definitions (`Value`, `ValueType`, `StructDefinition`)
- ✅ Signal quality indicators (`SignalQuality`, `QualifiedValue`)
- ✅ Type utilities (conversions, compatibility checks)
- ❌ NO networking (that's for client libraries)
- ❌ NO KUKSA-specific code (keep it generic)
- ❌ NO signal processing (that's for libVSSDAG)

This library is the **shared foundation** that other libraries build upon.

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
    bool, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string,
    // Arrays
    std::vector<bool>, std::vector<int32_t>, ..., std::vector<std::string>,
    // Structs (VSS 4.0)
    std::shared_ptr<StructValue>,
    std::vector<std::shared_ptr<StructValue>>
>;
```

### ValueType Enum

```cpp
enum class ValueType {
    UNSPECIFIED = 0,
    // Primitives: STRING, BOOL, INT32, INT64, UINT32, UINT64, FLOAT, DOUBLE
    // Arrays: STRING_ARRAY, BOOL_ARRAY, INT32_ARRAY, ... DOUBLE_ARRAY
    // Structs: STRUCT, STRUCT_ARRAY
};
```

### Signal Quality

```cpp
enum class SignalQuality {
    UNKNOWN,        // Quality not specified
    VALID,          // Signal is valid and reliable
    INVALID,        // Sensor error, out of range, etc.
    NOT_AVAILABLE,  // Sensor not present or disconnected
    STALE,          // Value is outdated
    OUT_OF_RANGE    // Value exceeds min/max bounds
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

## Integration with Other Libraries

### libVSSDAG (CAN Transformation)

```cpp
// Use for signal values and quality indicators
DynamicQualifiedValue signal{Value{speed_value}, SignalQuality::VALID};

// Use struct definitions for complex CAN mappings
StructValue battery_state{"BatteryState"};
battery_state.set_field("Voltage", voltage);
battery_state.set_field("Current", current);
battery_state.set_field("Temperature", temp);
```

### kuksa-cpp-client (KUKSA Client)

```cpp
// Convert between libvss-types and KUKSA protobuf
Value vss_value = 120.5f;
// ... convert to KUKSA Datapoint protobuf

// Use QualifiedValue for rich error handling
QualifiedValue<float> speed = client->get_qualified(speed_handle);
if (speed.is_stale()) {
    LOG(WARNING) << "Speed value is stale (age: " << speed.age().count() << "ms)";
}
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

Tests use Google Test and cover:
- Value type conversions
- Struct creation and validation
- Signal quality indicators
- Type compatibility checks

## Rationale

### Why a separate library?

**Before:**
- libVSSDAG had its own struct types
- sdk-x (kuksa-cpp-client) had its own types
- Duplication, inconsistency, maintenance burden

**After:**
- Single source of truth for VSS types
- Shared across all VSS C++ implementations
- Fix bugs once, benefit everywhere
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

### std::variant instead of inheritance

✅ **Chosen:** `using Value = std::variant<bool, int32_t, ...>`

❌ **Not chosen:** Virtual base class `class Value { virtual ~Value(); }`

**Why:**
- std::variant is type-safe at compile time
- No heap allocations for primitives
- No vtable overhead
- std::visit for type-safe dispatch

### shared_ptr for structs

✅ **Chosen:** `std::shared_ptr<StructValue>` in Value variant

**Why:**
- Avoids circular dependency (struct can contain struct)
- std::variant requires complete types
- Efficient copying (just pointer copy)
- Allows nested structs without recursion limits

### Backend-agnostic design

❌ **No protobuf dependency** - Keep protobuf conversion in client libraries
❌ **No gRPC dependency** - This is pure types, no networking
❌ **No KUKSA dependency** - Works with any VSS backend

**Why:**
- Maximum reusability
- Faster compilation
- Smaller binary size
- Use in embedded systems

## Roadmap

- [ ] YAML/JSON serialization for types
- [ ] Load struct definitions from vss-tools JSON output
- [ ] Signal metadata (min/max, unit, description)
- [ ] Type coercion utilities (safe conversions)
- [ ] Protobuf interop helpers (optional addon)

## Contributing

This library is designed to be **stable and minimal**. New features should:
1. Be fundamental to VSS type representation
2. Benefit multiple consumers (libVSSDAG, kuksa-cpp-client, etc.)
3. Have no external dependencies
4. Be well-tested

## License

Apache 2.0

## See Also

- [libVSSDAG](https://github.com/skarlsson/libVSSDAG) - CAN → VSS transformation with DAG processing
- [kuksa-cpp-client](../sdk-x) - C++ client for KUKSA databroker
- [VSS Specification](https://covesa.github.io/vehicle_signal_specification/) - Official VSS documentation
- [vss-tools](https://github.com/COVESA/vss-tools) - VSS tooling (code generation, validation)

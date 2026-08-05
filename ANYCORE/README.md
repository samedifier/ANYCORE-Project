# ANYCORE256

ANYCORE is a lightweight, high-performance runtime designed around deterministic entity and scene management.

## Features

- Entity creation, destruction, restoration, locking and unlocking
- Bulk entity operations
- Scene and chunk management
- Model management
- User-defined vertex formats
- 2D and 3D transforms
- Configurable API through `configuration.h`
- Dynamically loaded modules and behaviours
- Function-pointer based behaviour interface
- Configurable floating-point precision

## Project Structure

```text
ANYCORE256/
├── anycore_wrapper.h
├── configuration.h
├── primitives.h
├── include/
└── src/
```

### `include/`

Public headers for ANYCORE's managers and utilities.

### `src/`

ANYCORE's implementation.

### `anycore_wrapper.h`

The function table exposed to behaviours. Behaviours receive the wrapper through ANYCORE's injection system and use it to access the available ANYCORE functionality.

### `configuration.h`

Compile-time configuration for ANYCORE.

ANYCORE can be configured for either 2D or 3D operation:

#define SPACE_2D 2
#define SPACE_3D 3

#define SPACE SPACE_3D

It can also be configured to use either single-precision (float) or double-precision (double) floating-point values:

#define PRESICION_FLOAT  4
#define PRESICION_DOUBLE 8

#define PRESICION_ PRESICION_FLOAT

PRESICION is intentionally misspelled. This is a known and intentional part of the current configuration interface and is kept unchanged to preserve compatibility with existing code. Do not rename for the purpose of correcting.

Individual functionality can be enabled or disabled through ANYCORE_ENABLE_* definitions.

For example:

#define ANYCORE_ENABLE_CREATE_ENTITY       ON
#define ANYCORE_ENABLE_CREATE_ENTITY_BULK  ON
#define ANYCORE_ENABLE_DESTROY_ENTITY      ON

ON is 1 and OFF is 0.

Some core functions, including the execution and lifecycle functions required by ANYCORE, are always available.

### `primitives.h`

The shared source of common data types used by ANYCORE, modules and behaviours.

When developing a module or behaviour for ANYCORE, use the primitives.h provided with the same ANYCORE release so that all components use the same shared type definitions.

Do not modify `primitives.h`. It is a shared interface between ANYCORE, modules and behaviours and must remain unchanged.

## Building

ANYCORE does not require CMake.

The project can be compiled directly using the provided build script.

```sh
./compile.sh
```

## Behaviours and Modules

ANYCORE supports dynamically loaded modules and behaviours.

Behaviours can receive an `ANYCORE_Wrapper` and use the exposed function table to interact with ANYCORE without directly accessing its internal implementation.

More detailed information about writing modules and behaviours will be provided separately.

## Demos

Example demos are provided separately from the ANYCORE source.

The demos demonstrate entity creation, scene updates, transforms, destruction and restoration.

Throughput tests are provided separately from the demonstration programs.

## Platform

The initial public release targets Linux.

ANYCORE itself is intentionally kept relatively independent from platform-specific functionality. Additional platform implementations may be provided in the future.

## Configuration and Performance

ANYCORE exposes compile-time configuration so applications can disable functionality they do not require.

This allows a build to contain only the functionality needed by the application and can reduce unnecessary code and performance overhead.

Performance characteristics depend on the selected configuration, compiler, hardware and workload.

(Disabling unused functionality can affect performance. The impact depends on the functionality removed and the workload.)

## License

See `LICENSE`.

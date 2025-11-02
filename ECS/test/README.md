# ECS Unit Tests

This directory contains comprehensive unit tests for all ECS managers and core logic using [doctest](https://github.com/doctest/doctest).

## Test Files

### Manager Tests
- **`asset_manager_test.cpp`** - Tests for AssetManager (texture/sound caching)
- **`render_manager_test.cpp`** - Tests for RenderManager (window, scaling, rendering)
- **`physics_manager_test.cpp`** - Tests for PhysicsManager (collision detection, spatial hashing)
- **`audio_manager_test.cpp`** - Tests for AudioManager (volume control, mute/unmute)
- **`messaging_manager_test.cpp`** - Tests for MessagingManager (EventBus, CommandDispatcher, MessageQueue)

### Core Logic Tests
- **`ecs_core_test.cpp`** - Tests for Registry, Components, Systems, and Zipper
  - Entity spawning and lifecycle
  - Component add/remove/retrieve
  - View and zipper iteration
  - System updates
  - Integration scenarios

## Building Tests

Tests are automatically built if `doctest` is available via vcpkg.

```bash
cd ECS
mkdir -p build && cd build
cmake ..
make

# Build output:
# ✓ asset_manager_test
# ✓ render_manager_test  
# ✓ physics_manager_test
# ✓ audio_manager_test
# ✓ messaging_manager_test
# ✓ ecs_core_test
```

## Running Tests

### Run All Tests
```bash
cd build
ctest --output-on-failure
```

### Run Specific Test
```bash
./asset_manager_test          # AssetManager tests
./render_manager_test          # RenderManager tests
./physics_manager_test         # PhysicsManager tests
./audio_manager_test           # AudioManager tests
./messaging_manager_test       # MessagingManager tests
./ecs_core_test                # ECS core logic tests
```

### Run with Verbose Output
```bash
./ecs_core_test -s             # Show all test names
./ecs_core_test --duration     # Show test execution time
```

### Run Specific Test Suite
```bash
./ecs_core_test -ts="Registry Tests"
./physics_manager_test -ts="PhysicsManager Tests"
```

### Run Specific Test Case
```bash
./ecs_core_test -tc="Registry can spawn entities"
./asset_manager_test -tc="AssetManager caches textures correctly"
```

## Test Coverage

### AssetManager (6 test cases)
- ✓ Singleton pattern
- ✓ Initialization
- ✓ Texture caching
- ✓ Sound caching
- ✓ Missing file handling
- ✓ Resource unloading
- ✓ Resource count tracking

### RenderManager (12 test cases)
- ✓ Singleton pattern
- ✓ Initialization (default, scaled, windowed)
- ✓ Screen scaling (X, Y, Width, Height)
- ✓ Clear color management
- ✓ Camera access
- ✓ Sprite batch access
- ✓ FPS and delta time
- ✓ Monitor dimensions
- ✓ Frame begin/end

### PhysicsManager (13 test cases)
- ✓ Singleton pattern
- ✓ Initialization
- ✓ Entity updates
- ✓ Collision detection (AABB, Circle)
- ✓ Spatial hash queries
- ✓ Entity removal
- ✓ Clear all entities
- ✓ Overlapping/non-overlapping detection
- ✓ Multiple entity handling

### AudioManager (14 test cases)
- ✓ Singleton pattern
- ✓ Initialization
- ✓ Volume control (master, music, sfx)
- ✓ Effective volume calculation
- ✓ Mute/unmute functionality
- ✓ Volume preservation
- ✓ Safe multiple mute/unmute calls
- ✓ Player access (music, sfx)
- ✓ Update functionality
- ✓ Shutdown and re-initialization

### MessagingManager (12 test cases)
- ✓ Singleton pattern
- ✓ Initialization
- ✓ EventBus subscribe/emit
- ✓ Multiple subscribers
- ✓ Unsubscribe
- ✓ Different event types
- ✓ CommandDispatcher register/dispatch
- ✓ Missing handler errors
- ✓ MessageQueue push/pop
- ✓ FIFO ordering
- ✓ Clear all
- ✓ Update functionality

### ECS Core Logic (20+ test cases)
- ✓ Registry entity spawning
- ✓ Component add/remove/retrieve
- ✓ has_component checks
- ✓ Entity killing
- ✓ View iteration
- ✓ get_if array access
- ✓ Component default values
- ✓ Zipper multi-component iteration
- ✓ Zipper modification
- ✓ System updates
- ✓ System registry access
- ✓ Complete ECS workflow
- ✓ Entity lifecycle management
- ✓ Multiple systems processing

## Test Output Example

```
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases: 20 | 20 passed | 0 failed | 0 skipped
[doctest] assertions: 85 | 85 passed | 0 failed |
[doctest] Status: SUCCESS!
```

## CI/CD Integration

Tests can be integrated into CI pipelines:

```yaml
# GitHub Actions example
- name: Build ECS Tests
  run: |
    cd ECS/build
    cmake ..
    make

- name: Run ECS Tests
  run: |
    cd ECS/build
    ctest --output-on-failure
```

## Adding New Tests

1. Create test file: `test/my_feature_test.cpp`
2. Use doctest macros:
```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_SUITE("MyFeature Tests") {
    TEST_CASE("Feature works correctly") {
        CHECK(1 + 1 == 2);
        REQUIRE(ptr != nullptr);
    }
}
```
3. Add to CMakeLists.txt:
```cmake
add_executable(my_feature_test test/my_feature_test.cpp)
target_link_libraries(my_feature_test PRIVATE ECS raylib doctest::doctest)
add_test(NAME MyFeatureTest COMMAND my_feature_test)
```

## Debugging Tests

```bash
# Run with GDB
gdb ./ecs_core_test
(gdb) run
(gdb) backtrace

# Run with Valgrind (memory leaks)
valgrind --leak-check=full ./ecs_core_test
```

## Performance Benchmarks

Some tests include performance validations:
- PhysicsManager: O(n) vs O(n²) collision detection
- AssetManager: Cache hit vs cache miss
- MessagingManager: Event dispatch overhead

## Dependencies

- **doctest** - Test framework (header-only)
- **raylib** - Required for rendering/audio tests
- **ECS library** - Tested library

## Notes

- Tests use minimal raylib windows (1x1) to reduce overhead
- `SetTraceLogLevel(LOG_ERROR)` suppresses raylib spam
- Temporary files are cleaned up after tests
- All tests are self-contained and independent

---

**Total Test Count:** 77+ test cases across 6 test executables  
**Coverage:** All 5 managers + ECS core logic (Registry, Components, Systems, Zipper)  
**Run Time:** ~2-5 seconds for full suite

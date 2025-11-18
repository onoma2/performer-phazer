# TDD Methodology - PEW|FORMER Project Conventions

**Date**: 2025-11-18
**Purpose**: Document TDD practices, conventions, and lessons learned from implementation work

---

## Overview

The PEW|FORMER project follows strict **Test-Driven Development (TDD)** methodology for all feature implementations and bug fixes. This document captures the conventions, patterns, and key considerations discovered during development.

---

## Core TDD Cycle

### 1. RED → GREEN → REFACTOR

**RED**: Write a failing test that exposes the bug or validates the new feature
- Test should fail for the right reason
- Verify the test actually runs (avoid false passes)
- Keep tests focused and minimal

**GREEN**: Write minimal code to make the test pass
- Don't over-engineer
- Hard-code if necessary to pass the test
- Add only what's needed for this specific test

**REFACTOR**: Clean up code while keeping tests green
- Remove duplication
- Improve naming
- Extract methods
- Optimize algorithms

### 2. Iterative Approach

Build features **incrementally**:
1. Start with simplest possible test case
2. Add one test at a time
3. Implement minimal code to pass
4. Refactor before moving to next test
5. Repeat until feature complete

---

## Test Framework: UnitTest.h

### Framework Structure

PEW|FORMER uses a **custom lightweight test framework** (`src/test/UnitTest.h`), NOT external frameworks like Catch2 or Google Test.

**Basic Structure**:
```cpp
#include "UnitTest.h"

UNIT_TEST("TestName") {

CASE("test case description") {
    // Arrange
    int expected = 5;

    // Act
    int actual = myFunction();

    // Assert
    expectEqual(actual, expected, "values should match");
}

CASE("another test case") {
    // ...
}

} // End UNIT_TEST
```

### Key Macros

**Test Declaration**:
```cpp
UNIT_TEST("TestName")  // Defines a test suite
```

**Test Cases**:
```cpp
CASE("description")    // Individual test case within suite
```

**Assertions**:
```cpp
expectEqual(a, b, "message")       // Assert a == b
expectTrue(condition, "message")   // Assert condition is true
expectFalse(condition, "message")  // Assert condition is false
```

**Important**: NO `REQUIRE()`, `CHECK()`, `SECTION()` - those are Catch2/Catch, NOT UnitTest.h!

---

## Type Safety Considerations

### The expectEqual() Template

The `expectEqual()` macro uses a **template that requires exact type matches**:

```cpp
template<typename T>
static void expectEqual(T a, T b, const Location &location, const char *msg = nullptr) {
    // ...
}
```

**Key Insight**: Both arguments must be the **exact same type** or the template will fail to match.

### Common Type Mismatches

#### 1. uint8_t vs unsigned int

**Problem**: `uint8_t` (unsigned char) doesn't match `unsigned int` literals

```cpp
// ❌ WRONG - Type mismatch
uint8_t value = 5;
expectEqual(value, 5u, "should be 5");  // Error: uint8_t vs unsigned int
```

**Solution**: Cast to match types

```cpp
// ✅ CORRECT
uint8_t value = 5;
expectEqual((unsigned int)value, 5u, "should be 5");
```

#### 2. int16_t vs int

**Problem**: `int16_t` (short) doesn't match `int` literals

```cpp
// ❌ WRONG - Type mismatch
int16_t currentValue = accumulator.currentValue();  // Returns int16_t
expectEqual(currentValue, 0, "should be 0");  // Error: int16_t vs int
```

**Solution**: Cast to int

```cpp
// ✅ CORRECT
expectEqual((int)accumulator.currentValue(), 0, "should be 0");
```

#### 3. bool is fine

Booleans work without casting:

```cpp
// ✅ CORRECT
bool enabled = true;
expectEqual(enabled, true, "should be enabled");
expectTrue(enabled, "should be enabled");  // Even better!
```

### Type Casting Best Practices

**Pattern**: Cast the **variable**, not the literal

```cpp
// ✅ GOOD - Cast variable to match literal type
expectEqual((int)value, 42, "message");
expectEqual((unsigned int)id, 0u, "message");

// ❌ AVOID - Confusing to cast literals
expectEqual(value, (int16_t)42, "message");
```

**Rationale**: Casting the variable makes intent clearer and matches the literal's natural type.

---

## Naming Conventions

### Test File Naming

**Pattern**: `Test<ComponentName>.cpp`

Examples:
- `TestGateStruct.cpp` - Tests for Gate struct
- `TestRTrigAccumTicking.cpp` - Tests for RTRIG accumulator ticking
- `TestRTrigEdgeCases.cpp` - Tests for edge cases
- `TestAccumulator.cpp` - Tests for Accumulator class

### Test Case Naming

**Pattern**: Descriptive sentence fragments in lowercase

```cpp
CASE("basic gate fields")
CASE("experimental shouldTickAccumulator field")
CASE("gate metadata logic - RTRIG mode with accumulator enabled")
CASE("sequence validation - null sequence handling")
```

**Guidelines**:
- Use lowercase
- Use spaces (not underscores or camelCase)
- Be specific about what's being tested
- Include context when testing modes/configurations

### Assertion Message Naming

**Pattern**: Describe the expected outcome in present tense

```cpp
expectEqual(value, 5, "value should be 5");
expectTrue(enabled, "accumulator should be enabled");
expectFalse(condition, "condition should be false");
```

**NOT**:
```cpp
// ❌ Avoid past tense
expectEqual(value, 5, "value was set to 5");

// ❌ Avoid implementation details
expectEqual(value, 5, "setValue() was called");

// ✅ Focus on expected state
expectEqual(value, 5, "value should be 5");
```

---

## API Discovery Patterns

### Don't Assume Method Names

When working with existing classes, **always check the actual API**:

```cpp
// ❌ WRONG - Assumed API
accumulator.setMin(0);        // Doesn't exist!
accumulator.setMax(10);       // Doesn't exist!
accumulator.value();          // Doesn't exist!

// ✅ CORRECT - Actual API (discovered via header inspection)
accumulator.setMinValue(0);
accumulator.setMaxValue(10);
accumulator.currentValue();
```

**Lesson Learned**: When encountering compilation errors about missing methods:
1. Read the class header file
2. Check actual method signatures
3. Don't trust naming conventions from other languages/frameworks

### Method Name Patterns Observed

**Setters**:
- `setValue()` - Simple value setter
- `setMinValue()` / `setMaxValue()` - Explicit value setters
- `setEnabled()` / `setDirection()` - Boolean/enum setters

**Getters**:
- `value()` - Common but not universal
- `currentValue()` - More explicit variant
- `enabled()` / `direction()` - Direct property names (no "get" prefix)

**Takeaway**: This project avoids "get" prefixes and uses explicit names.

---

## Feature Flag Testing

### Pattern: Conditional Compilation

Tests must work with **both flag states** (0 and 1):

```cpp
CASE("basic functionality") {
    // This test runs regardless of flag state
    Gate gate = { 100, true };
    expectEqual(gate.tick, 100u, "tick should be 100");
}

#if CONFIG_EXPERIMENTAL_SPREAD_RTRIG_TICKS
CASE("experimental feature") {
    // This test only runs when flag=1
    Gate gate = { 100, true, true, 0 };
    expectEqual(gate.shouldTickAccumulator, true, "should tick");
}
#endif
```

### Backward Compatibility Testing

**Critical**: Tests must pass with **flag=0** (stable mode):

```cpp
// Test counts change based on flag:
// flag=0: 4 basic tests run
// flag=1: 9 tests run (4 basic + 5 experimental)
```

**Verification Process**:
1. Run tests with flag=1 → all experimental features tested
2. Change flag to 0
3. Run tests again → only basic features tested
4. Verify **zero test failures** in either mode

---

## Common Pitfalls & Solutions

### 1. Wrong Test Framework

**Problem**: Using Catch2 syntax in UnitTest.h project

```cpp
// ❌ WRONG - Catch2 syntax
#include "catch.hpp"
TEST_CASE("description") {
    SECTION("subsection") {
        REQUIRE(value == 5);
    }
}
```

**Solution**: Use UnitTest.h syntax

```cpp
// ✅ CORRECT - UnitTest.h syntax
#include "UnitTest.h"
UNIT_TEST("TestName") {
CASE("test case") {
    expectEqual(value, 5, "should be 5");
}
}
```

### 2. Private Member Access

**Problem**: Test needs to access private struct/class members

```cpp
// ❌ Compilation error
NoteTrackEngine::Gate gate;  // Error: Gate is private
```

**Solution**: Move struct to public section (if appropriate for testing)

```cpp
// In NoteTrackEngine.h
public:
    struct Gate {
        uint32_t tick;
        bool gate;
    };
```

### 3. Static Constexpr in Implementation Files

**Problem**: Using static constexpr members without class qualification

```cpp
// In .cpp file
// ❌ WRONG
if (event.sequenceId == MainSequenceId) { }

// ✅ CORRECT
if (event.sequenceId == NoteTrackEngine::MainSequenceId) { }
```

**Rule**: Static constexpr members need class qualification in .cpp files.

### 4. Delayed First Tick Feature

**Problem**: Accumulator doesn't increment on first tick after reset()

```cpp
// ❌ Test fails
sequence.accumulator().reset();
const_cast<Accumulator&>(sequence.accumulator()).tick();
expectEqual(sequence.accumulator().currentValue(), 1, "should be 1");
// FAILS: currentValue() is still 0!
```

**Reason**: Delayed first tick feature prevents jump on playback start

**Solution**: Account for delayed tick in tests

```cpp
// ✅ Test passes
sequence.accumulator().reset();

// First tick is skipped (feature behavior)
const_cast<Accumulator&>(sequence.accumulator()).tick();
expectEqual((int)sequence.accumulator().currentValue(), 0, "still 0 after delayed first tick");

// Subsequent ticks work normally
const_cast<Accumulator&>(sequence.accumulator()).tick();
expectEqual((int)sequence.accumulator().currentValue(), 1, "now 1");
```

---

## Test Organization

### File Structure

```
src/tests/unit/sequencer/
├── CMakeLists.txt              # Test registration
├── TestAccumulator.cpp         # Core feature tests
├── TestGateStruct.cpp          # Struct extension tests
├── TestRTrigAccumTicking.cpp   # Integration tests
└── TestRTrigEdgeCases.cpp      # Edge case tests
```

### Test Registration

Add to `CMakeLists.txt`:

```cmake
register_sequencer_test(TestGateStruct TestGateStruct.cpp)
```

**Pattern**: `register_sequencer_test(<executable_name> <source_file>)`

### Build & Run

```bash
cd build/sim/debug

# Clean rebuild (important after flag changes!)
rm -rf CMakeFiles src/tests/unit/sequencer/CMakeFiles

# Configure
cmake ../../..

# Build specific test
make -j TestGateStruct

# Run test
./src/tests/unit/sequencer/TestGateStruct
```

**Important**: Always clean rebuild after changing feature flags!

---

## Test Coverage Guidelines

### What to Test

**1. Core Functionality**:
- Happy path (expected behavior)
- Struct field access and initialization
- Method return values
- State transitions

**2. Edge Cases**:
- Null pointers
- Invalid IDs
- Boundary values (min/max)
- State validation chains

**3. Integration Points**:
- Cross-component interactions
- Queue behavior
- Sequence lookups
- Feature flag combinations

**4. Backward Compatibility**:
- Default values
- 2-arg vs 4-arg constructors
- Flag=0 vs flag=1 behavior

### Test Case Organization

**Pattern**: Organize by feature/concern, not by method

```cpp
UNIT_TEST("ComponentName") {

// Basic functionality
CASE("basic field access") { }
CASE("construction with defaults") { }

// Feature-specific
CASE("experimental feature behavior") { }
CASE("feature flag=0 compatibility") { }

// Edge cases
CASE("null handling") { }
CASE("boundary values") { }

} // End test
```

---

## Lessons Learned

### From RTRIG Spread-Ticks Implementation

**1. Type Safety Matters**:
- Always cast to match template types
- Prefer explicit types over auto/inference
- Read compiler errors carefully

**2. Know Your Framework**:
- Don't assume Catch2 syntax
- Check actual project test framework
- Verify macro names before using

**3. API Discovery**:
- Read header files first
- Don't guess method names
- Check return types

**4. Feature Flags**:
- Test both enabled and disabled states
- Verify zero regressions with flag=0
- Clean rebuild after flag changes

**5. Domain Knowledge**:
- Understand "delayed first tick" feature
- Know accumulator behavior patterns
- Respect existing architectural decisions

**6. Incremental Development**:
- One test case at a time
- Small commits with clear messages
- Verify tests pass before moving on

---

## Quick Reference

### Common Code Patterns

**Test Structure**:
```cpp
UNIT_TEST("TestName") {
CASE("description") {
    // Arrange
    Component component;
    component.setup();

    // Act
    int result = component.method();

    // Assert
    expectEqual(result, expected, "should match");
}
}
```

**Type-Safe Comparisons**:
```cpp
expectEqual((int)int16Value, 42, "message");
expectEqual((unsigned int)uint8Value, 5u, "message");
expectTrue(boolValue, "message");
```

**Feature Flag Tests**:
```cpp
CASE("always runs") { /* ... */ }

#if CONFIG_EXPERIMENTAL_FEATURE
CASE("only when flag=1") { /* ... */ }
#endif
```

**Null Safety**:
```cpp
const Type* ptr = nullptr;
bool isValid = (ptr != nullptr);
expectEqual(isValid, false, "should be null");
```

---

## Conclusion

Successful TDD in PEW|FORMER requires:
1. ✅ Using correct test framework (UnitTest.h)
2. ✅ Type-safe assertions (cast to match types)
3. ✅ Knowing actual API (read headers)
4. ✅ Testing both flag states (backward compatibility)
5. ✅ Understanding domain features (delayed ticks, etc.)
6. ✅ Clean rebuilds (after flag changes)

**Remember**: TDD is about confidence. Each green test is proof your code works.

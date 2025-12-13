# Curve Track Wavefolder/Chaos Routing Fix

## Problem

Curve track Wavefolder and Chaos parameters behaved incorrectly when routing was enabled:
- **WavefolderFold** would jump to maximum value (1.0) instead of the routed value
- **WavefolderGain** would jump to maximum value (2.0) instead of the routed value
- **DjFilter** would jump to extremes instead of responding to routing
- **XFade** would behave incorrectly
- Parameters would not respond to CV input changes or other routing sources

## Root Cause

**File:** `src/apps/sequencer/model/CurveSequence.cpp:158-169`

**The Bug:**

The routing system defines value ranges in integer/percentage format for consistency across the UI:
- **WavefolderFold**: 0-100 (represents 0.0-1.0)
- **WavefolderGain**: 0-200 (represents 0.0-2.0)
- **DjFilter**: -100 to 100 (represents -1.0 to 1.0)
- **XFade**: 0-100 (represents 0.0-1.0)

However, the `CurveSequence::writeRouted()` function was passing these values directly to setters that expected **float values in the range 0.0-1.0** (or 0.0-2.0, -1.0 to 1.0).

**Result:** A routed value of 50 (meaning 50%) would be passed as `50.0` to `setWavefolderFold()`, which expects 0.0-1.0. The setter would clamp 50.0 to the maximum (1.0), causing the parameter to always jump to maximum.

### Code Before Fix

```cpp
case Routing::Target::WavefolderFold:
    setWavefolderFold(floatValue, true);  // ❌ floatValue is 0-100, setter expects 0.0-1.0
    break;
case Routing::Target::WavefolderGain:
    setWavefolderGain(floatValue, true);  // ❌ floatValue is 0-200, setter expects 0.0-2.0
    break;
case Routing::Target::DjFilter:
    setDjFilter(floatValue, true);  // ❌ floatValue is -100 to 100, setter expects -1.0 to 1.0
    break;
case Routing::Target::XFade:
    setXFade(floatValue, true);  // ❌ floatValue is 0-100, setter expects 0.0-1.0
    break;
```

## The Fix

**File:** `src/apps/sequencer/model/CurveSequence.cpp:158-169`

Added division by 100.0 to convert routing percentage values to float range:

```cpp
case Routing::Target::WavefolderFold:
    setWavefolderFold(floatValue / 100.0f, true);  // ✓ Convert 0-100 to 0.0-1.0
    break;
case Routing::Target::WavefolderGain:
    setWavefolderGain(floatValue / 100.0f, true);  // ✓ Convert 0-200 to 0.0-2.0
    break;
case Routing::Target::DjFilter:
    setDjFilter(floatValue / 100.0f, true);  // ✓ Convert -100 to 100 to -1.0 to 1.0
    break;
case Routing::Target::XFade:
    setXFade(floatValue / 100.0f, true);  // ✓ Convert 0-100 to 0.0-1.0
    break;
```

**Result:** Routing values are now correctly converted to the float range expected by the setters.

## Why This Pattern?

The routing system uses integer/percentage ranges for several reasons:

1. **UI Consistency**: All routing targets display as percentages or integers (Tempo: 120.0, SlideTime: 50%, etc.)
2. **Precision**: Integer math for min/max calculations is more precise
3. **Serialization**: Integer ranges are easier to store and validate

Other parameters (like **SlideTime**, **FillAmount**) already used integer percentages (0-100) and had corresponding integer setters. The Wavefolder/Chaos parameters were unique in using **float setters** while having **integer routing ranges**.

## Verification

Created `TestCurveWavefolderRouting.cpp` with comprehensive test cases:

1. **WavefolderFold**: Verifies values convert correctly (0-100 → 0.0-1.0)
2. **WavefolderGain**: Verifies values convert correctly (0-200 → 0.0-2.0)
3. **DjFilter**: Verifies bipolar values convert correctly (-100 to 100 → -1.0 to 1.0)
4. **XFade**: Verifies values convert correctly (0-100 → 0.0-1.0)
5. **Clamping**: Verifies setters still properly clamp out-of-range values

All tests pass.

## Manual Testing

To verify the fix works in practice:

### Test 1: WavefolderFold Routing
1. Create Curve track on Track 1
2. Configure route: **CV In 1** → **Curve Track 1 WavefolderFold** (Min: 0, Max: 100)
3. Connect CV cable to CV In 1
4. Vary voltage from 0V to 5V
5. **Expected:** WavefolderFold changes smoothly from 0.00 to 1.00
6. **Before fix:** WavefolderFold stuck at 1.00 (maximum)
7. **After fix:** WavefolderFold follows CV input correctly ✅

### Test 2: WavefolderGain Routing
1. Configure route: **CV In 1** → **Curve Track 1 WavefolderGain** (Min: 0, Max: 200)
2. Vary CV voltage
3. **Expected:** WavefolderGain changes from 0.00 to 2.00
4. **Before fix:** WavefolderGain stuck at 2.00
5. **After fix:** WavefolderGain follows CV input correctly ✅

### Test 3: DjFilter Routing (Bipolar)
1. Configure route: **CV In 1** → **Curve Track 1 DjFilter** (Min: -100, Max: 100)
2. Vary CV voltage
3. **Expected:** DjFilter changes from -1.00 to +1.00
4. **Before fix:** DjFilter at extremes
5. **After fix:** DjFilter follows CV input correctly ✅

### Test 4: XFade Routing
1. Configure route: **CV In 1** → **Curve Track 1 XFade** (Min: 0, Max: 100)
2. Vary CV voltage
3. **Expected:** XFade changes from 0.00 to 1.00
4. **Before fix:** XFade stuck at 1.00
5. **After fix:** XFade follows CV input correctly ✅

## Affected Parameters

**Fixed (all Curve track sequence-level parameters):**
- ✅ WavefolderFold (0.0-1.0 range)
- ✅ WavefolderGain (0.0-2.0 range)
- ✅ DjFilter (-1.0 to 1.0 range)
- ✅ XFade (0.0-1.0 range)

**Already Working (integer parameters):**
- ✅ ChaosAmount (0-100 integer)
- ✅ ChaosRate (0-127 integer)
- ✅ ChaosParam1 (0-100 integer)
- ✅ ChaosParam2 (0-100 integer)

## Related Fixes

This fix completes the Curve track routing implementation that was partially enabled by the earlier fix at `Routing.cpp:190` (adding `isChaosTarget()` and `isWavefolderTarget()` to the outer dispatcher).

**Related Documentation:**
- `TUESDAY_ROUTING_COMPLETE_FIX.md` - Similar routing dispatcher fixes for Tuesday tracks
- `TUESDAY_ROUTING_FIX.md` - Initial (incomplete) Tuesday routing fix

## Files Modified

**Implementation:**
- `src/apps/sequencer/model/CurveSequence.cpp` (lines 158-169) - Added /100.0 conversion

**Tests:**
- `src/tests/unit/sequencer/TestCurveWavefolderRouting.cpp` (new file)
- `src/tests/unit/sequencer/CMakeLists.txt` (line 16) - Registered new test

**No changes to:**
- `src/apps/sequencer/model/Routing.cpp` - Already fixed in previous commit
- `src/apps/sequencer/model/CurveSequence.h` - Setter signatures unchanged

## Side Effects

**None expected.** This fix only enables functionality that was always supposed to work but didn't.

**Potential issues to watch for:**
- If users created workarounds (e.g., manually setting parameters because routing didn't work), those workarounds might now conflict
- Saved projects with "broken" route configurations will now suddenly work as intended

## Conclusion

**Four-line fix (division by 100.0) enables correct routing behavior for all Curve track Wavefolder and Chaos parameters.**

The routing infrastructure was always correct - the bug was a simple unit conversion mismatch between the routing system's integer percentage values and the parameter setters' float expectations.

**Status:** ✅ COMPLETELY FIXED

All Curve track wavefolder and chaos parameters now respond correctly to routing sources (CV inputs, LFOs, MIDI, internal routing, etc.).

# Issue: Trill Not Working in Finite Loops After Project Load

## 1. Description

After loading a project with a TuesdayTrack in finite (fixed) loop mode, trills do not play. The user must first switch to infinite loop mode, then back to finite loop mode for trills to work.

## 2. Root Cause

The trill functionality is implemented in **two separate code paths** that are not synchronized:

1. **Buffer Generation (Finite Loops)** - Lines ~1062-2116
   - Pre-generates 128 steps into `_buffer[]` when finite loop is initialized
   - Sets `isTrill` flag in `BufferedStep` structure
   - Buffer is read during playback

2. **Infinite Loop Code** - Lines ~2356-3400
   - Generates notes live during playback
   - Sets `_retriggerArmed` flag directly to trigger trill behavior

**The critical issue:** Trill code was added to the infinite loop paths for 7 algorithms, but the corresponding buffer generation code was not updated to set `isTrill = true`.

## 3. Affected Algorithms

| Algorithm | Buffer Gen Line | Infinite Loop Line | Buffer Has Trill? |
|-----------|----------------|-------------------|-------------------|
| STOMPER (2) | ~1159 | 2511, 2545 | NO |
| CHIPARP (4) | ~1323 | 2722 | NO |
| WOBBLE (7) | ~1478 | 2891 | NO |
| TECHNO (8) | ~1524 | 2946 | NO |
| FUNK (9) | ~1579 | 3013 | NO |
| RAGA (12) | ~1703 | 3155 | NO |
| ACID (14) | ~1912 | 3370 | NO |
| DRILL (15) | ~1798 | 3236 | YES (reference) |

## 4. Why Switching Modes "Fixes" It

**Timeline:**

1. **Load project with finite loop**:
   - `generateBuffer()` pre-generates 128 steps
   - `isTrill` flags are NOT set (except DRILL)
   - During playback, `tick()` reads `isTrill = false`
   - No trills happen

2. **Switch to infinite loop**:
   - Live generation code is used
   - Trills work because infinite loop code has trill logic

3. **Switch back to finite loop**:
   - `generateBuffer()` is called again
   - Buffer is regenerated (still without trill flags)
   - But some initialization happens that makes it work

## 5. BufferedStep Structure

```cpp
// TuesdayTrackEngine.h line 49
struct BufferedStep {
    int8_t note;
    int8_t octave;
    uint8_t gatePercent;
    uint8_t slide;
    uint8_t gateOffset;
    bool isTrill = false;  // <-- This field exists but isn't set
};
```

## 6. Reference Implementation: DRILL Buffer Generation

DRILL (case 15, lines 1798-1845) correctly implements trill in buffer generation:

```cpp
case 15: // DRILL buffer generation
    if (hihatHit) {
        // ... generate note, octave, gatePercent ...

        // Check for Trill/Retrigger
        int trillChanceAlgorithmic = 30;
        int userTrillSetting = _tuesdayTrack.trill();
        int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

        if (_uiRng.nextRange(100) < finalTrillChance) {
            isTrill = true;  // <-- Sets the flag!
            uint32_t retriggerLength = (CONFIG_SEQUENCE_PPQN / 4) / 2;
            gatePercent = (retriggerLength * 100) / CONFIG_SEQUENCE_PPQN;
        }
    }
    break;
```

## 7. How Buffer Trill Flag is Used

In `tick()` function around lines 2320-2329:

```cpp
// Check for trill/re-trigger
if (bufferedStep.isTrill) {
    _retriggerArmed = true;
    _retriggerCount = 2;
    _retriggerPeriod = divisor / 3;
    _retriggerLength = _retriggerPeriod / 2;
    _isTrillNote = false;

    float baseVoltage = (note + (octave * 12)) / 12.f;
    _trillCvTarget = baseVoltage + (2.f / 12.f);
}
```

**Note:** This hardcoded trill behavior (divisor/3, +2 semitones) is applied to ALL algorithms when reading from buffer. Algorithm-specific parameters from infinite loop code are NOT used.

## 8. Fix Required

For each algorithm, add trill probability check to buffer generation section:

```cpp
// Check for Trill
int trillChanceAlgorithmic = XX; // algorithm-specific base chance
int userTrillSetting = _tuesdayTrack.trill();
int finalTrillChance = (trillChanceAlgorithmic * userTrillSetting) / 100;

if (_uiRng.nextRange(100) < finalTrillChance) {
    isTrill = true;
}
```

### Algorithm-Specific Parameters

| Algorithm | Base Prob | Trigger Condition |
|-----------|-----------|-------------------|
| STOMPER | 18% | On SLIDEDOWN2/SLIDEUP2 modes |
| CHIPARP | 20% | On first chord position (chordpos == 0) |
| WOBBLE | 12% | On phase transitions |
| TECHNO | 25% | On hi-hat hits only |
| FUNK | 15% | On ghost notes |
| RAGA | 25% | On ascending movements |
| ACID | 20% | On accented notes |

## 9. Additional Consideration

The current `tick()` code applies uniform trill parameters (divisor/3, +2 semitones) to all algorithms. For full algorithm-specific behavior in finite loops, additional fields would need to be added to `BufferedStep`:

```cpp
struct BufferedStep {
    // ... existing fields ...
    bool isTrill = false;
    uint8_t trillCount = 2;      // retrigger count
    uint8_t trillDivisor = 3;    // period = divisor / trillDivisor
    int8_t trillPitchOffset = 2; // semitones
};
```

However, for initial fix, just setting `isTrill = true` with the default parameters is sufficient.

## 10. Current Status

**FIXED** - Buffer generation code has been updated for all 7 algorithms.

### Fix Applied

Added trill probability checks to buffer generation for:
- STOMPER (case 2) - lines 1196-1203, 1219-1226
- CHIPARP (case 4) - lines 1386-1393
- WOBBLE (case 7) - lines 1545-1552
- TECHNO (case 8) - lines 1595-1600
- FUNK (case 9) - lines 1671-1676
- RAGA (case 12) - lines 1809-1816
- ACID (case 14) - lines 2014-2019

Each algorithm now sets `isTrill = true` in the buffer based on its specific trigger conditions and probability.

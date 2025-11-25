# Curve Track Folder & Feedback Feature Development Status

This document summarizes the development and current status of the advanced CV shaping features for the `CurveTrack`, including wavefolding, filtering, and two independent feedback loops.

## 1. Overview

The goal of this session was to implement advanced "West Coast" style CV shaping for the `CurveTrack` in a modular fashion. This involved adding:
*   A wavefolder for complex shape generation.
*   A DJ-style LPF/HPF for sculpting the CV output.
*   Two independent feedback loops to create dynamic and potentially chaotic interactions.

## 2. Wavefolder Implementation

The wavefolder serves as the primary shape distortion unit.

*   **Parameters:**
    *   `Fold`: Controls the number of folds/complexity of the waveform.
    *   `Gain`: Controls the input drive into the folding stage.
    *   `Symmetry`: Adjusts the bias of the folding, affecting the harmonic character.
*   **Model (`CurveTrack`):** `_wavefolderFold`, `_wavefolderGain`, `_wavefolderSymmetry` (all `float`) were added and integrated into the `CurveTrack` model, including serialization (ProjectVersion 43).
*   **Engine (`CurveTrackEngine`):** An `applyWavefolder` helper function was implemented, operating on the normalized `0.0-1.0` signal.
*   **Fold Control Response:** The `Fold` parameter's linear value from the UI is squared (`fold * fold`) before being used by the wavefolder algorithm. This creates an exponential response, providing more nuanced control at lower fold settings.

## 3. DJ Filter Implementation

A versatile DJ-style filter was integrated to provide final sculpting of the CV signal.

*   **Parameters:**
    *   `djFilter`: A bipolar `float` parameter (`-1.0` to `+1.0`).
        *   `0.0`: Filter bypassed.
        *   Positive (`+0.02` to `+1.0`): Engages a High-Pass Filter (HPF).
        *   Negative (`-0.02` to `-1.0`): Engages a Low-Pass Filter (LPF).
    *   **Dead Zone:** A small dead zone (`+/- 0.02`) around the center of the `djFilter` control ensures an explicit bypass state.
*   **Filter Slope:** Implemented as a **6 dB/octave** (first-order IIR) filter.
*   **Model (`CurveTrack`):** `_djFilter` was added to `CurveTrack` and integrated into serialization (ProjectVersion 44).
*   **Engine (`CurveTrackEngine`):**
    *   A `_lpfState` float member was added to `CurveTrackEngine` to maintain the filter's internal state.
    *   An `applyDjFilter` helper function was implemented.
*   **Control Response:** The mapping of the `djFilter` control to the filter's `alpha` coefficient was carefully adjusted to provide an intuitive "stronger effect with more knob turn" behavior for both LPF and HPF modes, addressing initial observations about counter-intuitive amplitude drops.

## 4. Feedback Loops Implementation ("West Coast" Style)

Two independent feedback loops were added to create a rich, chaotic, and highly interactive CV shaping instrument.

*   **Parameters:**
    *   `Fold-F` (`_foldF`): Controls feedback from the filter output back to the wavefolder input (`0.0` to `1.0`).
    *   `Filter-F` (`_filterF`): Controls feedback around the filter itself (resonance/Q), from `0.0` to `1.0`.
*   **Model (`CurveTrack`):** `_foldF` and `_filterF` were added to `CurveTrack` and integrated into serialization (ProjectVersion 45).
*   **Engine (`CurveTrackEngine`):**
    *   A `_feedbackState` float member was added to `CurveTrackEngine` to store the last tick's filtered output for `Fold-F` feedback.
    *   The `applyDjFilter` function was modified to accept `_filterF` as a resonance parameter, implementing the filter-to-filter feedback.
    *   The `updateOutput` function was refactored to implement both feedback paths: `_feedbackState` is added to the wavefolder input for `Fold-F`, and the `applyDjFilter` now handles `Filter-F`.
*   **UI (`CurveSequenceEditPage`):**
    *   The `EditMode` state machine was expanded to four states: `Step`, `GlobalPhase`, `Wavefolder1` (for Fold, Gain, Symmetry, Filter), and `Wavefolder2` (for Fold-F, Filter-F).
    *   A second UI page (`Wavefolder2`) was created within `CurveSequenceEditPage` to display and allow editing of the `Fold-F` and `Filter-F` parameters, mimicking the `TuesdayEditPage` style with value bars and F-key selection.

## 5. Compilation Issues & Resolutions

Throughout the development process, several compilation errors and warnings were encountered and resolved, including:
*   A typo in an include directive (`LedPainter.hh` instead of `LedPainter.h`).
*   Syntax errors and scope issues in `CurveSequenceEditPage.cpp` due to refactoring the `draw()` and `encoder()` functions.
*   Mismatched enum usage (`EditMode::Wavefolder` after it was renamed to `EditMode::Wavefolder1`/`Wavefolder2`).
*   Warnings regarding the use of bitwise OR (`|`) with boolean operands, which were corrected to logical OR (`||`).

## 6. Issue to Investigate: FILTER-F Path

The implementation of `Filter-F` (Filter-to-Filter feedback / Resonance) within the `applyDjFilter` function utilizes a simplified resonance model for a 1-pole filter (`float feedback_input = input - lpfState * feedback;`). While this method introduces a resonant characteristic, it is inherently limited compared to what a multi-pole (e.g., 2-pole State Variable Filter) resonant filter can achieve.

*   **Behavior to Monitor:** The user should observe whether the `Filter-F` parameter provides a sufficiently pronounced and musically useful resonance.
*   **Potential Improvement:** If the resonance effect is deemed too subtle or lacks the desired musical quality, a future investigation into implementing a more advanced filter topology (e.g., a 2-pole resonant low-pass/high-pass filter) within `applyDjFilter` may be warranted. This would increase computational cost and complexity but could yield a more classic resonant filter sound.

# Feature: Selective Output Rotation (CV & Gate)

## Concept
Allow dynamic rotation of track-to-output assignments within a **user-defined subset** of tracks. This enables complex signal routing (e.g., rotating a chord voicing across 3 oscillators) while leaving other tracks (e.g., drums on track 4-8) static.

## Architecture
The "Active Rotation Pool" is defined dynamically by the **Routing Mask**.
*   If a Track is targeted by the Rotation Route, it joins the pool.
*   If a Track is NOT targeted (or has 0 rotation), it remains static on its assigned output.

## Algorithm: "The Modulated Pool"

1.  **Setup (Engine Update):**
    *   Scan all Physical Outputs.
    *   Identify the **Source Track** for each output.
    *   Check if that Source Track has a non-zero **Rotation Value** (modulated via Routing).
    *   Build a list of **Pool Indices** (Outputs that are "in the game").
    
2.  **Execution:**
    *   For each Output in the Pool:
        *   Calculate its position in the pool list.
        *   Apply the rotation offset (derived from the track's rotation parameter).
        *   Find the **New Source Track** from the rotated position in the pool.
    *   For Outputs NOT in the Pool:
        *   Use the static assignment from Layout.

## Implementation Plan

### 1. Model (`Track.h`, `Routing.h`)
*   **Add Parameter:** `Routable<int8_t> _outputRotate` to **`Track`** class.
    *   *Note:* We use a single parameter to control both CV and Gate rotation for that track, or split them if needed. Let's start with a unified `Target::OutputRotate` or separate `CvRotate`/`GateRotate`.
    *   Let's use distinct targets: `Routing::Target::CvOutputRotate` and `Routing::Target::GateOutputRotate` (Per Track).
*   **Routing:** Ensure these are flagged as `isTrackTarget`.

### 2. Engine (`Engine.cpp`)
*   **Logic:**
    ```cpp
    // Pseudo-code for CV Rotation
    std::vector<int> poolOutputs;
    std::vector<int> poolTracks;
    
    // 1. Identify Pool
    for (int i = 0; i < 8; ++i) {
        int trackIdx = _project.cvOutputTrack(i);
        if (trackIdx != -1 && _project.track(trackIdx).cvOutputRotate() != 0) {
            poolOutputs.push_back(i);
            poolTracks.push_back(trackIdx);
        }
    }
    
    // 2. Dispatch
    for (int i = 0; i < 8; ++i) {
        int originalTrack = _project.cvOutputTrack(i);
        if (originalTrack == -1) continue;
        
        // Check if this output is in the pool
        auto it = std::find(poolOutputs.begin(), poolOutputs.end(), i);
        if (it != poolOutputs.end()) {
            // It's rotating!
            int poolIndex = std::distance(poolOutputs.begin(), it);
            int rotation = _project.track(originalTrack).cvOutputRotate();
            
            // Wrap rotation within pool size
            int poolSize = poolOutputs.size();
            int rotatedIndex = (poolIndex - rotation) % poolSize;
            if (rotatedIndex < 0) rotatedIndex += poolSize;
            
            // Assign the rotated track
            driver.setCV(i, trackEngines[poolTracks[rotatedIndex]].cv());
        } else {
            // Static
            driver.setCV(i, trackEngines[originalTrack].cv());
        }
    }
    ```

## Resource Cost
*   **CPU:** Light. Two loops over 8 items. Vector/Find operations are tiny on size 8.
*   **RAM:** Per-track variable (8 bytes).

## Usage Example
1.  **Layout:** Track 1->Out 1, Track 2->Out 2, Track 3->Out 3, Track 4->Out 4.
2.  **Routing:** LFO1 -> `CvOutputRotate` -> Tracks **1, 2, 3** (Amount 100%).
3.  **Result:**
    *   Outputs 1, 2, 3 rotate sources (1->2->3->1...).
    *   Output 4 stays firmly connected to Track 4.

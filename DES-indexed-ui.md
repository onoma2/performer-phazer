# Indexed Track OLED UI Design Proposals

This document outlines three different UI design approaches for the indexed track implementation on the performer firmware, utilizing the standard OLED header and footer.

## Available Controls
- 16 step buttons (that may or may not be used to select steps)
- Shift + 16 steps
- 5 function buttons
- Shift + 5 function buttons
- Encoder press and rotation
- Shift encoder press and rotation
- All controls support press, hold, and toggle functionality
- Usability is the primary design principle

## UI Design Version 1: Step-Focused Layout

This design prioritizes visual representation of steps and makes it easy to see the current playback position and step states.

### Display Layout
```
HEADER: [Tempo] [Track] [Pattern] [Mode: INDEXED]
--------------------------------------------------
[Step 1] [Step 2] [Step 3] [Step 4] [Step 5] [Step 6] [Step 7] [Step 8]
[Step 9] [Step 10][Step 11][Step 12][Step 13][Step 14][Step 15][Step 16]
--------------------------------------------------
[SEL]   [DUR]   [GATE]  [NOTE]  [FUNC]  (Footer with labels)
```

### Control Mapping
- **16 Step Buttons**: Select steps 1-16 (visual feedback for selected/playing)
- **Shift + 16 Steps**: Toggle step groups A-D (each step button toggles multiple groups)
- **Function Buttons**:
  - F1 (SEL): Select step mode (default) - press to confirm selection
  - F2 (DUR): Duration edit mode - use encoder to adjust selected step duration
  - F3 (GATE): Gate length edit mode - use encoder to adjust gate %
  - F4 (NOTE): Note index edit mode - use encoder to adjust note index
  - F5 (FUNC): Function menu - access additional options (init, route, loop, etc.)

### Interaction Model
- **Normal Mode**: Step buttons select steps, encoder adjusts currently selected parameter
- **Shift Mode**: Hold shift while pressing step buttons to assign/remove groups
- **Encoder Press**: Toggle between edit/browse modes
- **Shift + Encoder Press**: Toggle follow mode
- **Encoder Rotation**: Adjust currently active parameter
- **Hold Function**: Access submenu for that function

### Visual Feedback
- Bright: Currently playing step
- Medium: Selected step
- Dim: Other steps
- Inverted: Steps in active groups

## UI Design Version 2: Parameter-Focused Layout

This design prioritizes parameter editing and provides detailed information about the selected step.

### Display Layout
```
HEADER: [Tempo] [Track] [Pattern] [Mode: INDEXED]
--------------------------------------------------
[St: 05] [Dur: 192] [Gate: 50%] [Note: C3]
[Grp: A ] [Scl: Major] [Rt: C] [Lp: ON]
--------------------------------------------------
[SEL]   [DUR]   [GATE]  [NOTE]  [FUNC]  (Footer with labels)
```

### Control Mapping
- **16 Step Buttons**: Select steps 1-16 (each button represents a step)
- **Shift + 16 Steps**: Direct parameter adjustment for that step number (e.g., Shift+1 adjusts duration of step 1)
- **Function Buttons**:
  - F1 (SEL): Select step mode - step buttons select steps
  - F2 (DUR): Duration mode - step buttons adjust duration of corresponding steps
  - F3 (GATE): Gate mode - step buttons adjust gate % of corresponding steps
  - F4 (NOTE): Note mode - step buttons adjust note index of corresponding steps
  - F5 (FUNC): Function mode - step buttons access different functions

### Interaction Model
- **Normal Mode**: Step buttons select steps, encoder adjusts active step's parameter
- **Shift Mode**: Direct parameter adjustment for each step
- **Function Button Press**: Switch to that parameter editing mode
- **Encoder Press**: Toggle parameter lock (prevents accidental changes)
- **Shift + Encoder Press**: Toggle between fine/coarse adjustment
- **Encoder Rotation**: Adjust currently active parameter

### Visual Feedback
- Highlighted parameter shows which one is currently being edited
- Step buttons light up when pressed
- Parameter values update in real-time

## UI Design Version 3: Hybrid Layout

This design combines the best of both previous approaches, providing both visual step representation and parameter editing capabilities.

### Display Layout
```
HEADER: [Tempo] [Track] [Pattern] [Mode: INDEXED]
--------------------------------------------------
[1][2][3][4][5][6][7][8] [DUR: 192]
[9][10][11][12][13][14][15][16] [GATE: 50%]
--------------------------------------------------
[SEL]   [DUR]   [GATE]  [NOTE]  [FUNC]  (Footer with labels)
```

### Control Mapping
- **16 Step Buttons**: 
  - Primary: Select steps 1-16 (visual feedback)
  - Hold: Mute/unmute individual steps
- **Shift + 16 Steps**: 
  - Toggle groups A-D for corresponding steps
  - Hold: Copy step to clipboard
- **Function Buttons**:
  - F1 (SEL): Selection mode - step buttons select steps
  - F2 (DUR): Duration mode - step buttons adjust duration, encoder fine-tunes
  - F3 (GATE): Gate mode - step buttons adjust gate, encoder fine-tunes
  - F4 (NOTE): Note mode - step buttons adjust note, encoder fine-tunes
  - F5 (FUNC): Function mode - access menus, clipboard operations

### Interaction Model
- **Normal Mode**: Step buttons select steps, encoder adjusts current step parameter
- **Shift Mode**: Group assignment or copy operations
- **Hold Step Button**: Mute/unmute that step
- **Hold Function Button**: Access advanced options for that function
- **Encoder Press**: Toggle edit mode
- **Shift + Encoder Press**: Toggle between step view and parameter view
- **Encoder Rotation**: Adjust parameter or scroll through steps

### Visual Feedback
- Bright: Currently playing step
- Medium: Selected step
- Inverted: Steps in active groups
- Dim: Other steps
- Bottom row shows parameters for currently selected step

## Detailed Interaction Models

### Version 1: Step-Focused Layout - Interaction Model

**Startup State:**
- Default to step selection mode
- Current step is highlighted
- Encoder adjusts the parameter currently shown in the footer

**Step Selection:**
- Press any step button (1-16) to select that step
- Visual feedback: Selected step becomes medium brightness
- The parameter display in footer updates to show selected step's values

**Parameter Editing:**
- Press encoder to toggle between browse and edit mode
- In edit mode, encoder adjusts the currently active parameter
- Press function buttons to switch active parameter (DUR/GATE/NOTE)
- Shift + encoder press toggles fine/coarse adjustment

**Group Assignment:**
- Hold shift while pressing a step button to toggle group assignment
- Use the same button multiple times to cycle through groups A, B, C, D
- Visual feedback: Inverted display for steps in active groups

**Function Menu:**
- Press FUNC button to access contextual menu
- Options: INIT sequence, ROUTE modulation, LOOP toggle, etc.
- Use encoder to navigate menu, encoder press to select

### Version 2: Parameter-Focused Layout - Interaction Model

**Startup State:**
- Default to showing parameters for step 1
- All parameters visible at once (Duration, Gate, Note, etc.)

**Step Selection:**
- Press any step button (1-16) to view/edit that step's parameters
- Display updates immediately to show selected step's parameters

**Parameter Editing:**
- Press function button to focus on that parameter
- With parameter focused, encoder adjusts that specific parameter
- Shift + encoder press enables fine adjustment mode
- Encoder press toggles parameter lock (prevents accidental changes)

**Direct Parameter Access:**
- Shift + step button directly adjusts that step's parameter based on active mode
- For example: If DUR mode is active, Shift+step adjusts duration of that step

**Advanced Functions:**
- Hold function button for extended options (e.g., copy/paste parameter to all steps)
- FUNC button accesses global sequence functions

### Version 3: Hybrid Layout - Interaction Model

**Startup State:**
- Visual grid shows all 16 steps
- Bottom row shows parameters for currently selected step
- Clear visual indication of playback position

**Step Interaction:**
- Press step button to select that step
- Hold step button to mute/unmute that step
- Visual feedback shows current selection and playback position

**Parameter Editing:**
- Press function button to enter parameter edit mode
- In parameter mode, step buttons affect the corresponding parameter
- Encoder fine-tunes the active parameter
- Encoder press toggles between step and parameter view

**Group Operations:**
- Shift + step button toggles groups for that step
- Hold shift while pressing multiple step buttons to assign same groups to multiple steps

**Advanced Operations:**
- Hold function button for clipboard operations (copy/paste/swap steps)
- FUNC button accesses sequence-wide operations (init, route, etc.)

**View Toggle:**
- Shift + encoder press toggles between step view (grid) and parameter view (detailed)

## Summary

Each design prioritizes different aspects of usability:

- **Version 1** emphasizes the visual representation of steps and is intuitive for users who think in terms of step positions
- **Version 2** emphasizes parameter editing and is ideal for users who need to make detailed adjustments
- **Version 3** provides a balance between visual feedback and parameter access

All designs follow the standard header/footer pattern and make full use of the available controls while maintaining usability as the primary concern.
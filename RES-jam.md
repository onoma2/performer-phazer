Yes, feasible.

  Proposed 16‑button mapping (top row = “up/positive”, bottom row = “down/negative”):

  Top row (Steps 1–8)

  1. Octave +
  2. Transpose +
  3. Root Note +
  4. Divisor ↑ (straight/even only)
  5. Divisor ↑ (triplet only)
  6. Divisor ÷2 (faster)
  7. Mask Parameter +
  8. Loop Length +

  Bottom row (Steps 9–16)
  9. Octave −
  10. Transpose −
  11. Root Note −
  12. Divisor ↓ (straight/even only)
  13. Divisor ↓ (triplet only)
  14. Divisor ×2 (slower)
  15. Mask Parameter −
  16. Loop Length −

  Shift modifiers

  - Shift+Step 8: RUN momentary OFF (press = stop)
  - Shift+Step 16: Mute momentary (press = mute)

  Notes on behavior:

  - Top row moves "up" (faster/greater), bottom row moves "down" (slower/less); for divisors, smaller = faster.
  - “Divisor ↑/↓ (straight)” can walk only known divisors with type '\0' in KnownDivisor (1/64 … 4).
  - “Divisor ↑/↓ (triplet)” can walk only known divisors with type 'T'.
  - ×2/÷2 just multiplies/divides the raw divisor and clamps 1..768.
  - Mask up/down maps to editMaskParameter(+/-1).

  For the momentary actions:

  - RUN: on press set Track::setRunGate(false), on release restore previous run state.
  - Mute: on press PlayState::muteTrack(track, Immediate), on release restore previous mute state.

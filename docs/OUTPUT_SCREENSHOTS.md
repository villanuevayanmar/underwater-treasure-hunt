# Output Screenshots

These are genuine screenshots captured by the GitHub Actions Chromium test from the compiled WebAssembly game on November 27, 2026. They show actual program output, not concept art.

| Screen | Screenshot |
|---|---|
| Main menu | [main-menu.png](../screenshots/main-menu.png) |
| Underwater gameplay | [gameplay.png](../screenshots/gameplay.png) |
| Submarine at the cave approach | [treasure-cave.png](../screenshots/treasure-cave.png) |
| Mission complete | [mission-complete.png](../screenshots/mission-complete.png) |
| Touch-enabled landscape layout | [mobile-landscape.png](../screenshots/mobile-landscape.png) |

## What the browser test checked

The test verified that:
- The desktop canvas opens at 960 × 540.
- The title screen starts the game.
- Pressing SPACE while far from the treasure does not collect it.
- Keyboard input moves the submarine to the cave.
- The submarine stops when it reaches a rock.
- The nearby treasure can be collected and opens the completion screen.
- The result text can be read back from the WebAssembly virtual file system.
- Play Again resets the mission.
- ESC and the Main Menu button return to the title screen.
- An 844 × 475 landscape touch-emulated viewport fits the game.
- Touch input starts the game and moves the submarine with the directional pad.

The mobile screenshot is a Chromium touch-emulation capture. It is **not** a screenshot from a physical Android phone. A real-device check is still required before claiming physical Android compatibility.

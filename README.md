# Underwater Treasure Hunt: The Lost Treasure

An interactive 2D underwater exploration game written in **C17** for a Programming 1 final animation project.

Pilot a submarine through the ocean, avoid rocks, find the hidden cave, and collect the lost treasure.

## Current status

**Playable browser game — main gameplay loop implemented and browser-tested.**

Implemented:
- Title screen with instructions and Start button
- Underwater scenery drawn with raylib shapes
- Moving fish and rising bubbles
- Submarine movement with WASD and arrow keys
- On-screen directional controls for touch screens
- Responsive 16:9 browser canvas and a portrait rotate-to-landscape notice
- Screen boundaries and rock collision detection
- Hidden cave and treasure chest
- SPACE / COLLECT interaction with a proximity check
- Mission-complete screen with exploration time
- Play Again, Main Menu, and ESC navigation
- Mission result saved using standard C file handling

## Technology

- **C17** — game logic
- **raylib 6.2** — drawing, animation, input, and collision checks
- **CMake** — build configuration
- **Emscripten / WebAssembly** — browser build
- **GitHub Actions** — cloud compilation and Chromium tests

The repository is public. The workflow uses a standard GitHub-hosted runner; it does not require a Codespaces session or a paid runner. Do not enable paid usage.

## Browser build

The latest successful build and its downloadable artifact are available from the [successful GitHub Actions run](https://github.com/villanuevayanmar/underwater-treasure-hunt/actions/runs/46000246011).

The browser build contains three files that must stay together:
- `underwater_treasure_hunt.html`
- `underwater_treasure_hunt.js`
- `underwater_treasure_hunt.wasm`

The HTML must be served over HTTP/HTTPS so the browser can load the JavaScript and WebAssembly files. Opening the HTML directly from a phone's file manager may not work. The artifact is temporary and expires after 7 days.

A permanent public playable URL has not been deployed yet.

## Controls

| Input | Action |
|---|---|
| W / Up Arrow | Move up |
| S / Down Arrow | Move down |
| A / Left Arrow | Move left |
| D / Right Arrow | Move right |
| SPACE | Collect the treasure when close |
| On-screen directional pad | Move on a touch screen |
| COLLECT button | Try to collect the treasure |
| ESC | Return to the title screen during the mission |

## Game objective

1. Start the mission from the title screen.
2. Explore the ocean and avoid the rocks.
3. Find the dark cave on the right side.
4. Move close to the treasure chest.
5. Press SPACE or tap COLLECT.
6. View the mission result and choose Play Again or Main Menu.

## File handling note

When the treasure is collected, the program uses `fopen`, `fprintf`, and `fclose` to write `mission_result.txt`. The browser test reads the file back from Emscripten's virtual file system and checks its contents.

In the WebAssembly build, this file is stored in the virtual file system for the current session. This version does **not** download the file to the phone or guarantee that it persists after the browser page closes or refreshes.

## C concepts demonstrated

| Concept | Use in the game |
|---|---|
| Variables | Position, speed, timer, game state |
| Conditions | Movement, collisions, treasure interaction, screen changes |
| Loops | Main game loop and object updates |
| Structures | Submarine, fish, bubbles, obstacles, treasure |
| Functions | Input, movement, collision, drawing, file saving |
| File handling | Saving the mission result with `FILE` |

## Documentation

- [Storyline](docs/STORYLINE.md)
- [Flowchart](docs/FLOWCHART.md)
- [Algorithm](docs/ALGORITHM.md)
- [Beginner code explanation](docs/CODE_EXPLANATION.md)
- [Output screenshots](docs/OUTPUT_SCREENSHOTS.md)
- [Group contributions template](docs/GROUP_CONTRIBUTIONS.md)

## Test results

The latest GitHub Actions run completed successfully. It:
- Configured and compiled the C17 / raylib / Emscripten project
- Verified the generated HTML, JavaScript, and WebAssembly files
- Loaded the game in headless Chromium without browser page errors
- Started the game and moved the submarine with keyboard input
- Verified that the submarine stops against a rock
- Confirmed SPACE cannot collect the treasure while the submarine is far away
- Reached the cave and completed the treasure interaction
- Read back `mission_result.txt` and verified the saved mission text
- Verified Play Again resets the mission
- Verified ESC and the Main Menu button return to the title screen
- Tested the landscape mobile viewport, touch Start, and held directional touch input

Genuine Chromium screenshots are in [`screenshots/`](screenshots/) and linked in the [output screenshot guide](docs/OUTPUT_SCREENSHOTS.md).

**Not yet tested:** a physical Android phone. The mobile checks use Chromium's touch emulation, not the actual phone. The screenshots are also headless desktop-browser captures, not camera photos of the phone.

## Team

Group members and their actual contributions are recorded in [GROUP_CONTRIBUTIONS.md](docs/GROUP_CONTRIBUTIONS.md). Fill in names and completed work before submission.

## Instructor approval

raylib and the WebAssembly/browser target are still proposed technologies. Confirm with the instructor that an external graphics library and this target are acceptable for the assignment. Do not represent them as approved until confirmed.

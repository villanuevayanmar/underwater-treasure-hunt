# Underwater Treasure Hunt: The Lost Treasure

An interactive 2D underwater exploration game written in **C17** for a Programming 1 final animation project.

Pilot a submarine through the ocean, avoid rocks, find the hidden cave, and collect the lost treasure.

## Current status

**Playable browser version — first complete gameplay loop.**

Implemented:
- Title screen with start control and instructions
- Underwater scenery made with raylib shapes
- Moving fish and rising bubbles
- Submarine movement using WASD or arrow keys
- On-screen directional controls for touch screens
- Screen-boundary checks and rock collision detection
- Cave and treasure chest
- SPACE / COLLECT interaction with a proximity check
- Mission-complete screen with elapsed time
- Restart and main-menu actions
- Mission result written with standard C file handling

## Technology

- **C17** — game logic
- **raylib 6.2** — drawing, animation, input, and collision checks
- **CMake** — build configuration
- **Emscripten / WebAssembly** — browser build
- **GitHub Actions** — cloud build and Chromium smoke test

The repository is public. The workflow uses GitHub-hosted runners for a public repository and does not require Codespaces or a paid runner. Do not enable paid usage.

## Play and inspect

The latest successful build and its downloadable artifact are available from [GitHub Actions](https://github.com/villanuevayanmar/underwater-treasure-hunt/actions/workflows/build-web.yml).

The browser build contains three files that must stay together:
- `underwater_treasure_hunt.html`
- `underwater_treasure_hunt.js`
- `underwater_treasure_hunt.wasm`

The generated HTML should be opened through a web server or static host that serves the WebAssembly file with the correct MIME type. Opening the HTML directly from a phone's file manager may not work.

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

When the treasure is collected, the program uses `fopen`, `fprintf`, and `fclose` to write `mission_result.txt`.

In the WebAssembly build, this file is stored in Emscripten's virtual file system for the current session. This version does not download the file to the phone or guarantee that it persists after the browser page closes or refreshes.

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

## Testing status

The GitHub Actions workflow has successfully:
- Configured the C17 / raylib / Emscripten build
- Compiled the game
- Verified that the HTML, JavaScript, and WebAssembly outputs exist
- Loaded the game in headless Chromium
- Captured the title, gameplay, cave-approach, and mission-complete screens
- Completed the scripted keyboard route without browser JavaScript errors

A physical Android device test has not yet been performed. The touch controls are implemented, but must still be checked on the actual phone before the group claims mobile behavior is fully verified.

## Team

Group members and their actual contributions are recorded in [GROUP_CONTRIBUTIONS.md](docs/GROUP_CONTRIBUTIONS.md). Fill in names and completed work before submission.

## Instructor approval

raylib and the WebAssembly/browser target are still proposed technologies. Confirm with the instructor that an external graphics library and this target are acceptable for the assignment. Do not represent them as approved until confirmed.

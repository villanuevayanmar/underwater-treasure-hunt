# Underwater Treasure Hunt: The Lost Treasure

An interactive 2D underwater exploration game written in **C17** for a Programming 1 animation project.

The player controls a submarine, explores an underwater environment, avoids obstacles, discovers a hidden cave, and collects a treasure.

## Development status

**Milestone 1 — browser build setup and movement prototype.**

The current prototype draws the underwater scene, animates fish and bubbles, and lets the submarine move with a keyboard or on-screen touch controls. Treasure collection, obstacles, collision rules, mission completion, and result saving are not implemented yet.

## Technology

- **C17** — game code
- **raylib 6.2** — 2D shapes, drawing, animation, and input
- **Emscripten / WebAssembly** — browser build
- **CMake** — build configuration
- **GitHub Actions** — cloud compilation

The repository is public. The standard GitHub-hosted runner used by the build workflow is free for public repositories. Do not enable paid runners or paid usage.

## Build

The GitHub Actions workflow builds the browser version whenever code is pushed to `main` or a pull request is opened.

To build in a CMake + Emscripten environment, run:

    emcmake cmake -S . -B build -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
    cmake --build build

The output is an HTML file with its JavaScript and WebAssembly files. The workflow stores these as a build artifact.

## Controls

- **W / A / S / D** or **arrow keys** — move the submarine
- **On-screen directional pad** — move on a touch screen

## Planned game features

- Submarine movement with screen-boundary checks
- Animated fish and bubbles
- Underwater scenery made from basic shapes
- Obstacles and collision detection
- A hidden cave and one treasure objective
- A mission-complete screen
- Save the mission result using standard C file handling

## C concepts to demonstrate

| Concept | Intended use |
|---|---|
| Variables | Position, speed, score, and game state |
| Conditions | Movement, boundaries, collisions, and treasure collection |
| Loops | Main game loop and animation |
| Structures | Submarine, fish, obstacles, and treasure |
| Functions | Drawing and game mechanics |
| `FILE` | Saving the mission result |

## Project documentation

The final submission will include:

1. Storyline
2. Flowchart
3. Algorithm
4. Source code
5. Output screenshots
6. Group contribution documentation

The group must be able to explain the code. Keep the implementation suitable for the topics and rules approved by the instructor.

## Team

Group members and their contributions will be recorded here once confirmed.

## Instructor approval

raylib is the proposed graphics library. Confirm with the instructor that an external graphics library and a browser/WebAssembly target are allowed before treating this toolchain as the final class-approved setup.

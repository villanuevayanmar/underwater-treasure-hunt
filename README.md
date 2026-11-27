# Underwater Treasure Hunt: The Lost Treasure

An interactive 2D underwater exploration game written in **C17** for a Programming 1 animation project.

The player controls a submarine, explores an underwater environment, avoids obstacles, discovers a hidden cave, and collects a treasure.

## Current status

**Project setup stage.** The repository is initialized, but gameplay code and a verified browser build have not been added yet.

## Planned technology

- C17 for game logic
- raylib for 2D drawing, animation, and input
- Emscripten / WebAssembly for a browser-playable build
- CMake for the build system
- GitHub Codespaces for cloud development from an Android browser

The game will target both keyboard and touch controls so it can be demonstrated on a computer or Android phone.

## Planned features

- Submarine movement with screen-boundary checks
- Animated fish and bubbles
- Underwater scenery made from basic shapes
- Obstacles and collision detection
- A hidden cave and one treasure objective
- A mission-complete screen
- Save the mission result using standard C file handling

## Planned C concepts

| Concept | Intended use |
|---|---|
| Variables | Position, speed, score, and game state |
| Conditions | Movement, collisions, and treasure collection |
| Loops | Main game loop and animation |
| Structures | Submarine, fish, obstacles, and treasure |
| Functions | Drawing and game mechanics |
| `FILE` | Saving the mission result |

## Development

The intended development environment is GitHub Codespaces. Open the repository, select **Code → Codespaces → Create codespace on main**, then follow the setup instructions that will be added as the cloud toolchain is configured.

A normal desktop raylib executable is not the target because development and testing are being done from an Android phone. The intended deliverable is a browser build using WebAssembly.

## Controls (planned)

- **W / A / S / D** — Move the submarine
- **SPACE** — Collect the treasure when close enough
- **ESC** — Exit
- **Touch controls** — On-screen controls for phones

## Project documentation

The final submission will include:

1. Storyline
2. Flowchart
3. Algorithm
4. Source code
5. Output screenshots
6. Group contribution documentation

The project must be explained by the group, including the code. The final implementation will be kept within the topics and rules approved by the instructor.

## Team

Group members and their contributions will be recorded here once confirmed.

## Important

The use of `raylib` is a proposed modern graphics approach. Confirm with the instructor that an external graphics library and the browser/WebAssembly target are acceptable before treating them as approved project requirements.

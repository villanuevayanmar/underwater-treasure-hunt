# Code Explanation

The game source is in [`src/main.c`](../src/main.c). It uses C17 and raylib 6.2.

## 1. Header files

- `raylib.h` gives the program drawing, keyboard, mouse, touch, and collision functions.
- `math.h` provides mathematical functions if needed by the C program.
- `stdio.h` provides `FILE`, `fopen`, `fprintf`, and `fclose`.
- The Emscripten header is included only for a browser build. It allows the browser to keep calling the game frame function.

## 2. Constants

The `#define` lines give names to fixed values such as the screen size and number of bubbles, fish, and obstacles. This avoids repeating unexplained numbers throughout the program.

## 3. Game state

The `GameState` enum has three values:

- `MENU`: show the title and Start button.
- `PLAYING`: update the submarine and run the mission.
- `COMPLETE`: show the result after the treasure is collected.

The program uses conditions to decide which update and drawing functions to call.

## 4. Structures

A structure groups information that belongs to one object:

- `Submarine` stores its position and movement speed.
- `Bubble` stores position, radius, and rising speed.
- `Fish` stores position, horizontal speed, and color.
- `Obstacle` stores the center and collision radius of a rock.
- `Treasure` stores its position and whether it has already been collected.

For example, `submarine.position.x` is the submarine's horizontal position.

## 5. Arrays and loops

The fish, bubbles, and obstacles are stored in arrays. A `for` loop updates or draws each item. This is simpler than writing separate code for every fish or bubble.

## 6. Movement and conditions

`UpdateGame` reads keyboard and touch input. It calculates a possible next position and checks it before changing the submarine's real position.

`IsBlocked` checks the submarine against each obstacle using `CheckCollisionCircles`. If a rock blocks the next position, the submarine does not move into it and a short warning appears.

The movement uses `GetFrameTime()`. This makes movement depend on elapsed time instead of assuming every device draws exactly the same number of frames per second.

## 7. Treasure interaction

`NearTreasure` checks whether the submarine is close enough to the chest. The SPACE key or COLLECT button only succeeds when the submarine is within that range.

After collection, `treasure.collected` becomes `true` and the game changes to the `COMPLETE` state. This prevents collecting the same treasure repeatedly.

## 8. File handling

`SaveMissionResult` demonstrates standard C file handling:

1. `fopen("mission_result.txt", "w")` opens a text file for writing.
2. The program checks whether the returned pointer is `NULL`. A NULL pointer means the file could not be opened.
3. `fprintf` writes the mission name, result, and time.
4. `fclose` closes the file and its result is checked.

In the WebAssembly version, this file is written into Emscripten's virtual file system. This project does **not** currently copy it to the phone's Downloads folder or promise that it survives a page refresh. The result is for the current browser game session.

## 9. Main loop

`UpdateDrawFrame` is called repeatedly:

- Update the moving environment.
- Update the active game state.
- Begin drawing.
- Draw the correct screen.
- End drawing.

On desktop, the program calls this function inside a `while` loop. In the WebAssembly build, Emscripten calls it using its browser main-loop function.

## 10. Build configuration

`CMakeLists.txt` sets the language standard to C17, downloads raylib 6.2 as a build dependency, and links raylib to the game.

The GitHub Actions workflow installs Emscripten and builds the HTML, JavaScript, and WebAssembly files in the cloud.

# Code Explanation

The game source is in [`src/main.c`](../src/main.c). It uses C17 and raylib 6.2.

## 1. Header files

- `raylib.h` provides drawing, keyboard, mouse, touch, timing, and collision functions.
- `stdio.h` provides `FILE`, `fopen`, `fprintf`, and `fclose`.
- The Emscripten header is included only for a browser build. It lets the browser repeatedly call the game frame function.

## 2. Constants

The `#define` lines give names to fixed values such as the screen size and number of bubbles, fish, and obstacles. This avoids repeating unexplained numbers throughout the program.

## 3. Game state

The `GameState` enum has three values:

- `MENU`: show the title and Start button.
- `PLAYING`: update the submarine and run the mission.
- `COMPLETE`: show the result after the treasure is collected.

Conditions in the main update function decide which screen and logic should run.

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

## 6. Input and movement

`UpdateGame` reads keyboard and touch input. W/A/S/D and the arrow keys change the movement direction. The on-screen directional pad checks the active touch points, so a direction stays active while a finger is held on it.

The one-shot Start and COLLECT buttons accept mouse presses and held touch input. The custom [`src/web_shell.html`](../src/web_shell.html) scales the 16:9 game canvas to fit a landscape browser viewport and shows a rotate-to-landscape message on small portrait screens.

The game uses `GetFrameTime()` to calculate movement from elapsed time instead of assuming every device draws exactly the same number of frames per second. Diagonal movement is reduced so it is not faster than straight movement.

## 7. Boundaries and collisions

The submarine's next position is calculated first. The program keeps that position inside the play area, then calls `IsBlocked` to compare the submarine with each rock using `CheckCollisionCircles`.

If the next position touches a rock, the program does not move the submarine into it and shows a short warning. The automated browser test presses the submarine into a rock and checks that its position stops changing.

## 8. Treasure interaction

`NearTreasure` checks whether the submarine is close enough to the chest. SPACE or the COLLECT button only succeeds when the submarine is within that distance.

After collection, `treasure.collected` becomes `true` and the game changes to the `COMPLETE` state. This prevents collecting the same treasure repeatedly.

## 9. File handling

`SaveMissionResult` demonstrates standard C file handling:

1. `fopen("mission_result.txt", "w")` opens a text file for writing.
2. The program checks whether the returned pointer is `NULL`. A NULL pointer means the file could not be opened.
3. `fprintf` writes the mission name, result, and time.
4. `fclose` closes the file and its result is checked.

In the WebAssembly version, this file is written into Emscripten's virtual file system. It is not copied to the phone's Downloads folder and is not guaranteed to survive a page refresh. The automated browser test reads it from the virtual file system during the running session.

## 10. Main loop

`UpdateDrawFrame` is called repeatedly:

- Update the moving environment.
- Update the active game state.
- Begin drawing.
- Draw the correct screen.
- End drawing.

On desktop, the program calls this function inside a `while` loop. In the WebAssembly build, Emscripten calls it through its browser main-loop function.

## 11. Build configuration

`CMakeLists.txt` sets the language standard to C17, downloads raylib 6.2 as a build dependency, and links raylib to the game.

The GitHub Actions workflow installs Emscripten, compiles the HTML, JavaScript, and WebAssembly files, and runs the Chromium test script.

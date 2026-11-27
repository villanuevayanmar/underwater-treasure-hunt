# Game Algorithm

1. Start the program.
2. Create the raylib window and set the frame rate.
3. Set the game state to `MENU`.
4. While the window is open:
   1. Read the time used by the previous frame.
   2. Update the positions of bubbles and fish.
   3. Check the current game state.
   4. If the state is `MENU`, draw the title screen and wait for Start or Enter.
   5. If the state is `PLAYING`:
      - Read W, A, S, D, arrow keys, and touch controls.
      - Calculate the submarine's next position.
      - Keep the submarine inside the play area.
      - Check the next position against every rock.
      - Move only when the new position is not blocked.
      - Add the frame time to the mission timer.
      - When SPACE or COLLECT is pressed, check the distance to the chest.
      - If the submarine is close enough, collect the treasure and save the result.
   6. If the state is `COMPLETE`, show the result and wait for Play Again or Main Menu.
   7. Draw the screen for the current state.
5. When the window closes, close the program.

## Treasure collection condition

The submarine and treasure are treated as circles. The treasure can be collected only when those circles are close enough and the treasure has not already been collected.

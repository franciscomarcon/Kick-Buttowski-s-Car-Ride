# Kick Buttowski's Car Ride
#### Video Demo: https://youtu.be/Mo3BWCk4uFk

## Description
**Kick Buttowski's Car Ride** is a top-down racing and combat game inspired by the SNES classic *Rock N' Roll Racing*. Developed in **C** using the **Raylib** library.

## Features
* **Dynamic Map Loading:** The game reads level data from `.txt` files located in the `assets/maps/` folder. This allows new tracks to be added without recompiling the source code.
* **Game Save System:** Players can save their current progress to a binary file (`salvamento.bin`). This allows the game state to be persisted and reloaded later, ensuring players don't lose their progress between sessions.
* **Advanced Collision System:** For the player, the game implements a vertex-based collision check. Before any movement or rotation, the system calculates if any of the car's vertices would intersect with a collision block, ensuring precision even at high speeds.
* **Enemy AI:** The AI uses a "look-ahead" logic. It detects obstacles in front of the vehicle and performs a lateral scan to decide the best direction to turn, simulating a basic decision-making process.
* **Power-ups:** Players can collect and use box powers during the race to gain an advantage over opponents.

## Controls
* **Menu:** Use **Arrow Keys** to navigate and **Enter** to select.
* **Driving:** **W, A, S, D** for acceleration and steering.
* **Abilities:** **Spacebar** to activate collected powers.
* **Pause:** Press **P** at any time to pause the game.

## File Structure
* **`src/`**: C source files, split by game responsibility.
* **`include/game.h`**: Shared types, constants, global declarations, and function prototypes.
* **`assets/maps/`**: Track data files.
* **`assets/textures/`**: Graphics, including the inventory bar assets.
* **`assets/music/`**: Background music.
* **`assets/sounds/`**: Sound effects.
* **`Makefile`**: Build rules for Linux and Windows.
* **`README.md`**: Project documentation.

## Design Choices and Challenges
One of the main challenges was implementing the car's rotation and collision simultaneously. Unlike a simple rectangle, a rotating car requires checking rotated coordinates against the tilemap grid. I decided to use a vertex-checking algorithm to ensure the car doesn't "clip" into walls during tight turns.

The game was developed primarily on **Linux (Ubuntu)**, but the Makefile automatically detects the operating system to link the appropriate Raylib and system libraries (OpenGL, WinMM, etc.).
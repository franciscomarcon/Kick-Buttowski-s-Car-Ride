# Kick Buttowski's Car Ride
#### Video Demo: https://youtu.be/Mo3BWCk4uFk

## Description
**Kick Buttowski's Car Ride** is a top-down racing and combat game inspired by the SNES classic *Rock N' Roll Racing*. Developed in **C** using the **Raylib** library.

## Features
* **Dynamic Map Loading:** The game reads level data from `.txt` files located in the `maps/` folder. This allows new tracks to be added without recompiling the source code.
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
* **`project.c`**: The main source code file containing the game loop and logic.
* **`Makefile`**: Script to compile the game on both Linux and Windows.
* **`mapas/`**: Contains `.txt` files used to generate the race tracks dynamically.
* **`texturas/`**: Stores all graphical assets, including car sprites and environment tiles.
* **`musicas/`**: Contains the background music tracks for the game.
* **`sons/`**: Stores sound effects for collisions, power-ups, and UI.
* **`README.md`**: This documentation file.

## Design Choices and Challenges
One of the main challenges was implementing the car's rotation and collision simultaneously. Unlike a simple rectangle, a rotating car requires checking rotated coordinates against the tilemap grid. I decided to use a vertex-checking algorithm to ensure the car doesn't "clip" into walls during tight turns.

The game was developed primarily on **Linux (Ubuntu)**, but the Makefile automatically detects the operating system to link the appropriate Raylib and system libraries (OpenGL, WinMM, etc.).
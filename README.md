# Zelda-sandbox

A 2D sandbox for the mechanics of *Tears of the Kingdom* and *Breath of the Wild*, built in C++ and SDL2 (no engine, no physics library). I want to build almost all of the mechanics in those two games, as many as I have time for. It is the second project after [Zelda-Link.cpp](https://github.com/Trojanhammer/Zelda-Link.cpp), and a way to keep learning C++ and game physics.

**How it is organised:** every mechanic is built first in its own small sandbox, on its own, so I can understand it, test it and get it right before anything else gets in the way. Once each one works well, I will consider merging them into one world.

![The left stick aims, X throws the sword, O recalls it along the path it flew](docs/throw-and-recall.gif)

*Recall, the first mechanic: the left stick aims, X throws, O recalls (recorded from the game itself, played by a scripted virtual gamepad).*

> **How this was built (AI use):** the mechanics and the physics (`Weapon`, `Player`, the throw, gravity, landing, friction, the recall, walking and picking the sword up) are written by me, by hand: I discuss an idea with Claude, write it myself until I understand it, and ask Claude to review it. The exceptions, written with AI to move faster: the SDL2 window and drawing helpers (`src/graphics/`, including the legs that swing when Link walks; `src/ui/`, which draws text with a built-in pixel font), the gamepad code (`src/input/`), the drawing lines in `main.cpp`, and the build script.

> **Disclaimer:** This is an unofficial fan project with no affiliation to Nintendo. *The Legend of Zelda* and its characters belong to Nintendo; this is built purely for personal learning and hobby purposes, with no plan to release or distribute it.

## Mechanics

| Mechanic | Status |
|---|---|
| Recall: throw a sword, then call it back along the way it came | done: aim and throw, rewind along the path it flew (in the air or from the grass), the sword falls if you walked away, and you can pick it up again. Still to do: the recall effects (see below) |
| Updraft: fire heats the air above it and lifts things up | planned (Link can walk now, which this one needed) |
| Magnesis: move a metal object with the stick | planned |

These three come first, each in its own sandbox. After that, more mechanics from the two games as time allows.

### How the recall works here

- While the sword flies it records its position every frame. The recall plays those dots backwards and blends between the two nearest ones, so it moves exactly along the path it flew and takes as long as the flight, at any frame rate.
- You can recall in the air or after it landed. The sword ends where the throw began. If Link is still standing there it is back in his hand; if he has walked away, it falls to the ground (about 0.3 s from hand height) and can be recalled again.
- The recall works for 3 seconds after the throw began, and the flight counts: a 1.4 s flight leaves about 1.6 s after it lands. After that the path is forgotten and the button does nothing. A sword that lies still does not make the rewind wait.
- Walk up to a sword lying on the ground (within 50 px) to pick it up: it jumps into his hand and the old path is cleared.

### Recall effects (not built yet)

In *Tears of the Kingdom* a recalled object also plays animations and visual effects (a glow, for example) so you can see that it is being recalled. Here the sword only moves back, with no effect yet. The dots needed to draw a trail along the path are already recorded, so adding one is mostly a graphics job.

## Build and run

```bash
./build.sh && ./sandbox
```

Needs SDL2 and SDL2_image (`brew install sdl2 sdl2_image`).

## Controls

| Input | Action |
|---|---|
| Left / Right arrow | Walk |
| Left stick | Aim the throw (0 to 180 degrees) |
| X | Throw the sword (while it is in his hand) |
| O | Recall the sword (in the air or on the ground) |
| Esc | Quit |

## Files

| File | Purpose |
|---|---|
| `src/main.cpp` | Window, loop, input |
| `src/Weapon.h/.cpp` | The sword: position, speed, state (held, thrown, idle, recalling, falling), the recorded path and the recall |
| `src/Player.h/.cpp` | The player walks, holds the sword, throws it and picks it up again; the physics of the throw |
| `src/graphics/` | Drawing only: ground, hand, a sword that turns, a dotted aim line, a marker, loading / drawing / turning sprites, and Link with swinging legs |
| `docs/` | The GIF and the screenshot shown above |
| `assets/` | `link.png` (Link without a weapon) and `master-sword.png`, pixel art generated with AI and cleaned up (background removed, shrunk, 24 colors) |
| `src/input/` | Gamepad: the left stick aims (as a throw angle), X throws, O recalls |
| `src/ui/` | Text drawn with a small built-in 5x7 pixel font (no SDL_ttf) |

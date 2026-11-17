# Level Roulette

**Version:** v0.1.0  
**Mod ID:** `mfkr0001.level_roulette`  
**Target:** Geometry Dash 2.2081 / Geode v5.11.2  
**Initial platform target:** Android ARM64

Level Roulette adds a wheel-based level picker to the Geometry Dash main menu. Players can maintain a local list of level IDs and spin a colorful wheel to choose one at random.

## Features

- Main-menu button that opens the Level Roulette popup.
- Wheel rendered as colored sectors with level IDs.
- Spin animation with gradual easing and a fixed top pointer.
- Numeric ID validation and duplicate prevention.
- Add and remove level IDs.
- Local persistence through Geode saved values.
- Android ARM64 GitHub Actions build.

## Use

1. Install the mod through Geode.
2. Open Level Roulette from the main-menu bottom row.
3. Enter a numeric level ID and press `+`.
4. Press `SPIN` to animate the wheel and select a level.
5. Use `REMOVE LAST` to remove the most recently added ID.

The mod selects and displays an ID. It does not automatically open or download the selected level.

## Build

This repository uses `geode-sdk/build-geode-mod@main` with the `Android64` target. The workflow uses the Geode SDK version declared in `mod.json`.

The build artifact is published by GitHub Actions as `Level-Roulette-Android64`.

## Project structure

- `src/LevelManager.*` — validation, list management and persistence.
- `src/RouletteWheel.*` — sector rendering, spin animation and winner calculation.
- `src/RoulettePopup.*` — popup controls and result display.
- `src/main.cpp` — main-menu integration.

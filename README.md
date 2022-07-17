# Accursed Fiend

A dungeon crawling adventure for the GameTank

| ![Recoiling after being attacked by a rat](screenshots/screenshot_00.bmp) | ![Fighting a bat near the stairs to the next level](screenshots/screenshot_01.bmp) | ![Avoiding a flurry of arrows](screenshots/screenshot_02.bmp) |
|-----------------|--------------------|--------------------|

## Playing

After downloading the First Edition release on github, or building via the instructions below,
you can flash fiend.gtr onto a 2MB GameTank cartridge, pop it into the system, and play!

...or in the much more likely scenario at time of writing, you can play it in the GameTank Emulator!
You can either use the [web emulator](https://clydeshaffer.com/builds/GameTankEmulator/wasm/?rom=fiend.gtr) or
use the [desktop version](https://github.com/clydeshaffer/GameTankEmulator).

Windows builds of the emulator are [generated nightly](https://clydeshaffer.com/builds/GameTankEmulator/latest.php), but OSX and Linux versions can be built from the GameTankEmulator
repo.


## Project notes

Accursed Fiend pushes new limits on the GameTank and demonstrates more effects possible on the hardware.
This is the first game for the system to feature smooth full-screen scrolling and dither-based fade transitions.
It also debuts a queueing system for drawing, so that CPU and blitter operations can be better overlapped.
Finally, this engine uses the graphics memory to its fullest and supports spritesheets/tilemaps up to 256x256!

## Building

Build requires cc65, can be located anywhere as long as cc65/bin is in your PATH

make emulate assumes emulator located at ../GameTankEmulator/bin/Windows_NT/GameTankEmulator.exe

make flash assumes eepromProgrammer repo located at ../eepromProgrammer

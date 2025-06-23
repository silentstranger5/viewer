# Image Viewer written in C with SDL

This is a simple image viewer written in C with SDL.

## How to Build

```sh
git clone https://github.com/silentstranger5/viewer
cd viewer
cmake -B build -S .
cmake --build build
```

## How to use

- Ctrl+O to open a file.
- Scroll mouse up and down to zoom.
- Drag your mouse while the mouse button is down to move the image.
- R to restore original zoom and position.

## About

This is a simple image viewer.
It supports browsing, zooming and dragging. 
This viewer was built to support more image formats than is supported by default on Windows. 
More specifically, it allows to browse PNM image files (including PBM, PGM and PPM).
Note that unlike most image viewers, this one uses nearest texture scaling filter.

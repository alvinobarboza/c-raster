# Software Rasterizer in C

Calculations done by software and passed to an array of [raylib](https://github.com/raysan5/raylib) Colors to be render on a texture the size of the screen with auto resize calculation.

Rasterizer implementation inspired on this book;

https://gabrielgambetta.com/computer-graphics-from-scratch/

-   [x] Draw Lines
-   [x] Draw Filled Triangle
-   [x] Shaded Triangle
-   [x] Perspective Projection
-   [x] Describing and rendering a scene (With camera controls)
-   [x] Clipping
    -   Full frustum culling - Near, far, left, right, top and bottom planes.
    -   Automatically calculate frustum on screen resize.
    -   Dynamic mesh generation on clipping plane.
-   [x] Back face culling
-   [ ] Depth buffer
-   [ ] Shading
-   [ ] Texture
-   [ ] More...

# Showcase

## Frustum culling and clipping planes

![Bottom and far plane clipping on view](./frustum_culling.png)

# Building

On windows it uses `Mingw-w64-x86_64-winpthreads` for building and `GnuWin32` make, on Linux, I can't garantee for now, exactly all dependencies but, `build-essentials` of your platform should work, also, it depends on X11, so maybe, further configuration is need.
With this in mind, just do the following;

-   build process expects a `bin` directory, so, you should create it `mkdir bin`(on powershell all command will work as expected).

```bash
$ make release && ./bin/c_raster
```

To run in debug mode, do the following;

```bash
$ make run
```

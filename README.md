# Software Rasterizer in C

Calculations done by software and passed to an array of [raylib](https://github.com/raysan5/raylib) Colors to be render on a texture the size of the screen with auto resize calculation.

Rasterizer implementation inspired on this book;

https://gabrielgambetta.com/computer-graphics-from-scratch/

- [x]   Draw Lines
- [x]  Draw Filled Triangle
- [x]  Shaded Triangle
- [x]  Perspective Projection
- [x]  Describing and rendering a scene (With camera controls)
- [x]  Clipping
    -   Full frustum culling - Near, far, left, right, top and bottom planes.
    -   Automatically calculate frustum on screen resize.
    -   Dynamic mesh generation on clipping plane.
- [ ]  Backface culling
- [ ]  Shading
- [ ]  Texture
- [ ]  More...

# Showcase

## Frustum culling and clipping planes

![Bottom and far plane clipping on view](./frustum_culling.png)

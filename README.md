# Software Rasterizer in C

Calculations done by software and passed to an array of [raylib](https://github.com/raysan5/raylib) Colors to be render on a texture the size of the screen with auto resize calculation.

Rasterizer implementation inspired on this book;

https://gabrielgambetta.com/computer-graphics-from-scratch/

-   Draw Lines = Done
-   Draw Filled Triangle = Done
-   Shaded Triangle = Done
-   Perspective Projection = Done
-   Describing and rendering a scene = Done (With camera controls)
-   Clipping = Done
    -   Full frustum culling - Near, far, left, right, top and bottom planes.
    -   Automatically calculate frustum on screen resize.
    -   Dynamic mesh generation on clipping plane.

# Showcase

## Frustum culling and clipping planes

![Bottom and far plane clipping on view](./frustum_culling.png)

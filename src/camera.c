#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "camera.h"

void swap_point_values(Point *p1, Point *p2) {
    Point temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

void put_pixel(Cam c, Color color, int x, int y) {
    x = c.width/2 + x;
	y = c.height/2 - y - 1;

	if (x < 0 || x >= c.width || y < 0 || y >= c.height) {
		return;
	}

	c.canvas[y*c.width+x] = color;
}

void clear_canvas(Cam c) { 
    size_t length = c.height * c.width;
    for(size_t i = 0; i < length; i++) {
        c.canvas[i].r = 50;
        c.canvas[i].g = 50;
        c.canvas[i].b = 50;
        c.canvas[i].a = OPAQUE;
    }
}

Point viewport_to_canvas(Cam c, float x, float y) {
    return (Point) {
        .brightness = 1.0f,
        .x = x * (c.width/c.view.width),
        .y = y * (c.height/c.view.height)
    };
}

Point project_vertex(Cam c, Vec3 v) {
    return viewport_to_canvas(
        c, 
        v.x * c.view.d / v.z,
        v.y * c.view.d / v.z
    );
}

Cam init_camera(int w, int h, Vec3 position, Vec3 rotation) {

    // From "LearnOpenGl"
    const float fov = DEFAULT_FOV/VIEW_PLANE_DISTANCE; // from book

    const Vec3 front = (Vec3){.x=0,.y=0,.z=1.0f};
    const Vec3 up = (Vec3){.x=0,.y=1.0f,.z=0};
    const Vec3 leftDirection = (Vec3){.x=-1.0f,.y=0,.z=0};
    const Vec3 rightDirection = (Vec3){.x=1.0f,.y=0,.z=0};
    const Vec3 topDirection = (Vec3){.x=0,.y=1.0f,.z=0};
    const Vec3 bottomDirection = (Vec3){.x=0,.y=-1.0f,.z=0};

    const float aspectRatio = (float)w / (float)h;
    const float halfVSide = FAR_PLANE_DISTANCE * tanf(fov * 0.5f * T_DEG2RAD);
    const float halfHSide = halfVSide * aspectRatio;
    const Vec3 frontMultFar = vec3_multiply( front, FAR_PLANE_DISTANCE );
    Vec3 rightPlanePosition = vec3_add( frontMultFar, vec3_multiply(rightDirection, halfHSide) );
    Vec3 leftPlanePosition = vec3_add( frontMultFar, vec3_multiply(leftDirection, halfHSide) );
    Vec3 topPlanePosition = vec3_add( frontMultFar, vec3_multiply(topDirection, halfVSide) );
    Vec3 bottomPlanePosition = vec3_add( frontMultFar, vec3_multiply(bottomDirection, halfVSide) );
    

    ViewPlane near = (ViewPlane) {
        .normal = (Vec3){.x = 0, .y = 0, .z = 1.0f},
        .d = -NEAR_PLANE_DISTANCE
    };

    ViewPlane far = (ViewPlane) {
        .normal = (Vec3){.x = 0, .y = 0, .z = -1.0f},
        .d = FAR_PLANE_DISTANCE
    };

    ViewPlane right = (ViewPlane) {
        .normal = vec3_normal(vec3_cross( rightPlanePosition, up )),
        .d = 0
    };
    
    ViewPlane left = (ViewPlane) {
        .normal = vec3_normal(vec3_cross( up, leftPlanePosition )),
        .d = 0
    };
    
    ViewPlane top = (ViewPlane) {
        .normal = vec3_normal(vec3_cross( rightDirection,topPlanePosition )),
        .d = 0
    };
    
    ViewPlane bottom = (ViewPlane) {
        .normal = vec3_normal(vec3_cross(  bottomPlanePosition, rightDirection )),
        .d = 0
    };

    Cam camera = (Cam) {
        .canvas = malloc(sizeof(Color) * w * h),
        .depthBuffer = malloc(sizeof(float) * w * h),
        .width = w,
        .height = h,
        .view = (Viewport) {.d = VIEW_PLANE_DISTANCE, .height = 1.0f, .width =  aspectRatio },
        .scale = (Vec3) {.x = 1.0f, .y = 1.0f, .z = 1.0f},
        .position = position,
        .rotation = rotation,
        .forwardDirection = (Vec3) { .x = 0.0f, .y = 0.0f, .z = 1.0f},
        .frustum = {near, far, left, right, top, bottom}
    };
    
    camera.matrixTransform = init_matrix();
    update_camera_transforms(&camera);

    return camera;
}

void free_camera(Cam c){
    if(c.canvas != NULL){
        free(c.canvas);
    }
    if(c.depthBuffer != NULL){
        free(c.depthBuffer);
    }
    if(c.matrixTransform != NULL){
        free(c.matrixTransform);
    }
}

void update_camera_transforms(Cam *c){
    if ( c->matrixTransform == NULL) {
        return;
    }

    float scale[M4X4];
    float rotation[M4X4];
    float rotationTransposed[M4X4];
    float translation[M4X4];
    float result[M4X4];

    make_scale_matrix(scale, c->scale);

    make_rotation_matrix(rotation, c->rotation);
    matrix_transpose(rotation, rotationTransposed);

    make_translation_matrix(translation, vec3_multiply(c->position, -1));

    matrix_multiplication(scale, rotationTransposed, result);
    matrix_multiplication(result, translation,  c->matrixTransform);
}

void update_camera_frustum(Cam *c, int w, int h) {
    c->height = h;
    c->width = w;

    c->canvas = realloc(c->canvas, sizeof(Color) * c->height * c->width);
    if (c->canvas == NULL) {
        return;
    }
    
    c->depthBuffer = realloc(c->depthBuffer, sizeof(float) * c->height * c->width);
    if (c->depthBuffer == NULL) {
        free(c->canvas);
        c->canvas = NULL;
        return;
    }


    c->view.width = (float)c->width / (float)c->height;

    const float fov = DEFAULT_FOV/VIEW_PLANE_DISTANCE; 

    const Vec3 front = (Vec3){.x=0,.y=0,.z=1.0f};
    const Vec3 up = (Vec3){.x=0,.y=1.0f,.z=0};
    const Vec3 leftDirection = (Vec3){.x=-1.0f,.y=0,.z=0};
    const Vec3 rightDirection = (Vec3){.x=1.0f,.y=0,.z=0};
    const Vec3 topDirection = (Vec3){.x=0,.y=1.0f,.z=0};
    const Vec3 bottomDirection = (Vec3){.x=0,.y=-1.0f,.z=0};
    
    const float aspectRatio = (float)w / (float)h;
    const float halfVSide = FAR_PLANE_DISTANCE * tanf(fov * 0.5f * T_DEG2RAD);
    const float halfHSide = halfVSide * aspectRatio;
    const Vec3 frontMultFar = vec3_multiply( front, FAR_PLANE_DISTANCE );
    Vec3 rightPlanePosition = vec3_add( frontMultFar, vec3_multiply(rightDirection, halfHSide) );
    Vec3 leftPlanePosition = vec3_add( frontMultFar, vec3_multiply(leftDirection, halfHSide) );
    Vec3 topPlanePosition = vec3_add( frontMultFar, vec3_multiply(topDirection, halfVSide) );
    Vec3 bottomPlanePosition = vec3_add( frontMultFar, vec3_multiply(bottomDirection, halfVSide) );

    c->frustum[2].normal = vec3_normal(vec3_cross( up, leftPlanePosition ));
    c->frustum[3].normal = vec3_normal(vec3_cross( rightPlanePosition, up ));
    c->frustum[4].normal = vec3_normal(vec3_cross( rightDirection,topPlanePosition ));
    c->frustum[5].normal = vec3_normal(vec3_cross(  bottomPlanePosition, rightDirection ));    
}

void camera_move_forward(Cam *c, float unit){
    float rotation[M4X4];
    make_rotation_matrix(rotation, c->rotation);

    Vec3 direction = mult_matrix_by_vec3(rotation, c->forwardDirection);
	Vec3 normalDirection = vec3_normal(direction);

	c->position.x += normalDirection.x * unit;
	c->position.y += normalDirection.y * unit;
	c->position.z += normalDirection.z * unit;

    update_camera_transforms(c);
}

void camera_move_backward(Cam *c, float unit){
    camera_move_forward(c, -unit);
}

void camera_move_up(Cam *c, float unit){
    c->position.y += unit;
    update_camera_transforms(c);
}

void camera_move_down(Cam *c, float unit){
    camera_move_up(c, -unit);
}

void camera_move_left(Cam *c, float unit){
    float rotation[M4X4];
    make_rotation_matrix(rotation, c->rotation);

    Vec3 direction = mult_matrix_by_vec3(rotation, c->forwardDirection);
	Vec3 sideDirection = vec3_cross(direction, (Vec3){.x = 0, .y = 1, .z = 0});
	Vec3 normalDirection = vec3_normal(sideDirection);

	c->position.x += normalDirection.x * unit;
	c->position.y += normalDirection.y * unit;
	c->position.z += normalDirection.z * unit;

    update_camera_transforms(c);
}

void camera_move_right(Cam *c, float unit){
    camera_move_left(c, -unit);
}

void camera_turn_left(Cam *c, float unit){
    c->rotation.y += unit;
    update_camera_transforms(c);
}

void camera_turn_right(Cam *c, float unit){
    camera_turn_left(c, -unit);
}

void camera_turn_up(Cam *c, float unit){
    c->rotation.x += unit;

    if (c->rotation.x < -89.0f){
        c->rotation.x = -89.0f;
    } else if (c->rotation.x > 89.0f){
        c->rotation.x = 89.0f;
    }

    update_camera_transforms(c);
}

void camera_turn_down(Cam *c, float unit){
    camera_turn_up(c, -unit);
}
#ifndef _SCENE_H
#define _SCENE_H

#include "models.h"

typedef struct Scene {
    Instance *instances;
    size_t objectCount;
} Scene;


#endif
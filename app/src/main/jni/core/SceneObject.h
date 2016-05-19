#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "global.h"

#include <map>
#include <string>

struct RenderInfo;

class SceneObject
{
public:
    SceneObject(){}

    virtual ~SceneObject(){}

// draw object
    virtual void draw(const RenderInfo *ri) = 0;

// move object by time step = dt
    virtual void updateFrame(float dt) {};

// reset object state to default
    virtual void restart() {}

// notify about screen size change
    virtual void onResize() {}

// Save object state
    virtual void save(Dictionary&) {};

// Load object data
    virtual void load(Dictionary&) {};
};

#endif // SCENEOBJECT_H

#ifndef TERRAIN_H
#define TERRAIN_H

#include "SceneObject.h"

class Terrain : public SceneObject
{
public:
    Terrain();

    ~Terrain();

    virtual void draw(const RenderInfo *ri) override;

    virtual void onResize() override;

private:

    void render();

    GLuint _vbufId;
};

#endif // TERRAIN_H

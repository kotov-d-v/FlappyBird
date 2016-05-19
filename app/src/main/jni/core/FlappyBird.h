#ifndef BIRD_H
#define BIRD_H

#include "SceneObject.h"

class FlappyBird : public SceneObject
{
public:

    FlappyBird();

    ~FlappyBird();

    virtual void draw(const RenderInfo *ri) override ;

    virtual void updateFrame(float dt) override;

    virtual void restart() override;

    virtual void save(Dictionary&) override;

    virtual void load(Dictionary&) override;

    math::Circle bounds();
    math::Vector2D velocity();
    math::Vector2D acceleration();

    static float radius() {return _r;}

    void flap();

    bool alive() {return _alive;}

    void die();

private:

    void render();

    GLuint _vbufId;
    float _cx, _cy; // current coordinates
    float _vx, _vy; // current velocity
    float _ax, _ay; // current acceleration
    bool _alive;

    static const float _r; // "radius" of the bird
};

#endif

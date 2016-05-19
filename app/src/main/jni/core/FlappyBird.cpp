#include "FlappyBird.h"
#include "GameEngine.h"
#include "util.h"
#include "math.h"

const float FlappyBird::_r = 0.05f;

FlappyBird::FlappyBird() :
    _vbufId(0)
{
    restart();
    render();
}

FlappyBird::~FlappyBird()
{
    glDeleteBuffers(1, &_vbufId);
}

void FlappyBird::flap()
{
    _vy = 0.5f;
}

void FlappyBird::render()
{
    if(!_vbufId) glGenBuffers(1, &_vbufId);

    static const float buf[] = {
        -_r, _r,        -_r, -_r,       _r, _r,     _r, -_r
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FlappyBird::draw(const RenderInfo *ri)
{
    static const GLfloat white[] = {1, 1, 1, 1};
    static const GLfloat black[] = {0, 0, 0, 1};

    const float sc = 0.5f/_r;
    float vx = _vx + GameEngine::cameraVelocity();
    float alpha = _vy*vx == 0 ? 0 : atan2f(_vy, vx);
    alpha *= 0.5f; // make angle smaller for natural look

// apply transformation
    egl::transRotMatrix(ri->modelMatrixHandle, _cx, _cy, alpha);

    egl::transScaleMatrix(ri->texMatrixHandle, 0.5f, 0.5f, sc, sc);

// set texture
    glBindTexture(GL_TEXTURE_2D, GameEngine::engine()->getTextureID(TEX_BIRD));
    glUniform1i(ri->textureHandle, 0);

// tail (single triangle, no use of VBO)
    static const float tail[] = {
            -0.7f*_r, 0,    -1.3f*_r, -0.1f*_r,    -1.4f*_r, 0.3f*_r
    };
    glUniform4fv(ri->colorHandle, 1, black);
    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 0, tail);
    glDrawArrays(GL_TRIANGLES, 0, 3);

// body
    glUniform4fv(ri->colorHandle, 1, white);
    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ri->textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glEnableVertexAttribArray(ri->textureCoordHandle);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

// finish
    glDisable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(ri->textureCoordHandle);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FlappyBird::die()
{
    _alive = false;
    GameEngine::engine()->gameOver();
}

void FlappyBird::updateFrame(float dt)
{
    if(!_alive)
    {
        _cx -= GameEngine::cameraVelocity()*dt;
        return;
    }

    const float vmax = 1.f;
    if(_vy <= -vmax) // no acceleration for high velocity
    {
        _vy = -vmax;
        _ay = 0;
    }
    else
    {
        _ay = -GameEngine::gravity();
    }

// exact integration from (c0, v0 to c1, v1)
    _cy += _vy*dt + 0.5f*_ay*dt*dt;
    _vy += _ay*dt;

    float ymin = GameEngine::bottom() + _r;
    float ymax = GameEngine::top() - _r;

    if(_cy <= ymin)
    {// touch bottom
        _cy = ymin;
        die();
    }
    else if(_cy >= ymax)
    {// touch top
        _cy = ymax;
        die();
    }
}

math::Circle FlappyBird::bounds()
{
    return math::Circle(_cx, _cy, _r);
}

math::Vector2D FlappyBird::velocity()
{
    return math::Vector2D(_vx, _vy);
}

math::Vector2D FlappyBird::acceleration()
{
    return math::Vector2D(_ax, _ay);
}

void FlappyBird::restart()
{
    _cx = 0.25f+_r;
    _cy = 0.8f;
    _vx = 0;
    _vy = 0;
    _ax = 0;
    _ay = 0;
    _alive = true;
}

void FlappyBird::save(Dictionary &d)
{
    d["bird.cx"] = util::encode(_cx);
    d["bird.cy"] = util::encode(_cy);
    d["bird.vx"] = util::encode(_vx);
    d["bird.vy"] = util::encode(_vy);
    d["bird.alive"] = util::encode(int(_alive));
}

void FlappyBird::load(Dictionary &d)
{
    _cx = util::decodeF(d["bird.cx"]);
    _cy = util::decodeF(d["bird.cy"]);
    _vx = util::decodeF(d["bird.vx"]);
    _vy = util::decodeF(d["bird.vy"]);
    _alive = (bool) util::decodeI(d["bird.alive"]);
}

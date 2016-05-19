#ifndef POLESET_H
#define POLESET_H

#include "SceneObject.h"

class PoleSet : public SceneObject
{
public:
    PoleSet(int Npoles, float* pInfo);
    PoleSet(int Npoles, int levelId);

    ~PoleSet();

    virtual void draw(const RenderInfo *ri) override ;

    virtual void updateFrame(float dt) override;

    virtual void restart() override {resetPosition();}

    virtual void onResize() override {updateIndexRange();}

    virtual void load(Dictionary&) override;

    // get bottom rectangle coordinates of the given pole
// IN:
// poleIndex - index of the pole (start from 0)
// OUT:
// rc - left, top, right, bottom values of the rectangle
    void getBottomRect(int poleIndex, math::Rect* rc);

// get top rectangle coordinates of the given pole
// IN:
// poleIndex - index of the pole (start from 0)
// OUT:
// rc - left, top, right, bottom values of the rectangle
    void getTopRect(int poleIndex, math::Rect* rc);

    int beginIndex() const { return _beginIndex; }

    int endIndex() const { return _endIndex; }

    int count() const { return _Npoles; }

private:
    void init(int Npoles);

    void initFinish();

    void render();

    void loadLevel(float* pInfo);

    void generateLevel(int levelId);

    void createRect(float* rect, float x0, float y0, float h);

    void resetPosition();

    void updateIndexRange();

    float getPoleLeft(int poleIndex);

    float getPoleRight(int poleIndex);

    static float _width;

    float _coordX;

    float* _vbuf;   // vertex buffer
    GLuint _vbufId; // ID of GL vertex buffer

    int _Npoles;
    int _beginIndex, _endIndex; // index range of poles in viewport
};

#endif // POLESET_H

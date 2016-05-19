#include "PoleSet.h"
#include "FlappyBird.h"
#include "GameEngine.h"

#define BUFPT_SIZE 24

float PoleSet::_width = 0.1f;

PoleSet::PoleSet(int Npoles, float* pInfo)
{
    init(Npoles);
    loadLevel(pInfo);
    initFinish();
}

PoleSet::~PoleSet()
{
    glDeleteBuffers(1, &_vbufId);

    delete[] _vbuf;
}

PoleSet::PoleSet(int Npoles, int levelId)
{
    init(Npoles);
    generateLevel(levelId);
    initFinish();
}

void PoleSet::init(int Npoles)
{
    _coordX = 0;
    _Npoles = Npoles;
    _vbufId = 0;
    _vbuf = new float[_Npoles*BUFPT_SIZE];
}

void PoleSet::initFinish()
{
    resetPosition();
    render();
}

void PoleSet::render()
{
    if(!_vbufId) glGenBuffers(1, &_vbufId);

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_Npoles*BUFPT_SIZE,
                 _vbuf, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PoleSet::draw(const RenderInfo *ri)
{
    static const GLfloat color[] = {1, 1, 1, 1};

    int count = _endIndex - _beginIndex;
    if(count <= 0) return;

    egl::transMatrix(ri->modelMatrixHandle, _coordX, 0);
    egl::scaleMatrix(ri->texMatrixHandle, 1.0f/_width, 1.0f);

    glUniform4fv(ri->colorHandle, 1, color);

    glBindTexture(GL_TEXTURE_2D, GameEngine::engine()->getTextureID(TEX_POLE));
    glUniform1i(ri->textureHandle, 0);

    void* vOffset = (void*) ( sizeof(float)*BUFPT_SIZE*_beginIndex );
    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 0, vOffset);
    glVertexAttribPointer(ri->textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, vOffset);

    glEnableVertexAttribArray(ri->textureCoordHandle);

    glDrawArrays(GL_TRIANGLES, 0, (BUFPT_SIZE/2)*count);

    glDisableVertexAttribArray(ri->textureCoordHandle);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PoleSet::updateFrame(float dt)
{
    _coordX = -GameEngine::engine()->cameraPosition();
    updateIndexRange();
}

// load pixels from level info pInfo
// pInfo contains Npoles elements of 3 floats each:
// pInfo[0] = coordinate of the left side 
// pInfo[1] = coordinate of bottom pole top 
// pInfo[2] = gap between top and bottom poles
void PoleSet::loadLevel(float* pInfo)
{
    float* infoEnd = pInfo + _Npoles;
    float* dest = _vbuf;

    for(; pInfo != infoEnd; pInfo+=3, dest += BUFPT_SIZE)
    {
        createRect(dest, pInfo[0], pInfo[1], pInfo[2]);
    }
}

void PoleSet::generateLevel(int levelId)
{
    math::srand(levelId); // fix randomizer for the given level
    float* destEnd = _vbuf + _Npoles*BUFPT_SIZE;

// gap between poles
    const float gap = 6.f*FlappyBird::radius(); // 3 sizes of the bird, in future might depend on level

// gap start minimum
    const float y0 = 0.2f;    // in future might depend on level

// gap start range
    const float range = 0.01f*(0.95f - y0 - gap);
    
    float x = 1.f;      // start poles at
    for(float* dest = _vbuf; dest != destEnd; dest += BUFPT_SIZE)
    {
        float y = y0 + range*(math::rand() % 100); // discretize h by 100
        x += _width*(3 + math::rand() % 5);        // discretize x by 5
        createRect(dest, x, y, gap);
    }
}

// Fill rect buffer coordinates from the given pole parameters
// IN:
// x0 - left x coordinate
// y0 - gap start
// h - gap height
// OUT:
// rect - buffer with 2D point coordinates
void PoleSet::createRect(float* rect, float x0, float y0, float h)
{
    float x1 = x0 + _width;
    float y1 = GameEngine::bottom();

// bottom pole
    rect[0] = x0;
    rect[1] = y0;

    rect[2] = x0;
    rect[3] = y1;

    rect[4] = x1;
    rect[5] = y1;

    rect[6] = x1;
    rect[7] = y1;

    rect[8] = x1;
    rect[9] = y0;

    rect[10] = x0;
    rect[11] = y0;


// top pole
    y1 = 1.f;
    y0 += h; 

    rect[12] = x0;
    rect[13] = y0;

    rect[14] = x0;
    rect[15] = y1;

    rect[16] = x1;
    rect[17] = y1;

    rect[18] = x1;
    rect[19] = y1;

    rect[20] = x1;
    rect[21] = y0;

    rect[22] = x0;
    rect[23] = y0;
}

void PoleSet::load(Dictionary &dictionary)
{
    resetPosition();
    updateFrame(0);
}


void PoleSet::resetPosition()
{
    _coordX = 0.f;
    _beginIndex = 0;
    _endIndex = 0;

    updateIndexRange();
}

void PoleSet::updateIndexRange()
{
// update begin index
    for(++_beginIndex; getPoleRight(_beginIndex) < 0.f && _beginIndex < _Npoles; ++_beginIndex);

    if(_beginIndex != _Npoles && getPoleRight(_beginIndex-1) >= 0.f)
        _beginIndex--;

// update end index
    const float w = GameEngine::engine()->width();
    for(; getPoleLeft(_endIndex) < w && _endIndex < _Npoles; ++_endIndex);
}

// get top rectangle coordinates of the given pole
// IN:
// poleIndex - index of the pole (start from 0)
// OUT:
// rc - left, top, right, bottom values of the rectangle
void PoleSet::getTopRect(int poleIndex, math::Rect* rc)
{
    float* pole = _vbuf + poleIndex*BUFPT_SIZE;
    rc->x0 = _coordX + pole[12];
    rc->y0 = pole[13];
    rc->x1 = _coordX + pole[16];
    rc->y1 = pole[15];
}

// get bottom rectangle coordinates of the given pole
// IN:
// poleIndex - index of the pole (start from 0)
// OUT:
// rc - left, top, right, bottom values of the rectangle
void PoleSet::getBottomRect(int poleIndex, math::Rect* rc)
{
    float* pole = _vbuf + poleIndex*BUFPT_SIZE;
    rc->x0 = _coordX + pole[0];
    rc->y1 = pole[1];
    rc->x1 = _coordX + pole[4];
    rc->y0 = pole[3];
}

float PoleSet::getPoleLeft(int poleIndex)
{
    return _coordX + _vbuf[poleIndex*BUFPT_SIZE];
}

float PoleSet::getPoleRight(int poleIndex)
{
    return _coordX + _vbuf[poleIndex*BUFPT_SIZE+4];
}

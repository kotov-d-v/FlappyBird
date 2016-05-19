//
// Created by Dmitry Kotov on 3/31/16.
//

#include "DigitText.h"
#include "GameEngine.h"
#include "util.h"

// rc = rect of the first digit
DigitText::DigitText(float height, int number, Widget* parent) :
    Widget(parent),
    _number(number),
    _vbufId(0),
    _Ndigits(0)
{
    _rect = math::Rect(0, 0, height*0.75f, height); // imply 4x3 digits texture

    render();
}

DigitText::~DigitText()
{
    glDeleteBuffers(1, &_vbufId);
}

void DigitText::draw(const RenderInfo *ri)
{
    static const GLfloat color[] = {1, 1, 1, 1};

    math::Point pos = absPosition();
    egl::transMatrix(ri->modelMatrixHandle, pos.x, pos.y);
    egl::identityMatrix(ri->texMatrixHandle);

    glUniform4fv(ri->colorHandle, 1, color);

    glBindTexture(GL_TEXTURE_2D, GameEngine::engine()->getTextureID(TEX_DIGITS));
    glUniform1i(ri->textureHandle, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);

    glVertexAttribPointer(ri->textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float),
                          (void*)(2*sizeof(float)));

    glEnableVertexAttribArray(ri->textureCoordHandle);
    glEnable(GL_BLEND);

    glDrawArrays(GL_TRIANGLES, 0, _Ndigits*6);

    glDisable(GL_BLEND);
    glDisableVertexAttribArray(ri->textureCoordHandle);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DigitText::setNumber(int num)
{
    _number = num;
    render();
}

void DigitText::render()
{
    const float gap = 0.95;

    if(!_vbufId)
    {
        glGenBuffers(1, &_vbufId);
        if (!_vbufId) return; // failed to create the buffer
    }

    std::string s = util::toString(_number);
    _Ndigits = static_cast<int>( s.size() );

// rect of the first digit
    math::Rect v_rc(0, 0, _rect.width(), _rect.height());

    std::vector<float> buf;
    const int rectSize = 24; // 6x4 = 6 points * sizeof(point)
    buf.resize(rectSize*_Ndigits);

    int ind = 0;
    for(int i = 0; i != _Ndigits; ++i)
    {
// write vertex position buffer
        writeRect(&buf[ind], v_rc, 4);

// compute digit params
        int digit = s[i] == '-' ? 10 : s[i] - '0'; // assume '-' is 10th in texture
        if(digit < 0 || digit > 11) digit = 11; // space

        int di = digit % 4;  // offset in x
        int dj = digit >> 2; // offset in y

// write texture coord buffer
        math::Rect a_rc(0, 0.33333f, 0.25f, 0);  // assume digits are 4x3
        a_rc.move(di*a_rc.width(), -dj*a_rc.height());
        writeRect(&buf[ind+2], a_rc, 4);

// move to next digit
        ind += rectSize;
        v_rc.move(v_rc.width()*gap, 0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*buf.size(), &buf[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DigitText::writeRect(float* buf, const math::Rect& rc, int stride)
{
    int i = 0;
    buf[i]   = rc.x0;
    buf[i+1] = rc.y1;
    i += stride;

    buf[i]   = rc.x0;
    buf[i+1] = rc.y0;
    i += stride;

    buf[i]   = rc.x1;
    buf[i+1] = rc.y1;
    i += stride;

    buf[i]   = rc.x0;
    buf[i+1] = rc.y0;
    i += stride;

    buf[i]   = rc.x1;
    buf[i+1] = rc.y1;
    i += stride;

    buf[i]   = rc.x1;
    buf[i+1] = rc.y0;
}


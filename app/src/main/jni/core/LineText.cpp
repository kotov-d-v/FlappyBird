//
// Created by Dmitry Kotov on 4/2/16.
//

#include "LineText.h"
#include "GameEngine.h"

LineText::LineText(float height, int textId, Widget* parent) :
    Widget(parent),
    _line(textId)
{
    _rect = math::Rect(0, 0, height*LINE_TEXT_LINES, height);
    render();
}

void LineText::draw(const RenderInfo *ri)
{
    static const GLfloat color[] = {1, 1, 1, 1};

    math::Point pos = absPosition();
    egl::transMatrix(ri->modelMatrixHandle, pos.x, pos.y);
    egl::identityMatrix(ri->texMatrixHandle);

    glUniform4fv(ri->colorHandle, 1, color);

    glBindTexture(GL_TEXTURE_2D, GameEngine::engine()->getTextureID(TEX_TEXT));
    glUniform1i(ri->textureHandle, 0);

    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 0, &_vbuf[0]);
    glVertexAttribPointer(ri->textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, &_abuf[0]);

    glEnableVertexAttribArray(ri->textureCoordHandle);
    glEnable(GL_BLEND);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisable(GL_BLEND);
    glDisableVertexAttribArray(ri->textureCoordHandle);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void LineText::render()
{
    float w = _rect.width();
    float h = _rect.height();

    _vbuf[0] = 0;
    _vbuf[1] = h;
    _vbuf[2] = 0;
    _vbuf[3] = 0;
    _vbuf[4] = w;
    _vbuf[5] = h;
    _vbuf[6] = w;
    _vbuf[7] = 0;
    
    float ay1 = 1.f/LINE_TEXT_LINES*_line;
    float ay0 = 1.f/LINE_TEXT_LINES*(_line+1);
    
    _abuf[0] = 0;
    _abuf[1] = ay1;
    _abuf[2] = 0;
    _abuf[3] = ay0;
    _abuf[4] = 1.0f;
    _abuf[5] = ay1;
    _abuf[6] = 1.0f;
    _abuf[7] = ay0;
    
}
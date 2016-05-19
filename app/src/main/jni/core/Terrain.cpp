#include "Terrain.h"
#include "GameEngine.h"

Terrain::Terrain()
{
    render();
}

Terrain::~Terrain()
{
    glDeleteBuffers(1, &_vbufId);
}

void Terrain::render()
{
    if(!_vbufId) glGenBuffers(1, &_vbufId);

    auto engine = GameEngine::engine();

    GLfloat w = engine->width();
    GLfloat h = GameEngine::bottom();
    GLfloat buf[8] = {0, 0, 0, h, w, 0, w, h};

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::onResize()
{
    render();
}

void Terrain::draw(const RenderInfo *ri)
{
    static const GLfloat color[] = {1, 1, 1, 1}; // set white to avoid texture multiplication

    auto engine = GameEngine::engine();

    float sy = 1.0f/GameEngine::bottom();
    float sx = 0.333f*sy;
    float dx = engine->cameraPosition()*sx;
    egl::transScaleMatrix(ri->texMatrixHandle, dx, 0, sx, sy);
    egl::identityMatrix(ri->modelMatrixHandle);

    glUniform4fv(ri->colorHandle, 1, color);

    glBindTexture(GL_TEXTURE_2D, engine->getTextureID(TEX_BRICK));
    glUniform1i(ri->textureHandle, 0);

    glBindBuffer(GL_ARRAY_BUFFER, _vbufId);
    glVertexAttribPointer(ri->positionHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ri->textureCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(ri->textureCoordHandle);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(ri->textureCoordHandle);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

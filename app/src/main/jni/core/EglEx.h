//
// Created by Dmitry Kotov on 3/26/16.
//

#ifndef FLAPPYBIRD_EGLEX_H
#define FLAPPYBIRD_EGLEX_H

#include "sysglobal.h"

namespace egl {

    GLuint loadShader(GLenum type, const char* shaderCode, GLint len);

    GLuint createRGBTexture(const GLsizei width, const GLsizei height, GLint filter, GLubyte *pix);

    GLuint loadTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);

    void identityMatrix(GLint matrixHandle);

    void transMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty);

    void transScaleMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty, GLfloat fx, GLfloat fy);

    void scaleMatrix(GLint matrixHandle, GLfloat fx, GLfloat fy);

    void transRotMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty, GLfloat alpha);

    void ortho(GLint matrixHandle, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
};


#endif //FLAPPYBIRD_EGLEX_H

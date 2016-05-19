//
// Created by Dmitry Kotov on 3/26/16.
//

#include "EglEx.h"
#include <math.h>

namespace egl {

    GLuint loadShader(GLenum type, const char* shaderCode, GLint len)
    {
        GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &shaderCode, &len);
        glCompileShader(shader);

        return shader;
    }

// create texture with a single white pixel to use for non-textured objects
    GLuint createRGBTexture(const GLsizei width, const GLsizei height, GLint filter, GLubyte *pix)
    {
        GLuint textureId;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &textureId);

        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pix);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        return textureId;
    }


    GLuint loadTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid *pixels)
    {
        GLuint texture_object_id;

        glGenTextures(1, &texture_object_id);
        glBindTexture(GL_TEXTURE_2D, texture_object_id);
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);

        glEnable(GL_TEXTURE_2D); // due to the bug on some ATI drivers
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture_object_id;
    }

    void identityMatrix(GLint matrixHandle)
    {
        static const GLfloat mat[16] = {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &mat[0]);
    }

    void transMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty)
    {
        GLfloat mat[16] = {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                tx, ty, 0, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &mat[0]);
    }

    void scaleMatrix(GLint matrixHandle, GLfloat fx, GLfloat fy)
    {
        GLfloat mat[16] = {
                fx, 0,  0, 0,
                0,  fy, 0, 0,
                0,  0,  1, 0,
                0,  0,  0, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &mat[0]);

    }

    void transScaleMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty, GLfloat fx, GLfloat fy)
    {
        GLfloat mat[16] = {
                fx, 0,  0, 0,
                0,  fy, 0, 0,
                0,  0,  1, 0,
                tx, ty, 0, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &mat[0]);
    }

    void transRotMatrix(GLint matrixHandle, GLfloat tx, GLfloat ty, GLfloat alpha)
    {
        float s = sinf(alpha);
        float c = cosf(alpha);
        GLfloat mat[16] = {
                 c, s, 0, 0,
                -s, c, 0, 0,
                 0, 0, 1, 0,
                 tx,ty, 0, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &mat[0]);
    }

    void ortho(GLint matrixHandle, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, float far)
    {
        GLfloat a = 2.0f / (right - left);
        GLfloat b = 2.0f / (top - bottom);
        GLfloat c = -2.0f / (far - near);

        GLfloat tx = - (right + left)/(right - left);
        GLfloat ty = - (top + bottom)/(top - bottom);
        GLfloat tz = - (far + near)/(far - near);

        GLfloat ortho[16] = {
                a, 0, 0, 0,
                0, b, 0, 0,
                0, 0, c, 0,
                tx, ty, tz, 1
        };

        glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, &ortho[0]);
    }

}
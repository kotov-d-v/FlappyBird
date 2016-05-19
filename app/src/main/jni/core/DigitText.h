//
// Created by Dmitry Kotov on 3/31/16.
//

#ifndef FLAPPYBIRD_DIGITTEXT_H
#define FLAPPYBIRD_DIGITTEXT_H

#include "Widget.h"

class DigitText : public Widget
{
public:
    DigitText(float height, int number = 0, Widget* parent = nullptr);

    ~DigitText();

    virtual void draw(const RenderInfo *ri) override;

    void setNumber(int num);

private:

    virtual void render() override;

    void writeRect(float* buf, const math::Rect& rc, int stride);

    int _number;

    int _Ndigits;
    
    GLuint _vbufId;
};


#endif //FLAPPYBIRD_DIGITTEXT_H

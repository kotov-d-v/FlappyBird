//
// Created by Dmitry Kotov on 4/2/16.
//

#ifndef FLAPPYBIRD_LINETEXT_H
#define FLAPPYBIRD_LINETEXT_H

#include "Widget.h"

#define LINE_TEXT_LINES 6  // imply 6-line text texture

class LineText : public Widget
{
public:
    LineText(float height, int textId, Widget* parent = nullptr);

    virtual void draw(const RenderInfo *ri) override;

private:

    virtual void render() override;

    int _line;

    float _vbuf[8], _abuf[8]; // simple rectangle
};


#endif //FLAPPYBIRD_LINETEXT_H

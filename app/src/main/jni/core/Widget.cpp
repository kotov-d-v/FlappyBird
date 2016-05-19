//
//  Widget.cpp
//  FlappyBird
//
//  Created by Dmitry Kotov on 4/20/16.
//  Copyright © 2016 Dmitry Kotov. All rights reserved.
//

#include "Widget.h"

Widget::Widget(Widget* parent) :
    _parent(parent)
{
}

Widget::~Widget()
{
    for(Widget* w : _children)
    {
        delete w;
    }
}

void Widget::draw(const RenderInfo *ri)
{
    for(Widget* w : _children)
    {
        w->draw(ri);
    }
}

void Widget::render()
{
    for(Widget* w : _children)
    {
        w->render();
    }
}

void Widget::moveTo(float x, float y)
{
    _rect.moveTo(x, y);
}

void Widget::moveCenterTo(float cx, float cy)
{
    _rect.moveTo(cx - _rect.width()*0.5f, cy - _rect.height()*0.5f);
}

math::Point Widget::absPosition()
{
    math::Point pos = math::Point(_rect.x0, _rect.y0);
    if(_parent) pos += _parent->absPosition();
    return pos;
}

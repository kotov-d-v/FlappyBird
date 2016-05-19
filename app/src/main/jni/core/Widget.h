//
//  Widget.hpp
//  FlappyBird
//
//  Created by Dmitry Kotov on 4/20/16.
//  Copyright © 2016 Dmitry Kotov. All rights reserved.
//

#ifndef Widget_h
#define Widget_h

#include "SceneObject.h"
#include <list>

class Widget : public SceneObject
{
public:
    
    Widget(Widget* parent = nullptr);
    
    virtual ~Widget();
    
    virtual void render();
    
    virtual void draw(const RenderInfo *ri) override;
    
    void moveTo(float x, float y);
    
    void moveCenterTo(float cx, float cy);
    
    math::Point absPosition();
    
protected:
    
    math::Rect _rect;
    
    std::list<Widget*> _children;
    Widget* _parent;
};

#endif /* Widget_h */

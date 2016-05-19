//
//  LabelWidget.cpp
//  FlappyBird
//
//  Created by Dmitry Kotov on 4/20/16.
//  Copyright © 2016 Dmitry Kotov. All rights reserved.
//

#include "LabelWidget.h"
#include "LineText.h"
#include "DigitText.h"
#include "GameEngine.h"

LabelWidget::LabelWidget(float height, int textId, Widget* parent) :
    Widget(parent)
{
    _rect = math::Rect(0, 0, height*LINE_TEXT_LINES, height);
    
    GameEngine* engine = GameEngine::engine();
    
    Widget* w = new LineText(height, textId, this);
    _children.push_back(w);
    
    if(textId == STRING_GAMEOVER || textId == STRING_WIN)
    {
        float lineHeight = height*1.1f;
        height *= 0.8f;
        
        w->moveTo(0, lineHeight);
        
        w = new LineText(height, STRING_SCORE, this);
        _children.push_back(w);
        
        w = new DigitText(height, engine->score(), this);
        w->moveTo(0.5f*_rect.width(), 0);
        _children.push_back(w);

        w = new LineText(height, STRING_RECORD, this);
        w->moveTo(0, -lineHeight);
        _children.push_back(w);

        w = new DigitText(height, engine->maxScore(), this);
        w->moveTo(0.6f*_rect.width(), -lineHeight);
        _children.push_back(w);
    }
}

//
//  INativeEngine.h
//  FlappyBird
//
//  Created by Dmitry Kotov on 4/19/16.
//  Copyright © 2016 Dmitry Kotov. All rights reserved.
//

#ifndef INativeEngine_h
#define INativeEngine_h

#include "global.h"

struct TextureData
{
    GLsizei width, height;
    GLvoid* pixels;
};

class INativeEngine
{
public:
    INativeEngine() {}
    
    // Load image with the given name to texture data
    virtual bool getTextureData(const char* name, TextureData* data) = 0;
    
    // Clean up texture loader after using it
    virtual void cleanUpTextureLoader() = 0;
    
    // Save game state
    virtual void saveState(const Dictionary&) = 0;
    
    // Load game data
    virtual void loadState(Dictionary&) = 0;
};

#endif /* INativeEngine_h */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "INativeEngine.h"
#include "SceneObject.h"
#include "PoleSet.h"

#include <list>
#include <vector>

class FlappyBird;
class PoleSet;
class DigitText;
class LabelWidget;

struct RenderInfo
{
// shaders
    GLuint vShader;
    GLuint fShader;
    GLuint shaderProgram;

// positions
    GLint positionHandle;
    GLint textureCoordHandle;

// uniform point data
    GLint colorHandle;
    GLint textureHandle;

// matrix
    GLint projMatrixHandle;
    GLint modelMatrixHandle;
    GLint texMatrixHandle;

    RenderInfo() : vShader(0), fShader(0), shaderProgram(0),
                   positionHandle(0), textureCoordHandle(0),
                   colorHandle(0), textureHandle(0),
                   projMatrixHandle(0), modelMatrixHandle(0), texMatrixHandle(0) {}
};

enum TextureResourse
{
    TEX_DUMMY = 0,
    TEX_BRICK,
    TEX_POLE,
    TEX_BIRD,
    TEX_DIGITS,
    TEX_TEXT,
    TEX_LAST = TEX_TEXT
};

enum StringResource
{
    STRING_GAMEOVER,
    STRING_WIN,
    STRING_PAUSE,
    STRING_SCORE,
    STRING_RECORD
};

enum GameStatus
{
    GAME_ACTIVE = 1,
    GAME_PAUSE,
    GAME_OVER,
    GAME_WIN
};

class GameEngine
{
public:

    static GameEngine* engine() {return _engine;};
    static GameEngine* createEngine(INativeEngine*);
    static void destroyEngine();

    static float cameraVelocity() {return 0.2f;}
    static float gravity() {return 1.f;}
    static float bottom() {return 0.1f;}
    static float top() {return 1.f;}

    void restart();

    void start();

    void stop();

    void pause(int txtId = STRING_PAUSE);
    
    void gameOver();

    void resume();

    void draw();

    void onTap();

    void updateFrame(float dt);

    void resize(int w, int h);

    float width() const {return _aspectRatio;} // engine width = aspect_ratio = w/h

    float height() const {return 1.0f;} // engine height = 1 always

    float cameraPosition() {return _cameraPos;}

    float collisionTest(float dt);

    void updateScore();

    GLuint getTextureID(int i) {return _textureIDs[i];}

    void addObject(SceneObject*);

    void deleteObject(SceneObject*);

    GameStatus gameStatus();
    
    void save();
    
    bool load();
    
    int score() {return _score;}
    
    int maxScore() {return _maxScore;}
    
private:
    GameEngine(INativeEngine*);
    ~GameEngine();

    void initShaders();

    void loadTextures();

    void loadTexture(const char *name, int id);

    static GameEngine* _engine;

    INativeEngine* _nativeEngine;

    float _aspectRatio;

    bool _pause;

    RenderInfo _renderInfo;

    std::list<SceneObject*> _sceneObjects; // list of all scene objects
    std::list<SceneObject*> _objToDelete;  // list of objects to delete at updateFrame()
    std::vector<GLuint> _textureIDs;       // set of texture IDs

    FlappyBird* _bird;
    PoleSet* _poles;
    DigitText* _scoreLabel;
    LabelWidget* _infoText; // information about the game (pause, game over, etc)
    int _nextPoleId;     // id of the next pole to pass
    int _score;          // game score
    int _maxScore;       // max user score
    float _cameraPos;    // position of the camera relative to the level begin (moves with the screen)
    float _resumeDelay;  // physical time before tap activates the game
};

#endif // GAMEENGINE_H

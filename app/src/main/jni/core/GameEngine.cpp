#include "GameEngine.h"
#include "FlappyBird.h"
#include "Terrain.h"
#include "DigitText.h"
#include "LabelWidget.h"
#include "util.h"

#define FONTSZ 0.12f // font size relative to the screen height
#define NPOLES 100    //number of poles, later can be moved to level settings
#define LEVELID 1

GameEngine* GameEngine::_engine = nullptr;

GameEngine::GameEngine(INativeEngine* ne) :
    _nativeEngine(ne),
    _aspectRatio(1.f),
    _pause(true),
    _resumeDelay(0),
    _bird(nullptr),
    _poles(nullptr),
    _infoText(nullptr),
    _scoreLabel(nullptr),
    _score(0),
    _maxScore(0)
{
    initShaders();
    loadTextures();

    glClearColor(0.95f, 0.95f, 1, 1);   // light blue
    glDisable(GL_DITHER);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);                // later will enable it only for some objects
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
}

// create game objects
void GameEngine::start()
{
    stop();

    _bird = new FlappyBird();
    _poles = new PoleSet(NPOLES, LEVELID);
    _scoreLabel = new DigitText(FONTSZ);

    addObject(new Terrain());
    addObject(_poles);
    addObject(_bird);
    addObject(_scoreLabel);

    if(load())
    {
        pause();
    }
    else
    {
        restart();
    }
}

void GameEngine::save()
{
    Dictionary d;
    
    _maxScore = math::max(_maxScore, _score);

    d["level"] = util::encode(1); // for future use
    d["score"] = util::encode(_score);
    d["nextpole"] = util::encode(_nextPoleId);
    d["record"] = util::encode(_maxScore);
    d["cameraPos"] = util::encode(_cameraPos);
    d["gameStatus"] = util::encode(gameStatus());

    for(auto i: _sceneObjects)
    {
        i->save(d);
    }

    _nativeEngine->saveState(d);
}

bool GameEngine::load()
{
    Dictionary d;
    _nativeEngine->loadState(d);

    _maxScore = util::decodeI(d["record"]);

    int status = util::decodeI(d["gameStatus"]); // 0 by default

    if(status != GAME_ACTIVE && status != GAME_PAUSE)
        return false;

    _cameraPos = util::decodeF(d["cameraPos"]);
    _nextPoleId = util::decodeI(d["nextpole"]);
    _score = util::decodeI(d["score"]);
    
    for(auto i: _sceneObjects)
    {
        i->load(d);
    }

    updateScore();

    return true;
}

GameEngine::~GameEngine()
{
    save();

    stop();

    glDeleteProgram(_renderInfo.shaderProgram);
    glDeleteShader(_renderInfo.vShader);
    glDeleteShader(_renderInfo.fShader);
    glDeleteTextures(static_cast<int>( _textureIDs.size() ), &_textureIDs[0]);
}

//destroy game objects
void GameEngine::stop()
{
    for(auto i: _sceneObjects)
    {
        delete i;
    }

    _sceneObjects.clear();
    _objToDelete.clear();
}

void GameEngine::loadTextures()
{
#define BRICK1 {0xFF, 0x55, 0}
#define BRICK2 {0xFF, 0x88, 0}

#define POLE1 {0x88, 0x88, 0x88}
#define POLE2 {0xCC, 0xCC, 0xCC}

    GLubyte dummy[] = {0xFF, 0xFF, 0xFF};

    GLubyte brick[][3] = {
            BRICK1, BRICK2,
            BRICK2, BRICK1
    };

    GLubyte pole[][3] = {
            POLE1, POLE2, POLE2, POLE1,
            POLE1, POLE2, POLE2, POLE1,
            POLE1, POLE2, POLE2, POLE1,
            POLE1, POLE2, POLE2, POLE1
    };
#undef BRICK1
#undef BRICK2
#undef POLE1
#undef POLE2
    
    _textureIDs.resize(TEX_LAST+1);
    _textureIDs[TEX_DUMMY] = egl::createRGBTexture(1, 1, GL_NEAREST, dummy);
    _textureIDs[TEX_BRICK] = egl::createRGBTexture(2, 2, GL_NEAREST, brick[0]);
    _textureIDs[TEX_POLE]  = egl::createRGBTexture(4, 4, GL_LINEAR, pole[0]);

    loadTexture("bird", TEX_BIRD);
    loadTexture("digits", TEX_DIGITS);
    loadTexture("text", TEX_TEXT);

    _nativeEngine->cleanUpTextureLoader();
}

void GameEngine::loadTexture(const char *name, int id)
{
    TextureData td;

    if(!_nativeEngine->getTextureData(name, &td))
        return;

    GLuint glId = egl::loadTexture(td.width, td.height, GL_RGBA, td.pixels);

    if(id < _textureIDs.size() ) // check just in case
        _textureIDs.resize(id+1);

    _textureIDs[id] = glId;
}

void GameEngine::initShaders()
{
    const char vertexShaderCode[] =
        "attribute vec4 vPosition;"
        "attribute vec2 aTexCoordinate;"
        "varying vec2 vTexCoordinate;"
        "uniform mat4 uProjMatrix;"
        "uniform mat4 uTexMatrix;"
        "uniform mat4 uModelMatrix;"
        "void main() {"
        "  gl_Position = uProjMatrix*uModelMatrix*vPosition;"
        "  vTexCoordinate = (uTexMatrix * vec4(aTexCoordinate, 0.0, 1.0)).xy;"
        "}";

    const char fragmentShaderCode[] =
        "precision mediump float;"
        "uniform vec4 vColor;"
        "uniform sampler2D uTexture;"
        "varying vec2 vTexCoordinate; "
        "void main(void) {"
        "   gl_FragColor = vColor*texture2D(uTexture, vTexCoordinate);"
        "}";

    _renderInfo.vShader = egl::loadShader(GL_VERTEX_SHADER, vertexShaderCode, sizeof(vertexShaderCode));
    _renderInfo.fShader = egl::loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode, sizeof(fragmentShaderCode));

    _renderInfo.shaderProgram = glCreateProgram();

    glAttachShader(_renderInfo.shaderProgram, _renderInfo.vShader);
    glAttachShader(_renderInfo.shaderProgram, _renderInfo.fShader);
    glLinkProgram(_renderInfo.shaderProgram);

    glUseProgram(_renderInfo.shaderProgram);

    _renderInfo.positionHandle = glGetAttribLocation(_renderInfo.shaderProgram, "vPosition");
    _renderInfo.projMatrixHandle = glGetUniformLocation(_renderInfo.shaderProgram, "uProjMatrix");
    _renderInfo.modelMatrixHandle = glGetUniformLocation(_renderInfo.shaderProgram, "uModelMatrix");
    _renderInfo.colorHandle = glGetUniformLocation(_renderInfo.shaderProgram, "vColor");
    _renderInfo.textureHandle = glGetUniformLocation(_renderInfo.shaderProgram, "uTexture");
    _renderInfo.textureCoordHandle = glGetAttribLocation(_renderInfo.shaderProgram, "aTexCoordinate");
    _renderInfo.texMatrixHandle =  glGetUniformLocation(_renderInfo.shaderProgram, "uTexMatrix");

    glEnableVertexAttribArray(_renderInfo.positionHandle); // used by all objects
}

GameEngine* GameEngine::createEngine(INativeEngine* ne)
{
    if(!_engine) _engine = new GameEngine(ne);
    return _engine;
}

void GameEngine::destroyEngine()
{
    delete _engine;
    _engine = nullptr;
}

void GameEngine::pause(int textId)
{
    _pause = true;

    if(!_infoText)
    {
        _infoText = new LabelWidget(FONTSZ, textId);
        _infoText->moveCenterTo(width() * 0.5f, 0.55f);
        addObject(_infoText);
    }

// wait for half a sec before allow to restart on tap
// (this is to avoid immediate restart on double tap)
    if(gameStatus() == GAME_OVER)
        _resumeDelay = 0.5f;
}

void GameEngine::gameOver()
{
    pause(STRING_GAMEOVER);
    save();
}

void GameEngine::resume()
{
    if(_resumeDelay > 0) return;

    _pause = false;

    if(_infoText)
    {
        deleteObject(_infoText);
        _infoText = nullptr;
    }

    GameStatus status = gameStatus();
    if(status == GAME_WIN || status == GAME_OVER)
        restart();
}

void GameEngine::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    for(auto i: _sceneObjects)
    {
        i->draw(&_renderInfo);
    }

    glFlush();
}

void GameEngine::onTap()
{
    if(_pause)
        resume();
    else
        _bird->flap();
}

void GameEngine::updateFrame(float dt)
{
    if(_pause)
    {
        if(_resumeDelay > 0)
            _resumeDelay -=dt; // do not care if _resumeDelay < 0 after this

        return;
    }

// collision test
    bool hit = false;
    float tc = collisionTest(dt);
    if(tc >= 0.f && tc <= dt)
    {
        dt = tc;
        hit = true;
    }

// move camera
    _cameraPos += cameraVelocity()*dt;

// move objects
    for(auto i: _sceneObjects)
    {
        i->updateFrame(dt);
    }

// check bird's life
    if(hit)
    {
        _bird->die();
    }

// scheduled objects deletion
    for(auto i: _objToDelete)
    {
        _sceneObjects.remove(i);
        delete i;
    }

    _objToDelete.clear();
}

void GameEngine::resize(int w, int h)
{
// engine height = 1 always
// engine width = aspect_ratio = w/h

    _aspectRatio = w/float(h);

    glViewport(0, 0, w, h);
    egl::ortho(_renderInfo.projMatrixHandle, 0, _aspectRatio, 0, 1, 0, 1);

// move labels that are bound to right side of the screen
    _scoreLabel->moveTo(width() - 0.27f, 0.85f);

    if(_infoText)
        _infoText->moveCenterTo(width()*0.5f, 0.55f);

// signal to all objects
    for(auto i: _sceneObjects)
    {
        i->onResize();
    }
}

// return the collision time at dt interval (-1 if not collisions)
float GameEngine::collisionTest(float dt)
{
    math::Circle cr = _bird->bounds();           // bounds of the bird
    math::Vector2D vel = _bird->velocity();      // initial velocity
    math::Vector2D acc = _bird->acceleration();  // current acceleration

// to avoid solving 4th order equations, do approximation
    vel += acc*dt*0.5f;         // average velocity = velocity at [time = t + dt/2]
    vel.x += cameraVelocity();  // absolute velocity of the bird (relative to the poles)

    math::Rect bbox(cr); // bounding box of the bird before move
    bbox.add( math::Circle(cr.pt + vel*dt, cr.r) ); // add bounding box of the bird after move

    math::Rect rc[2];
    for(int i = _nextPoleId; i < _poles->endIndex(); ++i)
    {
        _poles->getTopRect(i, rc);
        _poles->getBottomRect(i, rc+1);

        if(rc->x1 < bbox.x0)
        {// passed the pole
            _nextPoleId++;
            updateScore();
            continue;
        }

        if(rc->x0 > bbox.x1)
            break; // will not collide on this time step

        for(int j = 0; j < 2; ++j) // for top and bottom rect
        {
            if(!bbox.intersects(rc[j]))
                continue;

            float t = math::collisionTime(cr, vel, rc[j]);

            if(t >= 0.f && t <= dt)
                return t; // assume it cannot collide with more than one pole at reasonable dt
        }
    }

    return -1.f;
}

// restart the game
void GameEngine::restart()
{
    _nextPoleId = 0;
    _cameraPos = 0;

    updateScore();

    for(auto i: _sceneObjects)
    {
        i->restart();
    }
}

void GameEngine::updateScore()
{
    _score = _nextPoleId;
    _scoreLabel->setNumber(_score);

    if(gameStatus() == GAME_WIN)
        pause(STRING_WIN);
}

void GameEngine::deleteObject(SceneObject *object)
{
    _objToDelete.push_back(object);
}

void GameEngine::addObject(SceneObject *object)
{
    _sceneObjects.push_back(object);
}

GameStatus GameEngine::gameStatus()
{
    if(!_bird->alive()) return GAME_OVER;

    if(_nextPoleId >= _poles->count()) return GAME_WIN;

    if(_pause) return GAME_PAUSE;

    return GAME_ACTIVE;
}

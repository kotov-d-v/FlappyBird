//
// Created by Dmitry Kotov on 3/23/16.
//

#ifndef FLAPPYBIRD_ANDROIDENGINE_H
#define FLAPPYBIRD_ANDROIDENGINE_H

#include <thread>
#include <mutex>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "GameEngine.h"

class ANativeWindow;

class AndroidEngine : public INativeEngine {
public:
    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT,
        MSG_PAUSE,
        MSG_TAP
    };

    static AndroidEngine* createEngine();

    static void destroyEngine();

    static AndroidEngine* engine() {return _engine;}

    static ANativeWindow* window();

    static void sendMessage(RenderThreadMessage msg, void* param = nullptr);

// INativeEngine interface
    virtual bool getTextureData(const char* name, TextureData* data) override;

    virtual void cleanUpTextureLoader() override;

    virtual void saveState(const Dictionary&) override;

    virtual void loadState(Dictionary&) override;

private:

    static void threadStartCallback();

    static int64_t getTimeNsec();

    void mainLoop();
    bool initialize();
    void destroy();

    static AndroidEngine* _engine;

    AndroidEngine();
    ~AndroidEngine();

    void drawFrame(float dt);

    ANativeWindow* _window;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;

    int64_t _time;
    std::thread _thread;
    std::mutex _mutex;
    enum RenderThreadMessage _msg;
    void* _msgParam;
};


#endif //FLAPPYBIRD_ANDROIDENGINE_H

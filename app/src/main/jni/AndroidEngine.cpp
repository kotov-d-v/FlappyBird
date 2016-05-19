//
// Created by Dmitry Kotov on 3/23/16.
//

#include "AndroidEngine.h"
#include "native-engine.h"

AndroidEngine* AndroidEngine::_engine = nullptr;

AndroidEngine::AndroidEngine() :
    INativeEngine(),
    _thread(threadStartCallback),
    _window(nullptr),
    _display(0),
    _surface(0),
    _context(0),
    _msg(MSG_NONE)
{
}

AndroidEngine::~AndroidEngine()
{
    sendMessage(MSG_RENDER_LOOP_EXIT);

    _thread.join();

    destroy();
}

AndroidEngine* AndroidEngine::createEngine()
{
    if(!_engine) _engine = new AndroidEngine();

    return _engine;
}

void AndroidEngine::destroyEngine()
{
    delete _engine;
    _engine = nullptr;
}

void AndroidEngine::threadStartCallback()
{
    _engine->mainLoop();
}

void AndroidEngine::drawFrame(float dt)
{
    GameEngine* engine = GameEngine::engine();
    engine->updateFrame(dt);
    engine->draw();
}

int64_t AndroidEngine::getTimeNsec()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (int64_t) now.tv_sec*1000000000LL + now.tv_nsec;
}

void AndroidEngine::mainLoop()
{
    bool renderingEnabled = true;

    while (renderingEnabled) {

        _mutex.lock();

        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
                _window = (ANativeWindow*)_msgParam;
                initialize();
                break;

            case MSG_RENDER_LOOP_EXIT:
                renderingEnabled = false;
                destroy();
                break;

            case MSG_TAP:
                if(GameEngine::engine()) GameEngine::engine()->onTap();
                break;

            case MSG_PAUSE:
                if(GameEngine::engine()) GameEngine::engine()->pause();
                break;

            default:
                break;
        }
        _msg = MSG_NONE;

        _mutex.unlock();

        if (_display != EGL_NO_DISPLAY)
        {
            int64_t t = getTimeNsec();
            float dt = double(t - _time)*1.E-9;
            _time = t;

            drawFrame(dt);
            eglSwapBuffers(_display, _surface);
        }

    }

    return;
}

bool AndroidEngine::initialize()
{
    destroy(); // destroy previous instance of GameEngine if any

    const EGLint attribs1[] = {// with antialiasing
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SAMPLE_BUFFERS, 1,
            EGL_SAMPLES, 4,
            EGL_NONE
    };

    const EGLint attribs2[] = {// no antialiasing
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };

    EGLDisplay display;
    EGLConfig config;
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
        return false;

    if (!eglInitialize(display, 0, 0))
        return false;

    if (!eglChooseConfig(display, attribs1, &config, 1, &numConfigs))
    {
        if (!eglChooseConfig(display, attribs2, &config, 1, &numConfigs))
        {
            destroy();
            return false;
        }
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0)))
    {
        destroy();
        return false;
    }

    static const EGLint contextAttribs[] =
    {
            EGL_CONTEXT_CLIENT_VERSION,	2,
            EGL_NONE
    };

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs)))
    {
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height))
    {
        destroy();
        return false;
    }

    _display = display;
    _surface = surface;
    _context = context;

    GameEngine* engine = GameEngine::createEngine(this);
    engine->start();
    engine->resize(width, height);

    _time = getTimeNsec();

    return true;
}

void AndroidEngine::destroy()
{
    GameEngine::destroyEngine();

    if(_display == EGL_NO_DISPLAY) return;

    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);

    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;

    return;
}

// load texture using GameUtil class (implemented in Java)
bool AndroidEngine::getTextureData(const char *name, TextureData *data)
{
    JNIEnv *env;
    gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    gJavaVM->AttachCurrentThread(&env, NULL);

    jstring jstr = env->NewStringUTF(name);

    jmethodID jLoadTexture = env->GetStaticMethodID(gJGameUtil, "loadTexture",
                                                    "(Ljava/lang/String;)[B");

    jbyteArray jArray = (jbyteArray) env->CallStaticObjectMethod(gJGameUtil,
                                                                 jLoadTexture, jstr);
    if(!jArray)
        return false;

// pixel data from the buffer
    data->pixels = env->GetByteArrayElements(jArray, nullptr);

// size parameters are the last 8 bytes in the byffer
    int32_t* endbuf = (int32_t*) ((char*)data->pixels + env->GetArrayLength(jArray));
    data->width = endbuf[-2];
    data->height = endbuf[-1];

    gJavaVM->DetachCurrentThread();

    return data->pixels != nullptr;
}


void AndroidEngine::cleanUpTextureLoader()
{
    JNIEnv *env;
    gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    gJavaVM->AttachCurrentThread(&env, NULL);

    jmethodID jCleanUp = env->GetStaticMethodID(gJGameUtil, "cleanUp", "()V");

    env->CallStaticVoidMethod(gJGameUtil, jCleanUp);

    gJavaVM->DetachCurrentThread();
}

void AndroidEngine::sendMessage(AndroidEngine::RenderThreadMessage msg, void *param)
{
    if(!_engine) return;

    _engine->_mutex.lock();
    _engine->_msg = msg;
    _engine->_msgParam = param;
    _engine->_mutex.unlock();
}

ANativeWindow *AndroidEngine::window()
{
    if(!_engine) return nullptr;

    return _engine->_window;
}

void AndroidEngine::saveState(const Dictionary &dictionary)
{
    JNIEnv *env;
    gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    gJavaVM->AttachCurrentThread(&env, NULL);

    try
    {
        jmethodID jSaveSettings = env->GetStaticMethodID(gJGameUtil, "saveSettings", "(Ljava/util/Map;)V");
        jmethodID jMapCreate = env->GetMethodID(gJMap, "<init>", "()V");
        jmethodID jMapPut = env->GetMethodID(gJMap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        jobject joMap = env->NewObject(gJMap, jMapCreate);

        for(Dictionary::const_iterator i = dictionary.begin(); i != dictionary.end(); ++i)
        {
            jstring js1 = env->NewStringUTF(i->first.c_str());
            jstring js2 = env->NewStringUTF(i->second.c_str());
            env->CallObjectMethod(joMap, jMapPut, js1, js2);
        }

        env->CallStaticVoidMethod(gJGameUtil, jSaveSettings, joMap);
    }
    catch(...)
    {

    }

    gJavaVM->DetachCurrentThread();
}

// helper function to load string from java String
std::string GetJStringContent(JNIEnv *env, jstring jStr)
{
    std::string res;
    if (!jStr) return res;

    const char *s = env->GetStringUTFChars(jStr, nullptr);
    res = s;
    env->ReleaseStringUTFChars(jStr, s);

    return res;
}

void AndroidEngine::loadState(Dictionary &dictionary)
{
    JNIEnv *env;
    gJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    gJavaVM->AttachCurrentThread(&env, NULL);

    try
    {
        jmethodID jsettingKeys = env->GetStaticMethodID(gJGameUtil, "loadSettingKeys",
                                                        "()[Ljava/lang/String;");
        jmethodID jsettingVals = env->GetStaticMethodID(gJGameUtil, "loadSettingValues",
                                                        "()[Ljava/lang/String;");

        jobjectArray jKeys = (jobjectArray) env->CallStaticObjectMethod(gJGameUtil, jsettingKeys);
        jobjectArray jVals = (jobjectArray) env->CallStaticObjectMethod(gJGameUtil, jsettingVals);
        int N1 = env->GetArrayLength(jKeys);
        int N2 = env->GetArrayLength(jVals);
        int N = N1 <= N2 ? N1 : N2;

        for(jsize i = 0; i < N; ++i )
        {
            jstring js1 = (jstring) env->GetObjectArrayElement(jKeys, i);
            jstring js2 = (jstring) env->GetObjectArrayElement(jVals, i);
            dictionary[GetJStringContent(env, js1)] = GetJStringContent(env, js2);
        }
    }
    catch(...)
    {

    }

    gJavaVM->DetachCurrentThread();
}

//
// Created by Dmitry Kotov on 3/23/16.
//

#include "native-engine.h"
#include "AndroidEngine.h"
#include <android/native_window_jni.h>

JavaVM* gJavaVM = NULL;
jclass gJGameUtil = 0;
jclass gJMap = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
        return -1;

    gJavaVM = vm;

    jclass jGameUtil = env->FindClass("com/dmitrykotov/flappybird/GameUtil");
    gJGameUtil = (jclass) env->NewGlobalRef(jGameUtil);

    jclass jMap = env->FindClass("java/util/HashMap");
    gJMap = (jclass) env->NewGlobalRef(jMap);

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_dmitrykotov_flappybird_GameActivity_nativeSetSurface(JNIEnv *env, jclass type,
                                                              jobject surface)
{
    if (surface != 0)
    {
        AndroidEngine::createEngine();

        AndroidEngine::sendMessage(AndroidEngine::MSG_WINDOW_SET,
                            ANativeWindow_fromSurface(env, surface));
    }
    else
    {
        if(AndroidEngine::engine())
            ANativeWindow_release( AndroidEngine::engine()->window() );

        AndroidEngine::destroyEngine();
    }
}

JNIEXPORT void JNICALL
Java_com_dmitrykotov_flappybird_GameActivity_nativeOnTap(JNIEnv *env, jclass type)
{
    AndroidEngine::sendMessage(AndroidEngine::MSG_TAP);
}

JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeOnResume(JNIEnv *env, jclass type)
{

}

JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeOnPause(JNIEnv *env, jclass type)
{
    AndroidEngine::sendMessage(AndroidEngine::MSG_PAUSE);
}

//
// Created by Dmitry Kotov on 3/23/16.
//

#ifndef FLAPPYBIRD_FLAPPY_BIRD_EGL_H
#define FLAPPYBIRD_FLAPPY_BIRD_EGL_H

#include <jni.h>

extern JavaVM* gJavaVM;
extern jclass gJGameUtil;
extern jclass gJMap;

extern "C" {

JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeOnResume(JNIEnv *env, jclass type);

JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeOnPause(JNIEnv *env, jclass type);

JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeSetSurface(JNIEnv *env, jclass type,
                                                                      jobject surface);
JNIEXPORT void JNICALL
        Java_com_dmitrykotov_flappybird_GameActivity_nativeOnTap(JNIEnv *env, jclass type);

};

#endif //FLAPPYBIRD_FLAPPY_BIRD_EGL_H

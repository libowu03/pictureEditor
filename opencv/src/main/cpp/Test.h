//
// Created by Administrator on 2022/6/17.
//

#ifndef CHANGECOLORMASTER_TEST_H
#define CHANGECOLORMASTER_TEST_H

#include <jni.h>
#include <android/asset_manager.h>

extern jobject test(JNIEnv *env, jobject image,jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath);

extern jobjectArray getFacePoint(JNIEnv *env, jobject image,jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath);

#endif //CHANGECOLORMASTER_TEST_H

//
// Created by libowu on 5/30/22.
//

#ifndef CHANGECOLORMASTER_SUPERRES_H
#define CHANGECOLORMASTER_SUPERRES_H


#include <jni.h>

extern jobject startSuperres(JNIEnv *env, jclass clazz, jobject bitmap, jint size,jstring modelPath,jstring modelName);
#endif //CHANGECOLORMASTER_SUPERRES_H

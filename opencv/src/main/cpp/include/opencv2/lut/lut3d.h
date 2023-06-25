//
// Created by libowu on 2022/2/20.
//

#ifndef CHANGECOLORMASTER_LUT3D_H
#define CHANGECOLORMASTER_LUT3D_H

#endif //CHANGECOLORMASTER_LUT3D_H
extern jobject changeByLut(JNIEnv *env, jclass clazz, jobject bitmap, char *lutfile,jfloat strength);
extern jobject changeLutPyPng(JNIEnv *env, jclass clazz, jobject bitmap,jobject lutMap);
extern jobject cubeToPng(JNIEnv *env, jclass clazz, char *lutfile,jobject bitmap);
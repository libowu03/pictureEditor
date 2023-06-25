//
// Created by libowu on 4/23/22.
//

#ifndef CHANGECOLORMASTER_HSBNEW_H
#define CHANGECOLORMASTER_HSBNEW_H

#include <jni.h>
#include <opencv2/hsl/Hsb.h>

extern jobject startChangeS(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jfloat redAdjust, jfloat greenAdjust, jfloat blueAdjust, jfloat cyanAdjust, jfloat yellowAdjust, jfloat carmineAdjust);
extern jobject startChangeH(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,jfloat redAdjust,jfloat greenAdjust,jfloat blueAdjust,jfloat cyanAdjust,jfloat yellowAdjust,jfloat carmineAdjust);
extern jobject startChangeHsb(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,HueChange hueData);
extern jobject testHsb(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,HueChange changeData);
#endif //CHANGECOLORMASTER_HSBNEW_H

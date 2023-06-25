//
// Created by libowu on 6/3/22.
//

#ifndef CHANGECOLORMASTER_MASK_H
#define CHANGECOLORMASTER_MASK_H
#include <string>
#include <jni.h>

extern jobject makeHsbMask(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jint targetR, jint targetG, jint targetB, jfloat allowExtentH, jfloat allowExtentS, jfloat allowExtentB, jboolean isShowModel,jboolean isAddModel);
extern jobject makeThresholdMask(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint threshold,jboolean isShowModel,jboolean isAddModel);
extern jobject imageToMask(JNIEnv *env, jobject bitmap);
extern jobject imageToMaskPro(JNIEnv *env, jobject bitmap,float pro);
extern jobject maskToAlphaMask(JNIEnv *env, jclass clazz, jobject mask);
extern jobject getRGBMask(JNIEnv *env,jobject bitmap,int channelType,int needToOvalValue,bool isUseDefaultIfNotOver);
extern jobject getMaskAdd(JNIEnv *env,jobject maskOne,jobject maskTwo);
extern jobject getMaskCut(JNIEnv *env,jobject maskOne,jobject maskTwo);
extern jobject getPupilMaskByEyeMask(JNIEnv *env,jobject sourceMask,jobject eyeMask);
extern jobject KeepIntersectMask(JNIEnv *env,jobject maskOne,jobject maskTwo);
extern std::vector<int> getMaskRect(JNIEnv *env,jobject bitmap);
#endif //CHANGECOLORMASTER_MASK_H

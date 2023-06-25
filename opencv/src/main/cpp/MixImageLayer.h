//
// Created by libowu on 6/12/22.
//

#ifndef CHANGECOLORMASTER_MIXIMAGELAYER_H
#define CHANGECOLORMASTER_MIXIMAGELAYER_H

#include <jni.h>

//正常叠加
extern jobject normal(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jobject join,jint left,jint top,float addWeight);
//暗角
extern jobject darkCorner(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jboolean isBlack,jfloat strength);
//柔光
extern jobject toSoftLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float topWeight);
//叠加模式
extern jobject toOverlay(JNIEnv *env, jclass clazz,jobject bitmap, jobject mask,jobject join,jint left,jint top,float addWeight);
//强光
extern jobject toHardLight(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jobject join,jint left,jint top,float addWeight);
//叠加
extern jobject toOverlay(JNIEnv *env, jclass clazz,jobject bitmap, jobject mask,jobject join,jint left,jint top,float addWeight);
//亮光
extern jobject toVividLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//线性光
extern jobject toLinearLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//点光
extern jobject toPinLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//实色混合
extern jobject toHardMix(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//差值
extern jobject toDifference(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//排除
extern jobject toExclusion(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//减去
extern jobject toSubtract(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//划分
extern jobject toDivide(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//色相
extern jobject toHue(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//饱和度
extern jobject toSaturation(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//明度
extern jobject toLuminosity(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//颜色
extern jobject toColor(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//滤色
extern jobject toFilterColor(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//颜色加深
extern jobject toColorDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//线性减淡
extern jobject toLineColorLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//线性加深
extern jobject toLineColorDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//颜色减淡
extern jobject toColorLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//正片叠底
extern jobject toMultiply(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//变亮
extern jobject toLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
//变暗
extern jobject toDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,jint left,jint top,float addWeight);
#endif //CHANGECOLORMASTER_MIXIMAGELAYER_H

//
// Created by libowu on 20-8-29.
//

#ifndef OPENCV_OPENCVLINK_H
#define OPENCV_OPENCVLINK_H
extern jobject makeBitmapToPixels(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint pixels_size);
extern jobject makeBitmapColorCounter(JNIEnv *env, jclass clazz, jobject bitmap);
extern jobject makeBitmapGray (JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapWhiteAndBlack(JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapNostalgia(JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapFounding(JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapFrozen(JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapComics(JNIEnv *env, jclass obj, jobject bitmap);
extern jobject makeBitmapBlur(JNIEnv *env, jclass obj, jobject bitmap, jobject mask,int blurSize);
extern jobject makeBitmapErode(JNIEnv *env, jclass clazz, jobject bitmap);
extern jobject makeBitmapEdge(JNIEnv *env, jclass clazz, jobject bitmap);
extern jobject makeBitmapDilate(JNIEnv *env, jclass clazz, jobject bitmap);
extern jobject makeBitmapRemoveHaze(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,float value);
#endif //OPENCV_OPENCVLINK_H

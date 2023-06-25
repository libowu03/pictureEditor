//
// Created by libowu on 2021/10/4.
//
#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

#ifndef CHANGECOLORMASTER_HSB_H
#define CHANGECOLORMASTER_HSB_H

#endif //CHANGECOLORMASTER_HSB_H

class HueChange
{
public:
    float rgbH = 0;
    float rgbS = 0;
    float rgbB = 0;

    float rH = 0;
    float rS = 0;
    float rB = 0;

    float gH = 0;
    float gS = 0;
    float gB = 0;

    float bH = 0;
    float bS = 0;
    float bB = 0;

    //青色
    float cH = 0;
    float cS = 0;
    float cB = 0;

    //洋红
    float mH = 0;
    float mS = 0;
    float mB = 0;

    //橙色
    float oH = 0;
    float oS = 0;
    float oB = 0;

    //紫色
    float pH = 0;
    float pS = 0;
    float pB = 0;


    //黄色
    float yH = 0;
    float yS = 0;
    float yB = 0;

    //用户选中的颜色限制
    int rChoose = -1;
    int gChoose = -1;
    int bChoose = -1;
    int leftChoose = -1;
    int rightChoose = -1;
    int minChoose = -1;
    int maxChoose = -1;
};

extern jobject changeHsb(JNIEnv *env, jclass clazz, jobject bitmap, HueChange hueData);
extern jobject ChangeHsbB(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask, HueChange hueData);
extern jobject ChangeHsbS(JNIEnv *env, jclass clazz, jobject bitmap, HueChange hueData);
extern jobject changeSaturation(JNIEnv *env, jclass clazz, jobject bitmap, int saturationValue);
extern jobject changeSaturationTwo(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask, jfloat saturationValue);
extern jobject ChangeHsbH(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, HueChange hueData);
extern void changeB(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange);
extern void changeBTwo(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange);
extern jobject ReplaceHAndS(int targetH,int targetS,int targetB,JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jfloat saturationValue,jfloat pro_s,jfloat pro_b);
extern jint GetHsbAverageValue(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask);

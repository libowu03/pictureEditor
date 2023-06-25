//
// Created by libowu on 2021/10/22.
//

#ifndef CHANGECOLORMASTER_COLORBALANCE_H
#define CHANGECOLORMASTER_COLORBALANCE_H

class ColorBalance
{
public:
    int cyanAndRedLow = 0;
    int carmineAndGreenLow = 0;
    int yellowAndBlueLow = 0;

    int cyanAndRedMiddle = 0;
    int carmineAndGreenMiddle = 0;
    int yellowAndBlueMiddle = 0;

    int cyanAndRedHight = 0;
    int carmineAndGreenHight = 0;
    int yellowAndBlueHight = 0;

    //保持明度
    bool isStayLight = true;
};

extern jobject colorBalance(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, ColorBalance data);
extern jobject autoColorBalance(JNIEnv *env, jclass clazz, jobject bitmap);

#endif //CHANGECOLORMASTER_COLORBALANCE_H

//
// Created by libowu on 2021/2/13.
//
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

#ifndef IMAGEEDIT_BEAN_H
#define IMAGEEDIT_BEAN_H

/**
 * 3dLut基础信息
 */
class Lut3D{
public:
    vector<vector<float>> colorValue;
    int size;
    float minRColor;
    float maxRColor;
    float minGColor;
    float maxGColor;
    float minBColor;
    float maxBColor;
};

/**
 * 直方图
 */
class Histogram
{
public:
    jint *redChannel;
    jint *greenChannel;
    jint *blurChannel;
    jint *rgbChannel;
    jint arraySize;
};
#endif //IMAGEEDIT_BEAN_H

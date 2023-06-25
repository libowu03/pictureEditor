#include <opencv2/opencv.hpp>
#include <vector>
//视频相关的引用
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <map>
#include <jni.h>
#include <android/bitmap.h>
#include <opencv2/utils/utils.h>
#include "include/opencv2/utils/Commen.h"
#include "include/opencv2/hsl/ColorSpace.hpp"
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
using namespace std;
using namespace cv;

/**
 *
 * @param env
 * @param clazz
 * @param bitmap
 * @param darkLightProgress 阴影进度[-1,1]
 * @param hightLightProgress 高光进度[-1,1]
 * @return
 */
jobject changeHighLigAndDarkLight(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,float darkLightProgress,float hightLightProgress){
    if(darkLightProgress < -1){
        darkLightProgress = -1;
    }else if (darkLightProgress > 1){
        darkLightProgress = 1;
    }

    if(hightLightProgress < -1){
        hightLightProgress = -1;
    }else if (hightLightProgress > 1){
        hightLightProgress = 1;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img,img,COLOR_RGBA2RGB);
    cv::Mat result = img.clone();
    cvtColor(result,result,COLOR_RGBA2RGB);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,COLOR_RGBA2RGB);


    int width = img.cols;
    int height = img.rows;
    //最小亮度
    float minLight = 1;
    //最大亮度
    float maxLight = 0;
    cvtColor(img,img,COLOR_RGB2HSV_FULL);
    cvtColor(maskMt,maskMt,COLOR_RGB2HSV_FULL);
    cvtColor(result,result,COLOR_RGB2HSV_FULL);
    for (int i = 0; i < height; i++)
    {
        uchar* in = img.data + i*img.step;
        //遍历获取最大亮度和最小亮度
        for (int j = 0; j < width; j++)
        {
            if ((in[2]/255.0) > maxLight)
            {
                maxLight = in[2]/255.0;
            }
            if (in[2]/255.0 < minLight)
            {
                minLight = in[2]/255.0;
            }
            in = in+3;
        }
    }

    uchar pH[256];
    for (int i = 0; i < 256; i++)
    {
        pH[i] = i;
    }
    Mat lutH(1, 256, CV_8UC1, pH);

    uchar pS[256];
    for (int i = 0; i < 256; i++)
    {
        pS[i] = i;
    }
    Mat lutS(1, 256, CV_8UC1, pS);

    uchar pB[256];
    for (int i = 0; i < 256; i++)
    {
        float hsb[3] = {0.0,0.0,i/255.0f};
        uchar max = i;
        double black = minLight * 255 + ((maxLight * 255 - minLight * 255) / 3) / 1.5;
        float maxChangeRange = abs( maxLight - black/255.0 )/2.0;
        double numberOne = (1 - hsb[2]) * darkLightProgress * maxChangeRange;

        if (numberOne < 0)
        {
            //修改之后的亮度
            if (max > black)
            {
                hsb[2] = hsb[2] - pow(abs(numberOne), exp((max - black) / (maxLight * 255 - black)));
            }
            else
            {
                hsb[2] = hsb[2] - pow(abs(numberOne), exp((abs((max - black))) / (abs((minLight * 255 - black) * 1.0f))));
            }
        }
        else
        {
            if (max > black)
            {
                hsb[2] = hsb[2] + pow(numberOne, exp((max - black) / (maxLight * 255 - black)));
            }
            else
            {
                hsb[2] = hsb[2] + pow(numberOne, exp((abs((max - black))) / (abs((minLight * 255 - black) * 1.0f))));
            }
        }

        if (hsb[2] > 1)
        {
            hsb[2] = 1;
        }
        else if (hsb[2] < 0)
        {
            hsb[2] = 0;
        }

        double white = ((maxLight * 255 - minLight * 255) / 3);
        maxChangeRange = abs( maxLight - white/255.0 )/1.5;
        double numberTwo = (hsb[2]) * hightLightProgress * maxChangeRange;
        if (numberTwo < 0)
        {
            if (max >= 230){
                max = max - ((max - 230)/25.0);
            }
            if (max > white)
            {
                hsb[2] = hsb[2] - pow(abs(numberTwo), exp((max - white) / (maxLight * 255 - white)));
            }
            else
            {
                hsb[2] = hsb[2] - pow(abs(numberTwo), exp((abs((max - white))) / (abs((minLight * 255 - white) * 1.0f))));
            }
        }
        else
        {
            if (max > white)
            {
                hsb[2] = hsb[2] + pow(numberTwo, exp((max - white) / (maxLight * 255 - white)));
            }
            else
            {
                hsb[2] = hsb[2] + pow(numberTwo, exp((abs((max - white))) / (abs((minLight * 255 - white) * 1.0f))));
            }
        }

        if (hsb[2] > 1)
        {
            hsb[2] = 1;
        }
        else if (hsb[2] < 0)
        {
            hsb[2] = 0;
        }
        pB[i] = hsb[2]*255;
    }
    Mat lutB(1, 256, CV_8UC1, pB);
    vector<Mat> mergeMats;
    mergeMats.push_back(lutH);
    mergeMats.push_back(lutS);
    mergeMats.push_back(lutB);
    Mat lutResult;
    merge(mergeMats, lutResult);
    LUT(result,lutResult,result);



    for (int i = 0; i < height; i++)
    {
        uchar* in = img.data + i*img.step;
        uchar* out = result.data + i*result.step;
        uchar* maskIn = maskMt.data + i*maskMt.step;

        for (int j = 0; j < width; j++)
        {
            uchar r = in[2];
            uchar g = in[1];
            uchar b = in[0];
            if (maskIn[2] == 0)
            {
                out[2] = r;
                out[1] = g;
                out[0] = b;
                in += 3;
                maskIn += 3;
                out += 3;
                continue;
            }
            out[2] = r * (1 - maskIn[2] / 255.0) + out[2]*(maskIn[2] / 255.0);
            out[1] = g;
            out[0] = b;
            in += 3;
            maskIn += 3;
            out += 3;
        }
    }
    cvtColor(result,result,COLOR_HSV2RGB_FULL);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    img.release();
    maskMt.release();
    result.release();
    return resultBitmap;
}


//
// Created by libowu on 20-8-29.
// 中间调部分算法还存在一些问题，部分像素色值和photoshop存在一定差距，后面需要优化
//
#include <opencv2/opencv.hpp>
#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include "include/opencv2/filter/filter.h"
#include "include/opencv2/utils/utils.h"
#include <opencv2/features2d.hpp>
#include <opencv2/color/ColorBalance.h>
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
void changeColorBalance(ColorBalance data, uchar& resultRed, uchar& resultGreen, uchar& resultBlue);

jobject colorBalance(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, ColorBalance data) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2RGB);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,CV_RGBA2RGB);

    int height = image.rows;
    int widht = image.cols;
    Mat result(image.size(), image.type());
    cvtColor(result,result,CV_RGBA2RGB);

    for (int h = 0; h < height; h++) {
        const uchar *in = image.data + h * image.step;
        uchar *out = result.data + h * result.step;
        const uchar *maskIn = maskMt.data + h * maskMt.step;

        for (int w = 0; w < widht; w++) {
            uchar redValue = in[0];
            uchar greenValue = in[1];
            uchar blueValue = in[2];
            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                out[2] = blueValue;
                out[1] = greenValue;
                out[0] = redValue;
                out = out +3 ;
                maskIn = maskIn +3 ;
                in = in +3 ;
                continue;
            }
            uchar resultRedValue = redValue;
            uchar resultGreenValue = greenValue;
            uchar resultBlueValue = blueValue;
            changeColorBalance(data, resultRedValue, resultGreenValue, resultBlueValue);
            out[2] = blueValue * (1 - maxMask / 255.0) + resultBlueValue * maxMask / 255.0;
            out[1] = greenValue * (1 - maxMask / 255.0) + resultGreenValue * maxMask / 255.0;
            out[0] = redValue * (1 - maxMask / 255.0) + resultRedValue * maxMask / 255.0;
            out = out +3 ;
            maskIn = maskIn +3 ;
            in = in +3 ;
        }
    }

    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 * 自动白平衡
 * @param env
 * @param clazz
 * @param bitmap
 * @return
 */
jobject autoColorBalance(JNIEnv *env, jclass clazz, jobject bitmap){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    cvtColor(src,src,CV_RGBA2RGB);

    int row = src.rows;
    int col = src.cols;
    //Mat dst(row, col, CV_8UC3);
    Mat dst = src.clone();
    //保存像素通道和的数组
    int HistRGB[767] = { 0 };
    //像素通道和的最大值
    int MaxVal = 0;
    //计算像素通道和，并保存到数组中，同时获取像素通道和的最大值
    for (int i = 0; i < row; i++) {
        const uchar *in = src.data + i * src.step;
        for (int j = 0; j < col; j++) {
            MaxVal = max(MaxVal, (int)in[0]);
            MaxVal = max(MaxVal, (int)in[1]);
            MaxVal = max(MaxVal, (int)in[2]);
            int sum = in[0] + in[1] + in[2];
            HistRGB[sum]++;
            in = in + 3;
        }
    }
    int Threshold = 0;
    int sum = 0;
    for (int i = 766; i >= 0; i--) {
        sum += HistRGB[i];
        if (sum > row * col * 0.1) {
            Threshold = i;
            break;
        }
    }
    int AvgB = 0;
    int AvgG = 0;
    int AvgR = 0;
    int cnt = 0;
    for (int i = 0; i < row; i++) {
        const uchar *in = src.data + i * src.step;
        for (int j = 0; j < col; j++) {
            int sumP = in[0] + in[1] + in[2];
            if (sumP > Threshold) {
                AvgB += in[0];
                AvgG += in[1];
                AvgR += in[2];
                cnt++;
            }
            in = in + 3;
        }
    }
    AvgB /= cnt;
    AvgG /= cnt;
    AvgR /= cnt;
    for (int i = 0; i < row; i++) {
        const uchar *in = src.data + i * src.step;
        uchar *out = dst.data + i * dst.step;
        for (int j = 0; j < col; j++) {
            int Blue = in[0] * MaxVal / AvgB;
            int Green = in[1] * MaxVal / AvgG;
            int Red = in[2] * MaxVal / AvgR;
            if (Red > 255) {
                Red = 255;
            }
            else if (Red < 0) {
                Red = 0;
            }
            if (Green > 255) {
                Green = 255;
            }
            else if (Green < 0) {
                Green = 0;
            }
            if (Blue > 255) {
                Blue = 255;
            }
            else if (Blue < 0) {
                Blue = 0;
            }
            out[0] = Blue;
            out[1] = Green;
            out[2] = Red;
            in = in + 3;
            out = out + 3;
        }
    }
    jobject resultBitmap = matToBitmap(env, dst, true, bitmap);
    src.release();
    dst.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

inline void changeColorBalance(ColorBalance data, uchar& resultRed, uchar& resultGreen, uchar& resultBlue) {
    int maxInputValue = MAX(data.cyanAndRedLow, MAX(data.carmineAndGreenLow, data.yellowAndBlueLow));

    //===================设置阴影===================
    int tmp = 255 * ((int)resultRed - (maxInputValue - data.cyanAndRedLow)) / (255 - (maxInputValue - data.cyanAndRedLow));
    if (tmp > 255) {
        resultRed = 255;
    }
    else if (tmp < 0) {
        resultRed = 0;
    }
    else {
        resultRed = tmp;
    }

    tmp = 255 * ((int)resultGreen - (maxInputValue - data.carmineAndGreenLow)) / (255 - (maxInputValue - data.carmineAndGreenLow));
    if (tmp > 255) {
        resultGreen = 255;
    }
    else if (tmp < 0) {
        resultGreen = 0;
    }
    else {
        resultGreen = tmp;
    }

    tmp = 255 * ((int)resultBlue - (maxInputValue - data.yellowAndBlueLow)) / (255 - (maxInputValue - data.yellowAndBlueLow));
    if (tmp > 255) {
        resultBlue = 255;
    }
    else if (tmp < 0) {
        resultBlue = 0;
    }
    else {
        resultBlue = tmp;
    }
    //===================设置阴影End===================


    //===================中间调===================
    int maxInputMid = MAX(data.cyanAndRedMiddle, MAX(data.carmineAndGreenMiddle, data.yellowAndBlueMiddle));
    int minInputMid = MIN(data.cyanAndRedMiddle, MIN(data.carmineAndGreenMiddle, data.yellowAndBlueMiddle));
    int midInputMid = data.cyanAndRedMiddle + data.carmineAndGreenMiddle + data.yellowAndBlueMiddle - maxInputMid - minInputMid;
    int tmpCyanAndRedMiddle = data.cyanAndRedMiddle - midInputMid;
    int tmpCarmineAndGreenMiddle = data.carmineAndGreenMiddle - midInputMid;
    int tmpYellowAndBlueMiddle = data.yellowAndBlueMiddle - midInputMid;

    tmp = (pow((int)resultRed / 255.0, exp(-0.0033944 * (tmpCyanAndRedMiddle)))) * 255.0 + 0.5;
    if (tmp > 255) {
        resultRed = 255;
    }
    else if (tmp < 0) {
        resultRed = 0;
    }
    else {
        resultRed = tmp;
    }

    tmp = (pow((int)resultGreen / 255.0, exp(-0.0033944 * (tmpCarmineAndGreenMiddle)))) * 255.0 + 0.5;
    if (tmp > 255) {
        resultGreen = 255;
    }
    else if (tmp < 0) {
        resultGreen = 0;
    }
    else {
        resultGreen = tmp;
    }

    tmp = (pow((int)resultBlue / 255.0, exp(-0.0033944 * (tmpYellowAndBlueMiddle)))) * 255.0 + 0.5;
    if (tmp > 255) {
        resultBlue = 255;
    }
    else if (tmp < 0) {
        resultBlue = 0;
    }
    else {
        resultBlue = tmp;
    }
    //===================中间调End===================

    //===================设置高光===================
    int minInputValueHeight = MIN(data.cyanAndRedHight, MIN(data.carmineAndGreenHight, data.yellowAndBlueHight));
    //求取“青色-红色”
    tmp = 255 * (int)resultRed / (255 - (data.cyanAndRedHight - minInputValueHeight));
    if (tmp > 255) {
        resultRed = 255;
    }
    else if (tmp < 0) {
        resultRed = 0;
    }
    else {
        resultRed = tmp;
    }

    //求取“洋红-绿色”
    tmp = 255 * (int)resultGreen / (255 - (data.carmineAndGreenHight - minInputValueHeight));
    if (tmp > 255) {
        resultGreen = 255;
    }
    else if (tmp < 0) {
        resultGreen = 0;
    }
    else {
        resultGreen = tmp;
    }

    //求取“黄色-蓝色”
    tmp = 255 * (int)resultBlue / (255 - (data.yellowAndBlueHight - minInputValueHeight));
    if (tmp > 255) {
        resultBlue = 255;
    }
    else if (tmp < 0) {
        resultBlue = 0;
    }
    else {
        resultBlue = tmp;
    }
    //===================设置高光End===================

}
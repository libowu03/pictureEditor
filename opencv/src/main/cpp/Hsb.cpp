//
// Created by libowu on 2021/9/3.
//

#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include "include/opencv2/utils/utils.h"
#include "include/opencv2/hsl/ColorSpace.hpp"
#include "opencv2/hsl/Hsb.h"
#include "include/opencv2/utils/Commen.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

void changeColor(uchar &resultRed, uchar &resultGreen, uchar &resultBlue, float sPercentage,
                 float bPercentage, int hue, uchar redValue, uchar greenValue, uchar blueValue,
                 int max, int min, int type, HueChange hueData);
void changeH(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange);
void changeB(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange);
void changeS(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange);
void saturation(unsigned char *srcData, int width, int height, int stride, int saturation);
void AdjustSatuation(cv::Mat &image, float adjust,Mat mask);




/**
 *
 * 将某个颜色映射到其他的目标颜色上
 *
 **/
jobject changeHsb(JNIEnv *env, jclass clazz, jobject bitmap, HueChange hueData) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2RGB);
    int channel = image.channels();
    int widht = channel * image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        const uchar *in = image.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);

        for (int j = 0; j < widht; j = j + channel) {
            uchar max = MAX(in[j], MAX(in[j + 1], in[j + 2]));
            uchar min = MIN(in[j], MIN(in[j + 1], in[j + 2]));
            uchar mid = in[j] + in[j + 1] + in[j + 2] - min - max;
            uchar redValue = in[j + 0];
            uchar greenValue = in[j + 1];
            uchar blueValue = in[j + 2];
            uchar resultRed = redValue;
            uchar resultGreen = greenValue;
            uchar resultBlue = blueValue;
            if (hueData.rgbH != 0 || hueData.rgbS != 0 || hueData.rgbB != 0) {
                //RGB
                changeColor(resultRed, resultGreen, resultBlue, hueData.rgbS, hueData.rgbB,
                            hueData.rgbH, redValue, greenValue, blueValue, max, min, 0,hueData);
            }
            if ((max == redValue || max == greenValue) && min == blueValue &&
                (hueData.yH != 0 || hueData.yS != 0 || hueData.yB != 0)) {
                //黄色通道
                //cout << "y" << endl;
                if (blueValue != redValue && blueValue != greenValue) {
                    changeColor(resultRed, resultGreen, resultBlue, hueData.yS, hueData.yB,
                                hueData.yH, redValue, greenValue, blueValue, max, min, 1,hueData);
                }
            }
            if ((max == redValue && greenValue != redValue && blueValue != redValue &&
                 (hueData.rH != 0 || hueData.rS != 0 || hueData.rB != 0))) {
                //红色通道
                //cout << "r" << endl;
                changeColor(resultRed, resultGreen, resultBlue, hueData.rS, hueData.rB, hueData.rH,
                            redValue, greenValue, blueValue, max, min, 2,hueData);
            }
            if (max == blueValue && (hueData.bH != 0 || hueData.bS != 0 || hueData.bB != 0)) {
                //蓝色通道
                //cout << "b" << endl;
                changeColor(resultRed, resultGreen, resultBlue, hueData.bS, hueData.bB, hueData.bH,
                            redValue, greenValue, blueValue, max, min, 3,hueData);
            }
            if (max == greenValue && (hueData.gH != 0 || hueData.gS != 0 || hueData.gB != 0)) {
                //绿色通道
                //cout << "g" << endl;
                changeColor(resultRed, resultGreen, resultBlue, hueData.gS, hueData.gB, hueData.gH,
                            redValue, greenValue, blueValue, max, min, 4,hueData);
            }
            if ((max == blueValue || max == greenValue) && min == redValue &&
                (hueData.cH != 0 || hueData.cS != 0 || hueData.cB != 0)) {
                //青色
                //cout << "c" << endl;
                if (redValue != blueValue && redValue != greenValue) {
                    changeColor(resultRed, resultGreen, resultBlue, hueData.cS, hueData.cB,
                                hueData.cH, redValue, greenValue, blueValue, max, min, 5,hueData);
                }
            }
            if (min == greenValue && (hueData.mH != 0 || hueData.mS != 0 || hueData.mB != 0)) {
                //洋红
                //cout << "m" << endl;
                if (redValue != greenValue && blueValue != greenValue) {
                    changeColor(resultRed, resultGreen, resultBlue, hueData.mS, hueData.mB,
                                hueData.mH, redValue, greenValue, blueValue, max, min, 6,hueData);
                }
            }
            outResult[j + 0] = resultRed;
            outResult[j + 1] = resultGreen;
            outResult[j + 2] = resultBlue;
            if (channel == 4) {
                outResult[j + 3] = 255;
            }
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

jobject ChangeHsbH(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,HueChange hueData){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cvtColor(image,image,CV_RGBA2RGB);
    cvtColor(maskMt,maskMt,CV_RGBA2RGB);
    int channel = image.channels();
    int widht = channel * image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        uchar *in = image.data + i * image.step;
        uchar *outResult = result.data + i * result.step;
        uchar *maskIn = maskMt.data + i * maskMt.step;
        /*const uchar *in = image.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);*/

        for (int j = 0; j < widht; j = j + channel) {
            uchar max = MAX(in[0], MAX(in[1], in[2]));
            uchar min = MIN(in[0], MIN(in[1], in[2]));
            uchar mid = in[0] + in[1] + in[2] - min - max;
            uchar redValue = in[0];
            uchar greenValue = in[1];
            uchar blueValue = in[2];

            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                outResult[2] = blueValue;
                outResult[1] = greenValue;
                outResult[0] = redValue;
                if (channel == 4) {
                    outResult[3] = in[3];
                }
                in = in + channel;
                outResult = outResult + channel;
                maskIn = maskIn + channel;
                continue;
            }

            uchar resultRed = redValue;
            uchar resultGreen = greenValue;
            uchar resultBlue = blueValue;
            changeH(resultRed,resultGreen,resultBlue,hueData.rgbS,hueData.rgbB,hueData.rgbH,redValue,greenValue,blueValue,max,min,0,hueData);
            outResult[2] = blueValue * (1 - maxMask / 255.0) + resultBlue * maxMask / 255.0;
            outResult[1] = greenValue * (1 - maxMask / 255.0) + resultGreen * maxMask / 255.0;
            outResult[0] = redValue * (1 - maxMask / 255.0) + resultRed* maxMask / 255.0;

            if (channel == 4) {
                outResult[3] = 255;
            }
            in = in + channel;
            outResult = outResult + channel;
            maskIn = maskIn + channel;
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

jobject ChangeHsbS(JNIEnv *env, jclass clazz, jobject bitmap, HueChange hueData){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2RGB);
    int channel = image.channels();
    int widht = channel * image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        const uchar *in = image.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);

        for (int j = 0; j < widht; j = j + channel) {
            uchar max = MAX(in[j], MAX(in[j + 1], in[j + 2]));
            uchar min = MIN(in[j], MIN(in[j + 1], in[j + 2]));
            uchar mid = in[j] + in[j + 1] + in[j + 2] - min - max;
            uchar redValue = in[j + 0];
            uchar greenValue = in[j + 1];
            uchar blueValue = in[j + 2];
            uchar resultRed = redValue;
            uchar resultGreen = greenValue;
            uchar resultBlue = blueValue;
            changeS(resultRed,resultGreen,resultBlue,hueData.rgbS,hueData.rgbB,hueData.rgbH,redValue,greenValue,blueValue,max,min,0,hueData);
            outResult[j + 0] = resultRed;
            outResult[j + 1] = resultGreen;
            outResult[j + 2] = resultBlue;
            if (channel == 4) {
                outResult[j + 3] = 255;
            }
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

jobject ChangeHsbB(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask, HueChange hueData){
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


    int channel = image.channels();
    int widht =  image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        uchar *in = image.data + i * image.step;
//        const uchar *in = image.ptr<uchar>(i);
        uchar *maskIn = maskMt.data + i * maskMt.step;
//        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *outResult = result.data + i * result.step;

        //uchar *outResult = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j++) {
            uchar max = MAX(in[0], MAX(in[1], in[2]));
            uchar min = MIN(in[0], MIN(in[1], in[2]));
            uchar mid = in[0] + in[1] + in[2] - min - max;
            uchar redValue = in[0];
            uchar greenValue = in[1];
            uchar blueValue = in[2];

            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                outResult[0] = redValue;
                outResult[1] = greenValue;
                outResult[2] = blueValue;
                in = in + channel;
                maskIn = maskIn + channel;
                outResult = outResult + channel;
                continue;
            }

            uchar resultRed = redValue;
            uchar resultGreen = greenValue;
            uchar resultBlue = blueValue;

            if (hueData.rgbH != 0 || hueData.rgbS != 0 || hueData.rgbB != 0) {
                //RGB
                changeB(resultRed, resultGreen, resultBlue, hueData.rgbS, hueData.rgbB,
                            hueData.rgbH, redValue, greenValue, blueValue, max, min, 0,hueData);
            }


            if ((max == redValue || max == greenValue) && min == blueValue &&
                (hueData.yH != 0 || hueData.yS != 0 || hueData.yB != 0)) {
                //黄色通道
                //cout << "y" << endl;
                if (blueValue != redValue && blueValue != greenValue) {
                    changeB(resultRed, resultGreen, resultBlue, hueData.yS, hueData.yB,
                                hueData.yH, redValue, greenValue, blueValue, max, min, 1,hueData);
                }
            }
            if ((max == redValue && greenValue != redValue && blueValue != redValue &&
                 (hueData.rH != 0 || hueData.rS != 0 || hueData.rB != 0))) {
                //红色通道
                //cout << "r" << endl;
                changeB(resultRed, resultGreen, resultBlue, hueData.rS, hueData.rB, hueData.rH,
                            redValue, greenValue, blueValue, max, min, 2,hueData);
            }
            if (max == blueValue && (hueData.bH != 0 || hueData.bS != 0 || hueData.bB != 0)) {
                //蓝色通道
                //cout << "b" << endl;
                changeB(resultRed, resultGreen, resultBlue, hueData.bS, hueData.bB, hueData.bH,
                            redValue, greenValue, blueValue, max, min, 3,hueData);
            }
            if (max == greenValue && (hueData.gH != 0 || hueData.gS != 0 || hueData.gB != 0)) {
                //绿色通道
                //cout << "g" << endl;
                changeB(resultRed, resultGreen, resultBlue, hueData.gS, hueData.gB, hueData.gH,
                            redValue, greenValue, blueValue, max, min, 4,hueData);
            }
            if ((max == blueValue || max == greenValue) && min == redValue &&
                (hueData.cH != 0 || hueData.cS != 0 || hueData.cB != 0)) {
                //青色
                //cout << "c" << endl;
                if (redValue != blueValue && redValue != greenValue) {
                    changeB(resultRed, resultGreen, resultBlue, hueData.cS, hueData.cB,
                                hueData.cH, redValue, greenValue, blueValue, max, min, 5,hueData);
                }
            }
            if (min == greenValue && (hueData.mH != 0 || hueData.mS != 0 || hueData.mB != 0)) {
                //洋红
                //cout << "m" << endl;
                if (redValue != greenValue && blueValue != greenValue) {
                    changeB(resultRed, resultGreen, resultBlue, hueData.mS, hueData.mB,
                                hueData.mH, redValue, greenValue, blueValue, max, min, 6,hueData);
                }
            }

            outResult[2] = blueValue * (1 - maxMask / 255.0) + resultBlue * maxMask / 255.0;
            outResult[1] = greenValue * (1 - maxMask / 255.0) + resultGreen * maxMask / 255.0;
            outResult[0] = redValue * (1 - maxMask / 255.0) + resultRed * maxMask / 255.0;
            if (channel == 4) {
                outResult[3] = 255;
            }
            in = in + channel;
            maskIn = maskIn + channel;
            outResult = outResult + channel;
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

void changeColor(uchar &resultRed, uchar &resultGreen, uchar &resultBlue, float sPercentage,
                 float bPercentage, int hue, uchar redValue, uchar greenValue, uchar blueValue,
                 int max, int min, int type,HueChange hueChange) {
    changeS(resultRed,resultGreen,resultBlue,sPercentage,bPercentage,hue,redValue,greenValue,blueValue,max,min,type,hueChange);

    if (resultRed > 255) {
        resultRed = 255;
    } else if (resultRed < 0) {
        resultRed = 0;
    }

    if (resultGreen > 255) {
        resultGreen = 255;
    } else if (resultGreen < 0) {
        resultGreen = 0;
    }

    if (resultBlue > 255) {
        resultBlue = 255;
    } else if (resultBlue < 0) {
        resultBlue = 0;
    }

    changeB(resultRed,resultGreen,resultBlue,sPercentage,bPercentage,hue,redValue,greenValue,blueValue,max,min,type,hueChange);

    if (resultRed > 255) {
        resultRed = 255;
    } else if (resultRed < 0) {
        resultRed = 0;
    }

    if (resultGreen > 255) {
        resultGreen = 255;
    } else if (resultGreen < 0) {
        resultGreen = 0;
    }

    if (resultBlue > 255) {
        resultBlue = 255;
    } else if (resultBlue < 0) {
        resultBlue = 0;
    }

    changeH(resultRed,resultGreen,resultBlue,sPercentage,bPercentage,hue,redValue,greenValue,blueValue,max,min,type,hueChange);

    if (resultRed > 255) {
        resultRed = 255;
    } else if (resultRed < 0) {
        resultRed = 0;
    }

    if (resultGreen > 255) {
        resultGreen = 255;
    } else if (resultGreen < 0) {
        resultGreen = 0;
    }

    if (resultBlue > 255) {
        resultBlue = 255;
    } else if (resultBlue < 0) {
        resultBlue = 0;
    }
}

void changeH(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange) {
    //修改明度
    //纯色的上边界,红色（345~15）,黄色(45~75)
    int right = 0;
    //纯色的下边界
    int left = 0;
    //最小边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int minBroder;
    //最大边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int maxBroder;
    int md;
    //上边界，下边界，边界最大值和边界最小值可以更具photoshop的色相饱和度工具推出。可以用油漆桶对调成极限颜色，再通过饱和度工具修改明度，看色值hsb是否收到影响。
    //修改色相
    uchar colorTmp[] = { resultBlue, resultGreen, resultRed };
    float hsbTmp[3];
    BGR2HSB(colorTmp, hsbTmp);
    if (hsbTmp[0] >= 315 || hsbTmp[0] <= 45 && type == 2) {
        //红色
        right = 15;
        left = 345;
        minBroder = 315;
        maxBroder = 45;
    }
    else if (hsbTmp[0] >= 15 && hsbTmp[0] <= 105 && type == 1) {
        //黄色
        left = 45;
        right = 75;
        minBroder = 15;
        maxBroder = 105;
    }
    else if (hsbTmp[0] >= 75 && hsbTmp[0] <= 165 && type == 4) {
        //绿色
        left = 105;
        right = 135;
        minBroder = 75;
        maxBroder = 165;
    }
    else if (hsbTmp[0] >= 135 && hsbTmp[0] <= 225 && type == 5) {
        //青色
        left = 165;
        right = 195;
        minBroder = 135;
        maxBroder = 225;
    }
    else if (hsbTmp[0] >= 195 && hsbTmp[0] <= 285 && type == 3) {
        //蓝色
        left = 225;
        right = 255;
        minBroder = 195;
        maxBroder = 285;
    }
    else if (hsbTmp[0] >= 255 && hsbTmp[0] <= 345 && type == 6) {
        //洋红
        left = 285;
        right = 315;
        minBroder = 255;
        maxBroder = 345;
    }
    else if ( redValue == greenValue && greenValue == blueValue) {
        return;
    }else if(type == 0){
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }
    else {
        return;
    }

    if (hueChange.leftChoose != -1 && hueChange.rightChoose != -1 && hueChange.minChoose != -1 && hueChange.maxChoose != -1){
        uchar inputColorArray[] = { (uchar)hueChange.bChoose,(uchar)hueChange.gChoose,(uchar)hueChange.rChoose };
        float inputHsb[3];
        BGR2HSB(colorTmp, inputHsb);
        right = hueChange.rightChoose;
        left = hueChange.leftChoose;
        minBroder = hueChange.minChoose;
        maxBroder = hueChange.maxChoose;
    }

    if (hue > 0)
    {
        if (!(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0 ) {
            if (type == 0) {
                if (hsbTmp[0] + hue >= 360) {
                    hsbTmp[0] = ((int)hsbTmp[0] + hue) % 360;
                }
                else {
                    hsbTmp[0] = ((int)hsbTmp[0] + hue);
                }
            }
            else {
                //-919表示没有定义
                int resultH = -919;
                if (hsbTmp[0] < left && hsbTmp[0] >= minBroder) {
                    resultH = hsbTmp[0] + hue * ((hsbTmp[0] - minBroder) / 30);
                }
                else if (hsbTmp[0] >= left && hsbTmp[0] <= right) {
                    resultH = ((int)hsbTmp[0] + hue);
                }
                else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder) {
                    resultH = hsbTmp[0] + hue * ((maxBroder - hsbTmp[0]) / 30);
                }


                if (maxBroder  <  minBroder) {
                    //红色特殊处理
                    if (hsbTmp[0] < left && hsbTmp[0] >= minBroder) {
                        resultH = hsbTmp[0] + hue * ((hsbTmp[0] - minBroder) / 30);
                    }
                    else if (hsbTmp[0] >= left || hsbTmp[0] <= right) {
                        resultH = ((int)hsbTmp[0] + hue);
                    }
                    else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder) {
                        resultH = hsbTmp[0] + hue * ((maxBroder - hsbTmp[0]) / 30);
                    }
                }
                if (resultH != -919) {
                    if (resultH >= 360) {
                        hsbTmp[0] = resultH % 360;
                    }
                    else {
                        hsbTmp[0] = resultH;
                    }
                }
            }
            HSB2BGR(hsbTmp, colorTmp);
            resultRed = colorTmp[2];
            resultGreen = colorTmp[1];
            resultBlue = colorTmp[0];
        }
    }
    else if (hue < 0)
    {
        if (!(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0) {
            if (type == 0) {
                //hue为负数，后面加减时注意用+号或用abs函数取绝对值，再用-号
                if (hsbTmp[0] + hue < 0) {
                    hsbTmp[0] = 360 - abs(((int)hsbTmp[0] + hue));
                }
                else {
                    hsbTmp[0] = hsbTmp[0] + hue;
                }
            }
            else {
                //-919表示没有定义
                int resultH = -919;
                if (hsbTmp[0] < left && hsbTmp[0] >= minBroder) {
                    resultH = hsbTmp[0] + hue * ((hsbTmp[0] - minBroder) / 30);
                }
                else if (hsbTmp[0] >= left && hsbTmp[0] <= right) {
                    resultH = ((int)hsbTmp[0] + hue);
                }
                else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder) {
                    resultH = hsbTmp[0] + hue * ((maxBroder - hsbTmp[0]) / 30);
                }
                if (maxBroder  <  minBroder) {
                    //红色特殊处理
                    if (hsbTmp[0] < left && hsbTmp[0] >= minBroder) {
                        resultH = hsbTmp[0] + hue * ((hsbTmp[0] - minBroder) / 30);
                    }
                    else if (hsbTmp[0] >= left || hsbTmp[0] <= right) {
                        resultH = ((int)hsbTmp[0] + hue);
                    }
                    else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder) {
                        resultH = hsbTmp[0] + hue * ((maxBroder - hsbTmp[0]) / 30);
                    }
                }
                if (resultH != -919) {
                    if (resultH < 0) {
                        hsbTmp[0] = 360 - abs(resultH);
                    }
                    else {
                        hsbTmp[0] = resultH;
                    }
                }
            }
        }


        HSB2BGR(hsbTmp, colorTmp);
        resultRed = colorTmp[2];
        resultGreen = colorTmp[1];
        resultBlue = colorTmp[0];

    }
}

/*
*
*
*修改饱和度
*/
/*
*
*
*修改饱和度
*/
void changeS(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange) {
    uchar color[] = { resultBlue, resultGreen, resultRed };
    float hsb[3];
    BGR2HSB(color, hsb);
    //修改明度
    //纯色的上边界,红色（345~15）,黄色(45~75)
    int right = 0;
    //纯色的下边界
    int left = 0;
    //最小边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int minBroder;
    //最大边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int maxBroder;
    int md;
    //上边界，下边界，边界最大值和边界最小值可以更具photoshop的色相饱和度工具推出。可以用油漆桶对调成极限颜色，再通过饱和度工具修改明度，看色值hsb是否收到影响。
    //修改色相
    uchar colorTmp[] = { resultBlue, resultGreen, resultRed };
    float hsbTmp[3];
    BGR2HSB(colorTmp, hsbTmp);
    if (hsbTmp[0] >= 315 || hsbTmp[0] <= 45 && type == 2) {
        //红色
        right = 15;
        left = 345;
        minBroder = 315;
        maxBroder = 45;
    }
    else if (hsbTmp[0] >= 15 && hsbTmp[0] <= 105 && type == 1) {
        //黄色
        left = 45;
        right = 75;
        minBroder = 15;
        maxBroder = 105;
    }
    else if (hsbTmp[0] >= 75 && hsbTmp[0] <= 165 && type == 4) {
        //绿色
        left = 105;
        right = 135;
        minBroder = 75;
        maxBroder = 165;
    }
    else if (hsbTmp[0] >= 135 && hsbTmp[0] <= 225 && type == 5) {
        //青色
        left = 165;
        right = 195;
        minBroder = 135;
        maxBroder = 225;
    }
    else if (hsbTmp[0] >= 195 && hsbTmp[0] <= 285 && type == 3) {
        //蓝色
        left = 225;
        right = 255;
        minBroder = 195;
        maxBroder = 285;
    }
    else if (hsbTmp[0] >= 255 && hsbTmp[0] <= 345 && type == 6) {
        //洋红
        left = 285;
        right = 315;
        minBroder = 255;
        maxBroder = 345;
    }
    else if (redValue == greenValue && greenValue == blueValue ) {
        return;
    }else if(type == 0){
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }
    else {
        return;
    }

    if (hueChange.leftChoose != -1 && hueChange.rightChoose != -1 && hueChange.minChoose != -1 && hueChange.maxChoose != -1){
        uchar inputColorArray[] = { (uchar)hueChange.bChoose,(uchar)hueChange.gChoose,(uchar)hueChange.rChoose };
        float inputHsb[3];
        BGR2HSB(colorTmp, inputHsb);
        if (hsbTmp[0] >= hueChange.minChoose && hsbTmp[0] <= hueChange.maxChoose){
            return;
        }
        right = hueChange.rightChoose;
        left = hueChange.leftChoose;
        minBroder = hueChange.minChoose;
        maxBroder = hueChange.maxChoose;
    }

    float h = hsb[0];
    float s = hsb[1];
    float l = hsb[2];
    //最大值加最小值表示s可变化的最大范围(photoshop找规律可知)
    int maxLimit = max + min;
    if (maxLimit > 255)
    {
        maxLimit = 255;
    }
    //根据photoshop规律可知，rgb的 (max - min)/2+min = minLimit,max表示三通道最大值，min表示三通道最小值，minLimit表示S降低到-100后最小的rgb值
    int minLimit = (max - min) / 2 + min;
    if (sPercentage > 0)
    {
        sPercentage = sPercentage * -1;
        //cout << sPercentage << endl;
        //大于0时候的饱和度变化
        if (redValue == greenValue && greenValue == blueValue)
        {
            resultRed = redValue;
            resultBlue = blueValue;
            resultGreen = greenValue;
        }
        else
        {
            if (max == redValue && greenValue >= blueValue)
            {
                resultRed = fabs(redValue + (maxLimit - redValue) * (sPercentage * -1));
                if (greenValue == blueValue)
                {
                    if (maxLimit - max <= blueValue)
                    {
                        resultBlue = fabs(blueValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        resultGreen = fabs(greenValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                    }
                    else
                    {
                        resultBlue = fabs(blueValue - (blueValue) * (sPercentage * -1)) + 0.5;
                        resultGreen = fabs(greenValue - (greenValue) * (sPercentage * -1)) + 0.5;
                    }
                }
                else
                {
                    if (maxLimit - max < blueValue)
                    {
                        resultBlue = fabs(blueValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                    }
                    else
                    {
                        resultBlue = fabs(blueValue - (blueValue) * (sPercentage * -1)) + 0.5;
                    }
                    resultGreen = (h * resultRed - h * resultBlue) / 60.0 + resultBlue + 0.5;
                }
            }
            else if (max == redValue && greenValue < blueValue)
            {
                resultRed = fabs(redValue + (maxLimit - redValue) * (sPercentage * -1)) + 0.5;
                if (maxLimit - max < blueValue)
                {
                    resultGreen = fabs(greenValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                }
                else
                {
                    resultGreen = fabs(greenValue - (greenValue) * (sPercentage * -1)) + 0.5;
                }
                resultBlue = -((h - 360) * (resultRed - resultGreen)) / 60.0 + resultGreen + 0.5;
            }
            else if (max == greenValue)
            {
                if (blueValue == redValue)
                {
                    if (maxLimit - max < blueValue)
                    {
                        resultRed = fabs(redValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        resultBlue = fabs(blueValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                    }
                    else
                    {
                        resultRed = fabs(redValue - (redValue) * (sPercentage * -1)) + 0.5;
                        resultBlue = fabs(blueValue - (blueValue) * (sPercentage * -1)) + 0.5;
                    }
                }
                else
                {
                    if (min == redValue)
                    {
                        if (maxLimit - max < min)
                        {
                            resultRed = fabs(redValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        }
                        else
                        {
                            resultRed = fabs(redValue - (redValue) * (sPercentage * -1)) + 0.5;
                        }
                        resultBlue = ((h - 120) * (resultGreen - resultRed)) / 60.0 + resultRed + 0.5;
                    }
                    else
                    {
                        if (maxLimit - max < min)
                        {
                            resultBlue = fabs(blueValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        }
                        else
                        {
                            resultBlue = fabs(blueValue - (blueValue) * (sPercentage * -1)) + 0.5;
                        }
                        resultRed = -(((h - 120) * (resultGreen - resultBlue)) / 60) + resultBlue + 0.5;
                    }
                }
            }
            else if (max == blueValue)
            {
                if (greenValue == redValue)
                {
                    if (maxLimit - max < greenValue)
                    {
                        resultRed = fabs(redValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        resultGreen = fabs(greenValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                    }
                    else
                    {
                        resultRed = fabs(redValue - (redValue) * (sPercentage * -1)) + 0.5;
                        resultGreen = fabs(greenValue - (greenValue) * (sPercentage * -1)) + 0.5;
                    }
                }
                else
                {
                    if (min == redValue)
                    {
                        if (maxLimit - max < min)
                        {
                            resultRed = fabs(redValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        }
                        else
                        {
                            resultRed = fabs(redValue - (redValue) * (sPercentage * -1)) + 0.5;
                        }
                        resultGreen = -((h - 240) * (resultBlue - resultRed) / 60.0) + resultRed + 0.5;
                    }
                    else
                    {
                        if (maxLimit - max < min)
                        {
                            resultGreen = fabs(greenValue - (maxLimit - max) * (sPercentage * -1)) + 0.5;
                        }
                        else
                        {
                            resultGreen = fabs(greenValue - (greenValue) * (sPercentage * -1)) + 0.5;
                        }
                        resultRed = ((h - 240) * (resultBlue - resultGreen) / 60.0) + resultGreen + 0.5;
                    }
                }
            }
        }
    }
    else if (sPercentage < 0)
    {
        sPercentage = sPercentage * -1;
        //低于0时候的饱和度变化
        if (type == 0) {
            resultRed = fabs(redValue - (redValue - minLimit) * (sPercentage));
            resultGreen = fabs(greenValue - (greenValue - minLimit) * (sPercentage));
            resultBlue = fabs(blueValue - (blueValue - minLimit) * (sPercentage));
        }
        else {
            int subtract = 0;
            if (hsbTmp[0] < left && hsbTmp[0] >= minBroder) {
                minLimit = minLimit - (minLimit - min) * (((left - hsbTmp[0]) / 30.0));
                int tmpMinLimit = (max - min) / 2 + min;
                if (min == redValue) {
                    resultRed = fabs(redValue - (redValue - minLimit) * (sPercentage));
                    if (max == blueValue) {
                        resultBlue = resultBlue - ((resultBlue - tmpMinLimit) - (resultBlue - tmpMinLimit)*(((left - hsbTmp[0]) / 30.0)))*sPercentage;
                        resultGreen = resultGreen + ((tmpMinLimit - resultGreen) - ((tmpMinLimit - resultGreen)* ((left - hsbTmp[0]) / 30.0)) )*sPercentage;
                    }
                    else {
                        resultGreen = resultGreen - ((resultGreen - tmpMinLimit) - (resultGreen - tmpMinLimit) * (((left - hsbTmp[0]) / 30.0))) * sPercentage;
                        resultBlue = resultBlue + ((tmpMinLimit - resultBlue) - ((tmpMinLimit - resultBlue) * ((left - hsbTmp[0]) / 30.0))) * sPercentage;
                    }
                }
                else if (min == greenValue) {
                    resultGreen = fabs(greenValue - (greenValue - minLimit) * (sPercentage));
                    if (max == redValue) {
                        resultRed = resultRed - ((resultRed - tmpMinLimit) - (resultRed - tmpMinLimit) * (((left - hsbTmp[0]) / 30.0))) * sPercentage;
                        resultBlue = resultBlue + ((tmpMinLimit - resultBlue) - ((tmpMinLimit - resultBlue) * ((left - hsbTmp[0]) / 30.0))) * sPercentage;
                    }
                    else {
                        resultBlue = resultBlue - ((resultBlue - tmpMinLimit) - (resultBlue - tmpMinLimit) * (((left - hsbTmp[0]) / 30.0))) * sPercentage;
                        resultRed = resultRed + ((tmpMinLimit - resultRed) - ((tmpMinLimit - resultRed) * ((left - hsbTmp[0]) / 30.0))) * sPercentage;
                    }
                }
                else if (min == blueValue) {
                    resultBlue = fabs(blueValue - (blueValue - minLimit) * (sPercentage));
                    if (max == redValue) {
                        resultRed = resultRed - ((resultRed - tmpMinLimit) - (resultRed - tmpMinLimit) * (((left - hsbTmp[0]) / 30.0))) * sPercentage;
                        resultGreen = resultGreen + ((tmpMinLimit - resultGreen) - ((tmpMinLimit - resultGreen) * ((left - hsbTmp[0]) / 30.0))) * sPercentage;
                    }
                    else {
                        resultGreen = resultGreen - ((resultGreen - tmpMinLimit) - (resultGreen - tmpMinLimit) * (((left - hsbTmp[0]) / 30.0))) * sPercentage;
                        resultRed = resultRed + ((tmpMinLimit - resultRed) - ((tmpMinLimit - resultRed) * ((left - hsbTmp[0]) / 30.0))) * sPercentage;
                    }
                }
            }
            else if (hsbTmp[0] >= left || hsbTmp[0] <= right) {
                resultRed = fabs(redValue - (redValue - minLimit) * (sPercentage));
                resultGreen = fabs(greenValue - (greenValue - minLimit) * (sPercentage));
                resultBlue = fabs(blueValue - (blueValue - minLimit) * (sPercentage));
            }
            else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder) {
                minLimit = minLimit - (minLimit - min) * (((hsbTmp[0] - right) / 30.0));
                int tmpMinLimit = (max - min) / 2 + min;
                if (min == redValue) {
                    resultRed = fabs(redValue + (minLimit - redValue) * (sPercentage));
                    resultBlue = resultBlue - ((resultBlue - tmpMinLimit) - ((resultBlue - tmpMinLimit)* (((hsbTmp[0] - right) / 30.0))) ) * sPercentage;
                    resultGreen = resultGreen - ((resultGreen - tmpMinLimit) - ((resultGreen - tmpMinLimit) * (((hsbTmp[0] - right) / 30.0)))) * sPercentage;
                }
                else if (min == greenValue) {
                    resultGreen = fabs(resultGreen + (minLimit - resultGreen) * (sPercentage));
                    resultBlue = resultBlue - ((resultBlue - tmpMinLimit) - ((resultBlue - tmpMinLimit) * (((hsbTmp[0] - right) / 30.0)))) * sPercentage;
                    resultRed = resultRed - ((resultRed - tmpMinLimit) - ((resultRed - tmpMinLimit) * (((hsbTmp[0] - right) / 30.0)))) * sPercentage;
                }
                else if (min == blueValue) {
                    resultBlue = fabs(blueValue + (minLimit - blueValue) * (sPercentage));
                    if (redValue == 238 && greenValue == 199 && blueValue == 120) {
                        int a = 0;
                    }
                    resultGreen = resultGreen - ((resultGreen - tmpMinLimit) - ((resultGreen - tmpMinLimit) * (((hsbTmp[0] - right) / 30.0)))) * sPercentage;
                    resultRed = resultRed - ((resultRed - tmpMinLimit) - ((resultRed - tmpMinLimit) * (((hsbTmp[0] - right) / 30.0)))) * sPercentage;
                }
            }

        }
    }
}

void changeB(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange) {
    //修改明度
    //纯色的上边界,红色（345~15）,黄色(45~75)
    int right = 0;
    //纯色的下边界
    int left = 0;
    //最小边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int minBroder = 0;
    //最大边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int maxBroder = 0;
    int md;
    //上边界，下边界，边界最大值和边界最小值可以更具photoshop的色相饱和度工具推出。可以用油漆桶对调成极限颜色，再通过饱和度工具修改明度，看色值hsb是否收到影响。
    uchar colorTmp[] = { resultBlue, resultGreen, resultRed };
    float hsbTmp[3];
    BGR2HSB(colorTmp, hsbTmp);
    if (hsbTmp[0] >= 315 || hsbTmp[0] <= 45 && type == 2) {
        //红色
        right = 15;
        left = 345;
        minBroder = 315;
        maxBroder = 45;
    }
    else if (hsbTmp[0] >= 15 && hsbTmp[0] <= 105 && type == 1) {
        //黄色
        left = 45;
        right = 75;
        minBroder = 15;
        maxBroder = 105;
    }
    else if (hsbTmp[0] >= 75 && hsbTmp[0] <= 165 && type == 4) {
        //绿色
        left = 105;
        right = 135;
        minBroder = 75;
        maxBroder = 165;
    }
    else if (hsbTmp[0] >= 135 && hsbTmp[0] <= 225 && type == 5) {
        //青色
        left = 165;
        right = 195;
        minBroder = 135;
        maxBroder = 225;
    }
    else if (hsbTmp[0] >= 195 && hsbTmp[0] <= 285 && type == 3) {
        //蓝色
        left = 225;
        right = 255;
        minBroder = 195;
        maxBroder = 285;
    }
    else if (hsbTmp[0] >= 255 && hsbTmp[0] <= 345 && type == 6) {
        //洋红
        left = 285;
        right = 315;
        minBroder = 255;
        maxBroder = 345;
    } else if (redValue == greenValue && greenValue == blueValue ) {
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }else if(type == 0){
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }
    else {
        return;
    }

    if (hueChange.leftChoose != -1 && hueChange.rightChoose != -1 && hueChange.minChoose != -1 && hueChange.maxChoose != -1){
        uchar inputColorArray[] = { (uchar)hueChange.bChoose,(uchar)hueChange.gChoose,(uchar)hueChange.rChoose };
        float inputHsb[3];
        BGR2HSB(colorTmp, inputHsb);
        if (hsbTmp[0] >= hueChange.minChoose && hsbTmp[0] <= hueChange.maxChoose){
            return;
        }
        right = hueChange.rightChoose;
        left = hueChange.leftChoose;
        minBroder = hueChange.minChoose;
        maxBroder = hueChange.maxChoose;
    }

    if (bPercentage < 0)
    {
        if ( !(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0) {
            if (type == 0)
            {
                if (resultBlue == resultGreen && resultGreen == resultRed)
                {
                    resultRed = resultRed + resultRed * bPercentage;
                    resultGreen = resultGreen + resultGreen * bPercentage;
                    resultBlue = resultBlue + resultBlue * bPercentage;
                }
                else
                {
                    hsbTmp[2] = hsbTmp[2] + hsbTmp[2] * bPercentage;
                    HSB2BGR(hsbTmp, colorTmp);
                    resultRed = colorTmp[2];
                    resultGreen = colorTmp[1];
                    resultBlue = colorTmp[0];
                }
            }
            else
            {
                int maxTmp = MAX((int)resultRed, MAX((int)resultGreen, (int)resultBlue));
                int minTmp = MIN((int)resultRed, MIN((int)resultGreen, (int)resultBlue));
                //超出纯色的比值
                float outOfColorPrecentage = -1;
                bool isRedIn = false;
                if (left == 345) {
                    if (hsbTmp[0] <= 360 && hsbTmp[0] >= left || (hsbTmp[0] >= 0 && hsbTmp[0] <= 15)) {
                        isRedIn = true;
                    }
                    else if (hsbTmp[0] > 15 && hsbTmp[0] <= maxBroder) {
                        outOfColorPrecentage = (hsbTmp[0] - 15) / 30;
                    }
                    else if (hsbTmp[0] >= minBroder && hsbTmp[0] < left)
                    {
                        outOfColorPrecentage = (left - hsbTmp[0]) / 30;
                    }
                }
                if ((hsbTmp[0] >= left && hsbTmp[0] <= right) || isRedIn) {
                    if (minTmp == resultRed) {
                        resultRed = minTmp;
                        resultGreen = resultGreen - (resultGreen - minTmp) * fabs(bPercentage);
                        resultBlue = resultBlue - (resultBlue - minTmp) * fabs(bPercentage);
                    }
                    else if (minTmp == resultGreen) {
                        resultGreen = minTmp;
                        resultRed = resultRed - (resultRed - minTmp) * fabs(bPercentage);
                        resultBlue = resultBlue - (resultBlue - minTmp) * fabs(bPercentage);
                    }
                    else if (minTmp == resultBlue) {
                        resultBlue = minTmp;
                        resultRed = resultRed - (resultRed - minTmp) * fabs(bPercentage);
                        resultGreen = resultGreen - (resultGreen - minTmp) * fabs(bPercentage);
                    }
                }
                else
                {
                    if ((hsbTmp[0] < left && hsbTmp[0] >= minBroder) && outOfColorPrecentage == -1)
                    {
                        outOfColorPrecentage = (left - hsbTmp[0]) / 30;
                    }
                    else if ((hsbTmp[0] > right && hsbTmp[0] <= maxBroder) && outOfColorPrecentage == -1) {
                        outOfColorPrecentage = (hsbTmp[0] - right) / 30;
                    }
                    if (outOfColorPrecentage != -1) {
                        //纯色情况下最大明度与最小明度之间的差距，纯色情况下最大亮度可以被调整到最小明度
                        int maxLength = maxTmp - minTmp;
                        //最大明度允许被调节到的最小值
                        int newMax = maxTmp - ((1 - outOfColorPrecentage) * maxLength + 0.5);
                        if (newMax < 0) {
                            newMax = 0;
                        }
                        if (maxTmp == resultRed) {
                            resultRed = resultRed - (resultRed - newMax) * fabsf(bPercentage);
                            if (minTmp == resultGreen) {
                                resultGreen = minTmp;
                                resultBlue = -1 * ((hsbTmp[0] - 360) * (resultRed - resultGreen) / 60 - resultGreen);
                            }
                            else {
                                resultBlue = minTmp;
                                resultGreen = hsbTmp[0] * (resultRed - resultBlue) / 60 + resultBlue;
                            }

                        }
                        else if (maxTmp == resultGreen) {
                            resultGreen = resultGreen - (resultGreen - newMax) * fabsf(bPercentage);
                            if (minTmp == resultBlue) {
                                resultBlue = minTmp;
                                resultRed = -1 * ((hsbTmp[0] - 120) * (resultGreen - resultBlue) / 60 - resultBlue);
                            }
                            else {
                                resultRed = minTmp;
                                resultBlue = (hsbTmp[0] - 120) * (resultGreen - resultRed) / 60 + resultRed;
                            }

                        }
                        else if (maxTmp == resultBlue) {
                            resultBlue = resultBlue - (resultBlue - newMax) * fabsf(bPercentage);
                            if (minTmp == resultGreen) {
                                resultGreen = minTmp;
                                resultRed = (hsbTmp[0] - 240) * (resultBlue - resultGreen) / 60 + resultGreen;
                            }
                            else {
                                resultRed = minTmp;
                                resultGreen = -1 * ((hsbTmp[0] - 240) * (resultBlue - resultRed) / 60 - resultRed);
                            }
                        }
                    }
                }
            }
        }
    }
    else if (bPercentage > 0)
    {
        if ( !(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0) {
            if (type == 0)
            {
                int maxNew = MAX(resultRed, MAX(resultGreen, resultBlue));
                int minNew = MIN(resultRed, MIN(resultGreen, resultBlue));

                if (resultBlue == resultGreen && resultGreen == resultRed)
                {
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                    resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                }
                else if (maxNew == resultRed && resultGreen >= resultBlue)
                {
                    //修改明度时，色相不变，确定两个变量数值，求取第三个变量数值
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                    resultGreen = ((hsbTmp[0] * resultRed - hsbTmp[0] * resultBlue) / 60) + resultBlue;
                    //hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 0;
                }
                else if (maxNew == resultRed && resultGreen < resultBlue)
                {
                    //hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 360;
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                    resultBlue = -(((hsbTmp[0] - 360) * (resultRed - resultGreen)) / 60) + resultGreen;
                }
                else if (maxNew == resultGreen)
                {
                    if (minNew == resultRed)
                    {
                        resultRed = resultRed + (255 - resultRed) * bPercentage;
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultBlue = ((hsbTmp[0] - 120) * (resultGreen - resultRed)) / 60 + resultRed;
                    }
                    else
                    {
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultRed = -(((hsbTmp[0] - 120) * (resultGreen - resultBlue)) / 60) + resultBlue;
                    }
                }
                else if (maxNew == resultBlue)
                {
                    if (minNew == resultGreen)
                    {
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultRed = ((hsbTmp[0] - 240) * (resultBlue - resultGreen)) / 60 + resultGreen;
                    }
                    else
                    {
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultRed = resultRed + (255 - resultRed) * bPercentage;
                        resultGreen = -(((hsbTmp[0] - 240) * (resultBlue - resultRed)) / 60) + resultRed;
                    }
                }
            }
            else
            {
                if (left == 345)
                {
                    if (hsbTmp[0] >= minBroder)
                    {
                        if (hsbTmp[0] - left >= 0)
                        {
                            hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                            if (hsbTmp[1] == 0)
                            {
                                hsbTmp[0] = 0;
                            }
                        }
                        else
                        {
                            hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - (left - hsbTmp[0]) / 30 * hsbTmp[1]) * bPercentage;
                        }
                    }
                    else
                    {
                        if (hsbTmp[0] - right <= 0)
                        {
                            hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                            if (hsbTmp[1] == 0)
                            {
                                hsbTmp[0] = 0;
                            }
                        }
                        else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder)
                        {
                            hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - (hsbTmp[0] - 15) / 30 * hsbTmp[1]) * bPercentage;
                        }
                    }
                    HSB2BGR(hsbTmp, colorTmp);
                    resultBlue = colorTmp[0];
                    resultGreen = colorTmp[1];
                    resultRed = colorTmp[2];
                }
                else
                {
                    if (hsbTmp[0] >= left && hsbTmp[0] <= right)
                    {
                        hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                        if (hsbTmp[1] == 0)
                        {
                            hsbTmp[0] = 0;
                        }
                    }
                    else if (hsbTmp[0] >= minBroder && hsbTmp[0] < left)
                    {
                        float b = (hsbTmp[1] - (hsbTmp[0] - minBroder) / 30 * hsbTmp[1]);
                        hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - b) * bPercentage;
                    }
                    else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder)
                    {
                        float b = (hsbTmp[1] - (hsbTmp[0] - right) / 30 * hsbTmp[1]) * bPercentage;
                        hsbTmp[1] = hsbTmp[1] - b;

                    }
                    HSB2BGR(hsbTmp, colorTmp);
                    resultBlue = colorTmp[0];
                    resultGreen = colorTmp[1];
                    resultRed = colorTmp[2];
                }
            }
        }
    }
}

void changeBTwo(uchar& resultRed, uchar& resultGreen, uchar& resultBlue, float sPercentage, float bPercentage, int hue, int redValue, int greenValue, int blueValue, int max, int min, int type,HueChange hueChange) {
    //修改明度
    //纯色的上边界,红色（345~15）,黄色(45~75)
    int right = 0;
    //纯色的下边界
    int left = 0;
    //最小边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int minBroder = 0;
    //最大边界,除了纯色区域外，左边界和有边界存在30度的过度色
    int maxBroder = 0;
    int md;
    //上边界，下边界，边界最大值和边界最小值可以更具photoshop的色相饱和度工具推出。可以用油漆桶对调成极限颜色，再通过饱和度工具修改明度，看色值hsb是否收到影响。
    uchar colorTmp[] = { resultBlue, resultGreen, resultRed };
    float hsbTmp[3];
    BGR2HSB(colorTmp, hsbTmp);
    if (hsbTmp[0] >= 315 || hsbTmp[0] <= 45 && type == 2) {
        //红色
        right = 15;
        left = 345;
        minBroder = 315;
        maxBroder = 45;
    }
    else if (hsbTmp[0] >= 15 && hsbTmp[0] <= 105 && type == 1) {
        //黄色
        left = 45;
        right = 75;
        minBroder = 15;
        maxBroder = 105;
    }
    else if (hsbTmp[0] >= 75 && hsbTmp[0] <= 165 && type == 4) {
        //绿色
        left = 105;
        right = 135;
        minBroder = 75;
        maxBroder = 165;
    }
    else if (hsbTmp[0] >= 135 && hsbTmp[0] <= 225 && type == 5) {
        //青色
        left = 165;
        right = 195;
        minBroder = 135;
        maxBroder = 225;
    }
    else if (hsbTmp[0] >= 195 && hsbTmp[0] <= 285 && type == 3) {
        //蓝色
        left = 225;
        right = 255;
        minBroder = 195;
        maxBroder = 285;
    }
    else if (hsbTmp[0] >= 255 && hsbTmp[0] <= 345 && type == 6) {
        //洋红
        left = 285;
        right = 315;
        minBroder = 255;
        maxBroder = 345;
    } else if (redValue == greenValue && greenValue == blueValue ) {
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }else if(type == 0){
        left = 0;
        right = 0;
        minBroder = 0;
        maxBroder = 0;
    }
    else {
        return;
    }

    if (hueChange.leftChoose != -1 && hueChange.rightChoose != -1 && hueChange.minChoose != -1 && hueChange.maxChoose != -1){
        uchar inputColorArray[] = { (uchar)hueChange.bChoose,(uchar)hueChange.gChoose,(uchar)hueChange.rChoose };
        float inputHsb[3];
        BGR2HSB(colorTmp, inputHsb);
        if (hsbTmp[0] >= hueChange.minChoose && hsbTmp[0] <= hueChange.maxChoose){
            return;
        }
        right = hueChange.rightChoose;
        left = hueChange.leftChoose;
        minBroder = hueChange.minChoose;
        maxBroder = hueChange.maxChoose;
    }

    if (bPercentage < 0)
    {
        if ( !(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0) {
            if (type == 0)
            {
                if (resultBlue == resultGreen && resultGreen == resultRed)
                {
                    resultRed = resultRed + resultRed * bPercentage;
                    resultGreen = resultGreen + resultGreen * bPercentage;
                    resultBlue = resultBlue + resultBlue * bPercentage;
                }
                else
                {
                    hsbTmp[2] = hsbTmp[2] + hsbTmp[2] * bPercentage;
                    HSB2BGR(hsbTmp, colorTmp);
                    resultRed = colorTmp[2];
                    resultGreen = colorTmp[1];
                    resultBlue = colorTmp[0];
                }
            }
            else
            {
                int maxTmp = MAX((int)resultRed, MAX((int)resultGreen, (int)resultBlue));
                int minTmp = MIN((int)resultRed, MIN((int)resultGreen, (int)resultBlue));
                //超出纯色的比值
                float outOfColorPrecentage = -1;
                bool isRedIn = false;
                if (left == 345) {
                    if (hsbTmp[0] <= 360 && hsbTmp[0] >= left || (hsbTmp[0] >= 0 && hsbTmp[0] <= 15)) {
                        isRedIn = true;
                    }
                    else if (hsbTmp[0] > 15 && hsbTmp[0] <= maxBroder) {
                        outOfColorPrecentage = (hsbTmp[0] - 15) / 30;
                    }
                    else if (hsbTmp[0] >= minBroder && hsbTmp[0] < left)
                    {
                        outOfColorPrecentage = (left - hsbTmp[0]) / 30;
                    }
                }
                if ((hsbTmp[0] >= left && hsbTmp[0] <= right) || isRedIn) {
                    if (minTmp == resultRed) {
                        resultRed = minTmp;
                        resultGreen = resultGreen - (resultGreen - minTmp) * fabs(bPercentage);
                        resultBlue = resultBlue - (resultBlue - minTmp) * fabs(bPercentage);
                    }
                    else if (minTmp == resultGreen) {
                        resultGreen = minTmp;
                        resultRed = resultRed - (resultRed - minTmp) * fabs(bPercentage);
                        resultBlue = resultBlue - (resultBlue - minTmp) * fabs(bPercentage);
                    }
                    else if (minTmp == resultBlue) {
                        resultBlue = minTmp;
                        resultRed = resultRed - (resultRed - minTmp) * fabs(bPercentage);
                        resultGreen = resultGreen - (resultGreen - minTmp) * fabs(bPercentage);
                    }
                }
                else
                {
                    if ((hsbTmp[0] < left && hsbTmp[0] >= minBroder) && outOfColorPrecentage == -1)
                    {
                        outOfColorPrecentage = (left - hsbTmp[0]) / 30;
                    }
                    else if ((hsbTmp[0] > right && hsbTmp[0] <= maxBroder) && outOfColorPrecentage == -1) {
                        outOfColorPrecentage = (hsbTmp[0] - right) / 30;
                    }
                    if (outOfColorPrecentage != -1) {
                        //纯色情况下最大明度与最小明度之间的差距，纯色情况下最大亮度可以被调整到最小明度
                        int maxLength = maxTmp - minTmp;
                        //最大明度允许被调节到的最小值
                        int newMax = maxTmp - ((1 - outOfColorPrecentage) * maxLength + 0.5);
                        if (newMax < 0) {
                            newMax = 0;
                        }
                        if (maxTmp == resultRed) {
                            resultRed = resultRed - (resultRed - newMax) * fabsf(bPercentage);
                            if (minTmp == resultGreen) {
                                resultGreen = minTmp;
                                resultBlue = -1 * ((hsbTmp[0] - 360) * (resultRed - resultGreen) / 60 - resultGreen);
                            }
                            else {
                                resultBlue = minTmp;
                                resultGreen = hsbTmp[0] * (resultRed - resultBlue) / 60 + resultBlue;
                            }

                        }
                        else if (maxTmp == resultGreen) {
                            resultGreen = resultGreen - (resultGreen - newMax) * fabsf(bPercentage);
                            if (minTmp == resultBlue) {
                                resultBlue = minTmp;
                                resultRed = -1 * ((hsbTmp[0] - 120) * (resultGreen - resultBlue) / 60 - resultBlue);
                            }
                            else {
                                resultRed = minTmp;
                                resultBlue = (hsbTmp[0] - 120) * (resultGreen - resultRed) / 60 + resultRed;
                            }

                        }
                        else if (maxTmp == resultBlue) {
                            resultBlue = resultBlue - (resultBlue - newMax) * fabsf(bPercentage);
                            if (minTmp == resultGreen) {
                                resultGreen = minTmp;
                                resultRed = (hsbTmp[0] - 240) * (resultBlue - resultGreen) / 60 + resultGreen;
                            }
                            else {
                                resultRed = minTmp;
                                resultGreen = -1 * ((hsbTmp[0] - 240) * (resultBlue - resultRed) / 60 - resultRed);
                            }
                        }
                    }
                }
            }
        }
    }
    else if (bPercentage > 0)
    {
        if ( !(left == 0 && right == 0 && minBroder == 0 && maxBroder == 0) || type == 0) {
            if (type == 0)
            {
                int maxNew = MAX(resultRed, MAX(resultGreen, resultBlue));
                int minNew = MIN(resultRed, MIN(resultGreen, resultBlue));

                if (resultBlue == resultGreen && resultGreen == resultRed)
                {
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                    resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                }
                else if (maxNew == resultRed && resultGreen >= resultBlue)
                {
                    //修改明度时，色相不变，确定两个变量数值，求取第三个变量数值
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                    resultGreen = ((hsbTmp[0] * resultRed - hsbTmp[0] * resultBlue) / 60) + resultBlue;
                    //hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 0;
                }
                else if (maxNew == resultRed && resultGreen < resultBlue)
                {
                    //hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 360;
                    resultRed = resultRed + (255 - resultRed) * bPercentage;
                    resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                    resultBlue = -(((hsbTmp[0] - 360) * (resultRed - resultGreen)) / 60) + resultGreen;
                }
                else if (maxNew == resultGreen)
                {
                    if (minNew == resultRed)
                    {
                        resultRed = resultRed + (255 - resultRed) * bPercentage;
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultBlue = ((hsbTmp[0] - 120) * (resultGreen - resultRed)) / 60 + resultRed;
                    }
                    else
                    {
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultRed = -(((hsbTmp[0] - 120) * (resultGreen - resultBlue)) / 60) + resultBlue;
                    }
                }
                else if (maxNew == resultBlue)
                {
                    if (minNew == resultGreen)
                    {
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultGreen = resultGreen + (255 - resultGreen) * bPercentage;
                        resultRed = ((hsbTmp[0] - 240) * (resultBlue - resultGreen)) / 60 + resultGreen;
                    }
                    else
                    {
                        resultBlue = resultBlue + (255 - resultBlue) * bPercentage;
                        resultRed = resultRed + (255 - resultRed) * bPercentage;
                        resultGreen = -(((hsbTmp[0] - 240) * (resultBlue - resultRed)) / 60) + resultRed;
                    }
                }
            }
            else
            {
                if (left == 345)
                {
                    if (hsbTmp[0] >= minBroder)
                    {
                        if (hsbTmp[0] - left >= 0)
                        {
                            hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                            if (hsbTmp[1] == 0)
                            {
                                hsbTmp[0] = 0;
                            }
                        }
                        else
                        {
                            hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - (left - hsbTmp[0]) / 30 * hsbTmp[1]) * bPercentage;
                        }
                    }
                    else
                    {
                        if (hsbTmp[0] - right <= 0)
                        {
                            hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                            if (hsbTmp[1] == 0)
                            {
                                hsbTmp[0] = 0;
                            }
                        }
                        else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder)
                        {
                            hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - (hsbTmp[0] - 15) / 30 * hsbTmp[1]) * bPercentage;
                        }
                    }
                    HSB2BGR(hsbTmp, colorTmp);
                    resultBlue = colorTmp[0];
                    resultGreen = colorTmp[1];
                    resultRed = colorTmp[2];
                }
                else
                {
                    if (hsbTmp[0] >= left && hsbTmp[0] <= right)
                    {
                        hsbTmp[1] = hsbTmp[1] - hsbTmp[1] * bPercentage;
                        if (hsbTmp[1] == 0)
                        {
                            hsbTmp[0] = 0;
                        }
                    }
                    else if (hsbTmp[0] >= minBroder && hsbTmp[0] < left)
                    {
                        float b = (hsbTmp[1] - (hsbTmp[0] - minBroder) / 30 * hsbTmp[1]);
                        hsbTmp[1] = hsbTmp[1] - (hsbTmp[1] - b) * bPercentage;
                    }
                    else if (hsbTmp[0] > right && hsbTmp[0] <= maxBroder)
                    {
                        float b = (hsbTmp[1] - (hsbTmp[0] - right) / 30 * hsbTmp[1]) * bPercentage;
                        hsbTmp[1] = hsbTmp[1] - b;

                    }
                    HSB2BGR(hsbTmp, colorTmp);
                    resultBlue = colorTmp[0];
                    resultGreen = colorTmp[1];
                    resultRed = colorTmp[2];
                }
            }
        }
    }
}

/**
 * 饱和度调节
 * @param env
 * @param clazz
 * @param bitmap
 * @param hueData
 * @return
 */
jobject changeSaturation(JNIEnv *env, jclass clazz, jobject bitmap, int saturationValue){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2BGR);
    cv::Mat result = image.clone();
    cvtColor(result,result,CV_RGBA2BGRA);
    saturation(result.data,result.cols,result.rows,result.step,saturationValue);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 * 饱和度调节
 * @param env
 * @param clazz
 * @param bitmap
 * @param saturationValue
 * @return
 */
jobject changeSaturationTwo(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jfloat saturationValue){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cvtColor(image,image,CV_RGBA2BGR);
    cvtColor(maskMt,maskMt,CV_RGBA2BGR);
    cv::Mat result = image.clone();
    cvtColor(result,result,CV_RGBA2BGR);
    AdjustSatuation(result,saturationValue,maskMt);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 * 饱和度调节
 * @param srcData
 * @param width
 * @param height
 * @param stride
 * @param saturation
 * @return
 */
void saturation(unsigned char *srcData, int width, int height, int stride, int saturation)
{
    unsigned char* pSrc = srcData;
    int r, g, b, rgbMin, rgbMax;
    saturation = CLIP3(saturation,-100,100);
    int k = saturation / 100.0f * 128;
    int alpha = 0;
    int offset = stride - width * 4;
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            r = pSrc[2];
            g = pSrc[1];
            b = pSrc[0];
            rgbMin = MIN2(MIN2(r, g), b);
            rgbMax = MAX2(MAX2(r, g), b);
            int delta = (rgbMax - rgbMin);
            int value = (rgbMax + rgbMin);
            if(delta ==0)
            {
                pSrc += 4;
                continue;
            }
            int L = value >> 1;
            int S = L < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
            if(k >= 0)
            {
                alpha = k + S >= 128 ? S : 128 - k;
                alpha = 128 * 128 / alpha - 128;
            }
            else
                alpha = k;
            r = r + ((r - L) * alpha >> 7);
            g = g + ((g - L) * alpha >> 7);
            b = b + ((b - L) * alpha >> 7);
            pSrc[0] = CLIP3(b, 0, 255);
            pSrc[1] = CLIP3(g, 0, 255);
            pSrc[2] = CLIP3(r, 0, 255);
            pSrc += 4;
        }
        pSrc += offset;
    }
};

/**
 *
 * @return
 */
jobject ReplaceHAndS(int targetH,int targetS,int targetB,JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jfloat saturationValue,jfloat pro_s,jfloat pro_b){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    int channel = image.channels();
    int widht = channel * image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        uchar *in = image.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            uchar max = MAX(in[j+0], MAX(in[j+1], in[j+2]));
            uchar min = MIN(in[j+0], MIN(in[j+1], in[j+2]));
            uchar mid = in[j+0] + in[j+1] + in[j+2] - min - max;
            uchar redValue = in[j+0];
            uchar greenValue = in[j+1];
            uchar blueValue = in[j+2];
            uchar maxMask = MAX(maskIn[j+0], MAX(maskIn[j+1], maskIn[j+2]));
            if (maxMask == 0)
            {
                out[j+2] = blueValue;
                out[j+1] = greenValue;
                out[j+0] = redValue;
                if (channel == 4) {
                    out[j+3] = in[j+3];
                }
                continue;
            }
            uchar colorTmp[] = { blueValue, greenValue, redValue };
            float hsbTmp[3];
            BGR2HSB(colorTmp,hsbTmp);
            if(targetH != -1){
                hsbTmp[0] = targetH;
            }
            if(targetS != -1){
                hsbTmp[1] = targetS/100.0 + (1 - hsbTmp[1])*pro_s;
            }
            if(targetB != -1){
                hsbTmp[2] = targetB/100.0;
            }
            if( pro_b != -1){
                hsbTmp[2] = hsbTmp[2] + (1 - hsbTmp[2])*pro_b;
            }
            if(hsbTmp[1] >= 1){
                hsbTmp[1] = 1;
            }
            if(hsbTmp[2] >= 1){
                hsbTmp[2] = 1;
            }
            HSB2BGR(hsbTmp,colorTmp);
            out[j+2] = blueValue*(1-saturationValue) +  (saturationValue) * (blueValue * (1 - maxMask / 255.0) + colorTmp[0] * (maxMask / 255.0));
            out[j+1] = greenValue*(1-saturationValue) +  (saturationValue) *  (greenValue * (1 - maxMask / 255.0) + colorTmp[1] * (maxMask / 255.0));
            out[j+0] = redValue*(1-saturationValue) +  (saturationValue) *  (redValue * (1 - maxMask / 255.0) + colorTmp[2]* (maxMask / 255.0));
            if (channel == 4) {
                out[j+3] = in[j+3];
            }
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 * 饱和度调节
 * @param image
 * @param adjust[-1,1]
 */
void AdjustSatuation(cv::Mat &image, float adjust,Mat mask)
{
    if (adjust < -1){
        adjust = -1;
    }else if (adjust > 1){
        adjust = 1;
    }
    int width = image.cols, height = image.rows;
    for (int i = 0; i < height; ++i)
    {
        auto data = image.ptr<cv::Vec3b>(i);
        auto maskIn = mask.ptr<cv::Vec3b>(i);
        for (int j = 0; j < width; ++j)
        {
            uchar b = data[j][0];
            uchar g = data[j][1];
            uchar r = data[j][2];

            uchar maskTmpB = maskIn[j][0];
            uchar maskTmpG = maskIn[j][1];
            uchar maskTmpR = maskIn[j][2];

            uchar maxMask = MAX(maskTmpR, MAX(maskTmpG, maskTmpB));
            if (maxMask == 0)
            {
                data[j][0] = b;
                data[j][1] = g;
                data[j][2] = r;
                continue;
            }

            float lum = data[j][0] * 0.299f + data[j][1] * 0.587f + data[j][2] * 0.114f;
            float maskB = std::max(0.0f, std::min(data[j][0] - lum, 255.0f)) / 255.0f;
            float maskG = std::max(0.0f, std::min(data[j][1] - lum, 255.0f)) / 255.0f;
            float maskR = std::max(0.0f, std::min(data[j][2] - lum, 255.0f)) / 255.0f;
            float lumMask = (1.0f - (maskB * 0.299f + maskG * 0.587f + maskR * 0.114f)) * adjust;

            data[j][0] = b * (1 - maxMask / 255.0) + (cv::saturate_cast<uchar>(data[j][0] * (1.0f + lumMask) - lum * lumMask))*maxMask / 255.0;
            data[j][1] = g * (1 - maxMask / 255.0) + (cv::saturate_cast<uchar>(data[j][1] * (1.0f + lumMask) - lum * lumMask))*maxMask / 255.0;
            data[j][2] = r * (1 - maxMask / 255.0) + (cv::saturate_cast<uchar>(data[j][2] * (1.0f + lumMask) - lum * lumMask))*maxMask / 255.0;
        }
    }
}

/**
 * 读取平均亮度【0-255】
 * @param env
 * @param clazz
 * @param bitmap
 * @param mask
 * @return
 */
jint GetHsbAverageValue(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    long average = 0;
    long number = 1;

    int channel = image.channels();
    int widht = channel * image.cols;
    int height = image.rows;
    Mat result(image.size(), image.type());
    for (int i = 0; i < height; i++) {
        uchar *in = image.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            uchar max = MAX(in[j + 0], MAX(in[j + 1], in[j + 2]));
            uchar min = MIN(in[j + 0], MIN(in[j + 1], in[j + 2]));
            uchar mid = in[j + 0] + in[j + 1] + in[j + 2] - min - max;
            uchar redValue = in[j + 0];
            uchar greenValue = in[j + 1];
            uchar blueValue = in[j + 2];
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                out[j + 2] = blueValue;
                out[j + 1] = greenValue;
                out[j + 0] = redValue;
                if (channel == 4) {
                    out[j + 3] = in[j + 3];
                }
                continue;
            }
            average = average + MAX(redValue, MAX(greenValue,blueValue));
            number++;
        }
    }
    image.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return average/number;
}

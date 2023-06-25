//
// Created by libowu on 4/23/22.
//

#include "HsbNew.h"
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
#include "include/opencv2/utils/Commen.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

void change(uchar r, uchar g, uchar b, uchar &resultR, uchar &resultG, uchar &resultB, int colorCenter, int h, int minLength, int redLength, float adjust);
float changeHDetailH(float currentH, float colorCenter, int minLength, int redLength, float adjust, float realH, float realS, float realB);
void changeH(float adjustT,int length,int &tmpH);
void changeB(float adjustT,int length,int &tmpB,int tmpH);
void changeS(float adjustT,int length,int &tmpS);
unordered_map<int,int> HashmapH;
unordered_map<int,int> HashmapS;
unordered_map<int,int> HashmapB;

//需要调节的饱和度值[-1,1]
jobject startChangeS(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jfloat redAdjust,jfloat greenAdjust,jfloat blueAdjust,jfloat cyanAdjust,jfloat yellowAdjust,jfloat carmineAdjust){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img,img,CV_RGBA2BGR);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,CV_RGBA2BGR);


    Mat result(img.size(), img.type());
    cvtColor(result,result,CV_RGBA2BGR);
    int width = img.cols * img.channels();
    int height = img.rows;
    uchar channel = img.channels();
    for (int i = 0; i < height; i++)
    {
        uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j += channel)
        {
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];
            uchar max = MAX(r, MAX(g, b));
            uchar min = MIN(r, MIN(g, b));

            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                out[j + 2] = b;
                out[j + 1] = g;
                out[j] = r;
                continue;
            }

            short h = 0;
            if ((r == g && g == b))
            {
                out[j + 2] = b;
                out[j + 1] = g;
                out[j] = r;
                continue;
            }
            if (max == min)
            {
                h = 0;
            }
            else if (max == r && g >= b)
            {
                h = 60 * ((g - b) / (max - min * 1.0));
            }
            else if (max == r && g < b)
            {
                h = 60 * ((g - b) / (max - min * 1.0)) + 360;
            }
            else if (max == g)
            {
                h = 60 * ((b - r) / (max - min * 1.0)) + 120;
            }
            else if (max == b)
            {
                h = 60 * ((r - g) / (max - min * 1.0)) + 240;
            }
            else
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            float adjust = 0;
            //颜色的标准色相值，红色0,蓝色210,绿色120，黄色60，青色180，洋红300
            short redLength = 0;
            short colorCenter;
            if (45 - h > 0)
            {
                redLength = h;
            }
            else
            {
                redLength = 360 - h;
            }
            short greenLength = abs(120 - h);
            short blueLength = abs(240 - h);
            short yellowLength = abs(60 - h);
            short cyanLength = abs(180 - h);
            short carmineLength = abs(300 - h);
            //计算距离村色的最小距离是哪个
            int minLength = MIN(redLength, MIN(greenLength, MIN(blueLength, MIN(yellowLength, MIN(cyanLength, carmineLength)))));
            uchar resultR = r;
            uchar resultG = g;
            uchar resultB = b;
            if (h == 30)
            {
                int a = 0;
            }
            //通过与纯色最小距离，判断当前像素属于哪个颜色，并设置纯色值
            if (minLength == redLength)
            {
                colorCenter = 0;
                adjust = redAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 0, h, minLength, redLength, adjust);
                if (abs(300 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 300, 300, minLength, -1, carmineAdjust * (1 - carmineLength / 60.0));
                }
                if (abs(60 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 60, 60, minLength, -1, yellowAdjust * (1 - yellowLength / 60.0));
                }
            }
            else if (minLength == greenLength)
            {
                colorCenter = 120;
                adjust = greenAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 120, h, minLength, redLength, adjust);
                if (abs(180 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 180, 180, minLength, -1, cyanAdjust * (1 - cyanLength / 60.0));
                }
                if (abs(60 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 60, 60, minLength, -1, yellowAdjust * (1 - yellowLength / 60.0));
                }
            }
            else if (minLength == blueLength)
            {
                colorCenter = 240;
                adjust = blueAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 240, h, minLength, redLength, adjust);
                if (abs(180 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 180, 180, minLength, -1, cyanAdjust * (1 - cyanLength / 60.0));
                }
                if (abs(300 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 300, 300, minLength, -1, carmineAdjust * (1 - carmineLength / 60.0));
                }
            }
            else if (minLength == yellowLength)
            {
                colorCenter = 60;
                adjust = yellowAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 60, h, minLength, redLength, adjust);
                if (abs(120 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 120, 120, minLength, -1, greenAdjust * (1 - greenLength / 60.0));
                }
                if (abs(0 - h) <= 60 || abs(360 - h) <= 60)
                {
                    if (h >= 300)
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, redAdjust * (1 - abs(360 - h) / 60.0));
                    }
                    else
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, redAdjust * (1 - abs(0 - h) / 60.0));
                    }
                }
            }
            else if (minLength == carmineLength)
            {
                colorCenter = 300;
                adjust = carmineAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 300, h, minLength, redLength, adjust);
                if (abs(0 - h) <= 60 || abs(360 - h) <= 60)
                {
                    if (h >= 300)
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, redAdjust * (1 - abs(360 - h) / 60.0));
                    }
                    else
                    {
                        if (abs(0 - h) / 60.0 > 1)
                        {
                            int a = 0;
                        }
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, redAdjust * (1 - abs(0 - h) / 60.0));
                    }
                }
                if (abs(240 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 240, 240, minLength, -1, blueAdjust * (1 - blueLength / 60.0));
                }
            }
            else if (minLength == cyanLength)
            {
                colorCenter = 180;
                adjust = cyanAdjust;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 180, h, minLength, redLength, adjust);
                if (abs(240 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 240, 240, minLength, -1, blueAdjust * (1 - blueLength / 60.0));
                }
                if (abs(120 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 120, 120, minLength, -1, greenAdjust * (1 - greenLength / 60.0));
                }
            }

            out[j + 2] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
            out[j] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    img.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}


float guoduValue = 60.0;

jobject startChangeH(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jfloat redAdjust,jfloat greenAdjust,jfloat blueAdjust,jfloat cyanAdjust,jfloat yellowAdjust,jfloat carmineAdjust)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cvtColor(img,img,CV_RGBA2BGR);
    cvtColor(maskMt,maskMt,CV_RGBA2BGR);
    Mat result(img.size(), img.type());
    int width = img.cols * img.channels();
    int height = img.rows;
    uchar channel = img.channels();
    short redCenter = 0;
    short greenCenter = 120;
    short blueCenter = 240;
    short cyanCenter = 180;
    short yellowCenter = 60;
    short carmineCenter = 300;
    short tmpRedLength = 255;
    short tmpGreenLength = 255;
    short tmpBlueLength = 255;
    short tmpYellowLength = 255;
    short tmpCyanLength = 255;
    short tmpCarmineLength = 255;
    float tmpRedS = 0;
    float tmpGreenS = 0;
    float tmpBlueS = 0;
    float tmpCyanS = 0;
    float tmpYellowS = 0;
    float tmpCarmineS = 0;

    for (int i = 0; i < height; i++)
    {
        uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j += channel)
        {
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];

            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }

            uchar max = MAX(r, MAX(g, b));
            uchar min = MIN(r, MIN(g, b));
            short h = 0;
            if ((r == g && g == b))
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            uchar bgr[] = {b, g, r};
            float hsb[] = {0, 0, 0};
            BGR2HSB(bgr, hsb);
            h = hsb[0];
            float adjust = 0;
            //颜色的标准色相值，红色0,蓝色210,绿色120，黄色60，青色180，洋红300
            short redLength = 0;
            short colorCenter;
            if (45 - h > 0)
            {
                redLength = h;
            }
            else
            {
                redLength = 360 - h;
            }
            short greenLength = abs(120 - h);
            short blueLength = abs(240 - h);
            short yellowLength = abs(60 - h);
            short cyanLength = abs(180 - h);
            short carmineLength = abs(300 - h);
            //计算距离村色的最小距离是哪个
            int minLength = MIN(redLength, MIN(greenLength, MIN(blueLength, MIN(yellowLength, MIN(cyanLength, carmineLength)))));
            uchar resultR = r;
            uchar resultG = g;
            uchar resultB = b;
            if (h == 294)
            {
                int a = 0;
            }
            if (h == 30)
            {
                int b = 0;
            }
            //通过与纯色最小距离，判断当前像素属于哪个颜色，并设置纯色值
            if (minLength == redLength)
            {
                colorCenter = redCenter;
                adjust = redAdjust;
                h = changeHDetailH(h, redCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(carmineCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], carmineCenter, minLength, -1, carmineAdjust * (1 - abs(300 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(yellowCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], yellowCenter, minLength, -1, yellowAdjust * (1 - abs(60 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == greenLength)
            {
                colorCenter = greenCenter;
                adjust = greenAdjust;
                h = changeHDetailH(h, greenCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(cyanCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], cyanCenter, minLength, -1, cyanAdjust * (1 - abs(180 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(yellowCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], yellowCenter, minLength, -1, yellowAdjust * (1 - abs(60 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == blueLength)
            {
                colorCenter = blueCenter;
                adjust = blueAdjust;
                h = changeHDetailH(h, blueCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(cyanCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], cyanCenter, minLength, -1, cyanAdjust * (1 - abs(180 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(carmineCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], carmineCenter, minLength, -1, carmineAdjust * (1 - abs(300 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == yellowLength)
            {
                colorCenter = yellowCenter;
                adjust = yellowAdjust;
                h = changeHDetailH(h, yellowCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(greenCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], greenCenter, minLength, -1, greenAdjust * (1 - abs(120 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(redCenter - hsb[0]) <= 60 || abs(360 - h) <= guoduValue)
                {
                    if (hsb[0] >= 300)
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, redAdjust * (1 - abs(360 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                    else
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, redAdjust * (1 - abs(redCenter - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                }
            }
            else if (minLength == carmineLength)
            {
                colorCenter = carmineCenter;
                adjust = carmineAdjust;
                h = changeHDetailH(h, carmineCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);

                if (abs(redCenter - hsb[0]) <= guoduValue || abs(360 - hsb[0]) <= guoduValue)
                {
                    if (hsb[0] >= 300)
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, redAdjust * (1 - abs(360 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                    else
                    {
                        if (abs(0 - hsb[0]) / guoduValue > 1)
                        {
                            int a = 0;
                        }
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, redAdjust * (1 - abs(redCenter - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                }
                if (abs(blueCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], blueCenter, minLength, -1, blueAdjust * (1 - abs(240 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == cyanLength)
            {
                colorCenter = cyanCenter;
                adjust = cyanAdjust;
                h = changeHDetailH(h, cyanCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(blueCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], blueCenter, minLength, -1, blueAdjust * (1 - abs(240 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(greenCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], greenCenter, minLength, -1, greenAdjust * (1 - abs(120 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            hsb[0] = h;
            HSB2BGR(hsb, bgr);

            out[j + 2] = r * (1 - maxMask / 255.0) + bgr[2] * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + bgr[1] * maxMask / 255.0;
            out[j] = b * (1 - maxMask / 255.0) + bgr[0] * maxMask / 255.0;
        }
    }
    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    img.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

jobject startChangeHsb(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,HueChange hueData)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cvtColor(img,img,CV_RGBA2BGR);
    cvtColor(maskMt,maskMt,CV_RGBA2BGR);
    Mat result(img.size(), img.type());
    int width = img.cols * img.channels();
    int height = img.rows;
    uchar channel = img.channels();
    short redCenter = 0;
    short greenCenter = 120;
    short blueCenter = 240;
    short cyanCenter = 180;
    short yellowCenter = 60;
    short carmineCenter = 300;
    short tmpRedLength = 255;
    short tmpGreenLength = 255;
    short tmpBlueLength = 255;
    short tmpYellowLength = 255;
    short tmpCyanLength = 255;
    short tmpCarmineLength = 255;
    float tmpRedS = 0;
    float tmpGreenS = 0;
    float tmpBlueS = 0;
    float tmpCyanS = 0;
    float tmpYellowS = 0;
    float tmpCarmineS = 0;

    for (int i = 0; i < height; i++)
    {
        uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j += channel)
        {
            uchar redValue = in[2];
            uchar greenValue = in[1];
            uchar blueValue = in[0];
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];

            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }

            uchar max = MAX(r, MAX(g, b));
            uchar min = MIN(r, MIN(g, b));
            short h = 0;
            if ((r == g && g == b))
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            uchar bgr[] = {b, g, r};
            float hsb[] = {0, 0, 0};
            BGR2HSB(bgr, hsb);
            h = hsb[0];
            float adjust = 0;
            //颜色的标准色相值，红色0,蓝色210,绿色120，黄色60，青色180，洋红300
            short redLength = 0;
            short colorCenter;
            if (45 - h > 0)
            {
                redLength = h;
            }
            else
            {
                redLength = 360 - h;
            }
            short greenLength = abs(120 - h);
            short blueLength = abs(240 - h);
            short yellowLength = abs(60 - h);
            short cyanLength = abs(180 - h);
            short carmineLength = abs(300 - h);
            //计算距离村色的最小距离是哪个
            int minLength = MIN(redLength, MIN(greenLength, MIN(blueLength, MIN(yellowLength, MIN(cyanLength, carmineLength)))));
            uchar resultR = r;
            uchar resultG = g;
            uchar resultB = b;
            if (h == 294)
            {
                int a = 0;
            }
            if (h == 30)
            {
                int b = 0;
            }
            //通过与纯色最小距离，判断当前像素属于哪个颜色，并设置纯色值
            if (minLength == redLength)
            {
                colorCenter = redCenter;
                adjust = hueData.rH;
                h = changeHDetailH(h, redCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(carmineCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], carmineCenter, minLength, -1, hueData.mH * (1 - abs(300 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(yellowCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], yellowCenter, minLength, -1, hueData.yH * (1 - abs(60 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == greenLength)
            {
                colorCenter = greenCenter;
                adjust = hueData.gH;
                h = changeHDetailH(h, greenCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(cyanCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], cyanCenter, minLength, -1, hueData.cH * (1 - abs(180 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(yellowCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], yellowCenter, minLength, -1, hueData.yH * (1 - abs(60 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == blueLength)
            {
                colorCenter = blueCenter;
                adjust = hueData.bH;
                h = changeHDetailH(h, blueCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(cyanCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], cyanCenter, minLength, -1, hueData.cH * (1 - abs(180 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(carmineCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], carmineCenter, minLength, -1, hueData.mH * (1 - abs(300 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == yellowLength)
            {
                colorCenter = yellowCenter;
                adjust = hueData.yH;
                h = changeHDetailH(h, yellowCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(greenCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], greenCenter, minLength, -1, hueData.gH * (1 - abs(120 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(redCenter - hsb[0]) <= 60 || abs(360 - h) <= guoduValue)
                {
                    if (hsb[0] >= 300)
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, hueData.rH * (1 - abs(360 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                    else
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, hueData.rH * (1 - abs(redCenter - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                }
            }
            else if (minLength == carmineLength)
            {
                colorCenter = carmineCenter;
                adjust = hueData.mH;
                h = changeHDetailH(h, carmineCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);

                if (abs(redCenter - hsb[0]) <= guoduValue || abs(360 - hsb[0]) <= guoduValue)
                {
                    if (hsb[0] >= 300)
                    {
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, hueData.rH * (1 - abs(360 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                    else
                    {
                        if (abs(0 - hsb[0]) / guoduValue > 1)
                        {
                            int a = 0;
                        }
                        h = changeHDetailH(hsb[0], redCenter, minLength, redLength, hueData.rH * (1 - abs(redCenter - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                    }
                }
                if (abs(blueCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], blueCenter, minLength, -1, hueData.bH * (1 - abs(240 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            else if (minLength == cyanLength)
            {
                colorCenter = cyanCenter;
                adjust = hueData.cH;
                h = changeHDetailH(h, cyanCenter, minLength, redLength, adjust, h, hsb[1], hsb[2]);
                if (abs(blueCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], blueCenter, minLength, -1, hueData.bH * (1 - abs(240 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
                if (abs(greenCenter - hsb[0]) <= guoduValue)
                {
                    h = changeHDetailH(hsb[0], greenCenter, minLength, -1, hueData.gH * (1 - abs(120 - hsb[0]) / guoduValue), h, hsb[1], hsb[2]);
                }
            }
            hsb[0] = h;
            HSB2BGR(hsb, bgr);
            resultR = bgr[2];
            resultG = bgr[1];
            resultB = bgr[0];
            //==================修改饱和度========================
            max = MAX(r, MAX(g, b));
            min = MIN(r, MIN(g, b));
            h = 0;
            if ((r == g && g == b))
            {
                out[j + 2] = b;
                out[j + 1] = g;
                out[j] = r;
                continue;
            }
            if (max == min)
            {
                h = 0;
            }
            else if (max == r && g >= b)
            {
                h = 60 * ((g - b) / (max - min * 1.0));
            }
            else if (max == r && g < b)
            {
                h = 60 * ((g - b) / (max - min * 1.0)) + 360;
            }
            else if (max == g)
            {
                h = 60 * ((b - r) / (max - min * 1.0)) + 120;
            }
            else if (max == b)
            {
                h = 60 * ((r - g) / (max - min * 1.0)) + 240;
            }
            else
            {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            adjust = 0;
            //颜色的标准色相值，红色0,蓝色210,绿色120，黄色60，青色180，洋红300
            redLength = 0;
            colorCenter;
            if (45 - h > 0)
            {
                redLength = h;
            }
            else
            {
                redLength = 360 - h;
            }
            greenLength = abs(120 - h);
            blueLength = abs(240 - h);
            yellowLength = abs(60 - h);
            cyanLength = abs(180 - h);
            carmineLength = abs(300 - h);
            //计算距离村色的最小距离是哪个
            minLength = MIN(redLength, MIN(greenLength, MIN(blueLength, MIN(yellowLength, MIN(cyanLength, carmineLength)))));
            resultR = r;
            resultG = g;
            resultB = b;
            if (h == 30)
            {
                int a = 0;
            }
            //通过与纯色最小距离，判断当前像素属于哪个颜色，并设置纯色值
            if (minLength == redLength)
            {
                colorCenter = 0;
                adjust = hueData.rS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 0, h, minLength, redLength, adjust);
                if (abs(300 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 300, 300, minLength, -1, hueData.mS * (1 - carmineLength / 60.0));
                }
                if (abs(60 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 60, 60, minLength, -1, hueData.yS * (1 - yellowLength / 60.0));
                }
            }
            else if (minLength == greenLength)
            {
                colorCenter = 120;
                adjust = hueData.gS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 120, h, minLength, redLength, adjust);
                if (abs(180 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 180, 180, minLength, -1, hueData.cS * (1 - cyanLength / 60.0));
                }
                if (abs(60 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 60, 60, minLength, -1, hueData.yS * (1 - yellowLength / 60.0));
                }
            }
            else if (minLength == blueLength)
            {
                colorCenter = 240;
                adjust = hueData.bS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 240, h, minLength, redLength, adjust);
                if (abs(180 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 180, 180, minLength, -1, hueData.cS * (1 - cyanLength / 60.0));
                }
                if (abs(300 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 300, 300, minLength, -1, hueData.mS * (1 - carmineLength / 60.0));
                }
            }
            else if (minLength == yellowLength)
            {
                colorCenter = 60;
                adjust = hueData.yS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 60, h, minLength, redLength, adjust);
                if (abs(120 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 120, 120, minLength, -1, hueData.gS * (1 - greenLength / 60.0));
                }
                if (abs(0 - h) <= 60 || abs(360 - h) <= 60)
                {
                    if (h >= 300)
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, hueData.rS * (1 - abs(360 - h) / 60.0));
                    }
                    else
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, hueData.rS * (1 - abs(0 - h) / 60.0));
                    }
                }
            }
            else if (minLength == carmineLength)
            {
                colorCenter = 300;
                adjust = hueData.mS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 300, h, minLength, redLength, adjust);
                if (abs(0 - h) <= 60 || abs(360 - h) <= 60)
                {
                    if (h >= 300)
                    {
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, hueData.rS * (1 - abs(360 - h) / 60.0));
                    }
                    else
                    {
                        if (abs(0 - h) / 60.0 > 1)
                        {
                            int a = 0;
                        }
                        change(resultR, resultG, resultB, resultR, resultG, resultB, 0, 0, minLength, redLength, hueData.rS * (1 - abs(0 - h) / 60.0));
                    }
                }
                if (abs(240 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 240, 240, minLength, -1, hueData.bS * (1 - blueLength / 60.0));
                }
            }
            else if (minLength == cyanLength)
            {
                colorCenter = 180;
                adjust = hueData.cS;
                change(resultR, resultG, resultB, resultR, resultG, resultB, 180, h, minLength, redLength, adjust);
                if (abs(240 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 240, 240, minLength, -1, hueData.bS * (1 - blueLength / 60.0));
                }
                if (abs(120 - h) <= 60)
                {
                    change(resultR, resultG, resultB, resultR, resultG, resultB, 120, 120, minLength, -1, hueData.gS * (1 - greenLength / 60.0));
                }
            }
            bgr[0] = resultB;
            bgr[1] = resultG;
            bgr[2] = resultR;
            float hsbTmp[] = {0, 0, 0};
            BGR2HSB(bgr,hsbTmp);
            hsbTmp[0] = hsb[0];
            HSB2BGR(hsbTmp,bgr);
            resultB = bgr[0];
            resultG = bgr[1];
            resultR = bgr[2];
            //==================修改饱和度End========================


            //=================修改明度=============================
            max = MAX(redValue, MAX(greenValue, blueValue));
            min = MIN(redValue, MIN(greenValue, blueValue));
            changeB(resultR, resultG, resultB, hueData.rS, hueData.rB, hueData.rH,
                    redValue, greenValue, blueValue, max, min, 2,hueData);
            if ((max == redValue || max == greenValue) && min == blueValue &&
                (hueData.yH != 0 || hueData.yS != 0 || hueData.yB != 0)) {
                //黄色通道
                //cout << "y" << endl;
                if (blueValue != redValue && blueValue != greenValue) {
                    changeB(resultR, resultG, resultB, hueData.yS, hueData.yB,
                            hueData.yH, redValue, greenValue, blueValue, max, min, 1,hueData);
                }
            }
            if ((max == redValue && greenValue != redValue && blueValue != redValue &&
                 (hueData.rH != 0 || hueData.rS != 0 || hueData.rB != 0))) {
                //红色通道
                //cout << "r" << endl;
                changeB(resultR, resultG, resultB, hueData.rS, hueData.rB, hueData.rH,
                        redValue, greenValue, blueValue, max, min, 2,hueData);
            }
            if (max == blueValue && (hueData.bH != 0 || hueData.bS != 0 || hueData.bB != 0)) {
                //蓝色通道
                //cout << "b" << endl;
                changeB(resultR, resultG, resultB, hueData.bS, hueData.bB, hueData.bH,
                        redValue, greenValue, blueValue, max, min, 3,hueData);
            }
            if (max == greenValue && (hueData.gH != 0 || hueData.gS != 0 || hueData.gB != 0)) {
                //绿色通道
                //cout << "g" << endl;
                changeB(resultR, resultG, resultB, hueData.gS, hueData.gB, hueData.gH,
                        redValue, greenValue, blueValue, max, min, 4,hueData);
            }
            if ((max == blueValue || max == greenValue) && min == redValue &&
                (hueData.cH != 0 || hueData.cS != 0 || hueData.cB != 0)) {
                //青色
                //cout << "c" << endl;
                if (redValue != blueValue && redValue != greenValue) {
                    changeB(resultR, resultG, resultB, hueData.cS, hueData.cB,
                            hueData.cH, redValue, greenValue, blueValue, max, min, 5,hueData);
                }
            }
            if (min == greenValue && (hueData.mH != 0 || hueData.mS != 0 || hueData.mB != 0)) {
                //洋红
                //cout << "m" << endl;
                if (redValue != greenValue && blueValue != greenValue) {
                    changeB(resultR, resultG, resultB, hueData.mS, hueData.mB,
                            hueData.mH, redValue, greenValue, blueValue, max, min, 6,hueData);
                }
            }
            //=================修改明度End=============================

            out[j + 2] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
            out[j] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
        }
    }
    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    img.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

float changeHDetailH(float currentH, float colorCenter, int minLength, int redLength, float adjust, float realH, float realS, float realB)
{
    if (adjust == 0)
    {
        return realH;
    }
    //计算距离村色的距离
    int length = abs(currentH - colorCenter);
    if (colorCenter == 0 && currentH > 0 && currentH <= 200)
    {
        length = currentH;
    }
    else if (colorCenter == 0 && currentH > 200 && currentH <= 360)
    {
        length = 360 - currentH;
    }
    else
    {
        length = abs(currentH - colorCenter);
    }

    if (length > guoduValue)
    {
        length = guoduValue;
    }
    int le = abs(guoduValue - length);
    if (le > guoduValue)
    {
        le = guoduValue;
    }
    if (le == 0)
    {
        return realH;
    }
    if (adjust < 0)
    {
        int hValue = 60 * (pow(abs(adjust * (4 / 4.0) * 10), le / guoduValue) / 10.0);
        // int sValue = 90 * (pow(abs(adjust * (1 / 4.0) * 10), realS / 1.0) / 10.0);
        // int bValue = 90 * (pow(abs(adjust * (1 / 4.0) * 10), abs(0.5 - realB) / 0.5) / 10.0);
        int cut = hValue /* + sValue + bValue*/;
        if (realH - cut < 0)
        {
            adjust = 360 - abs(realH - cut);
        }
        else
        {
            adjust = realH - cut;
        }
    }
    else
    {
        int hValue = 60 * (pow(abs(adjust * (4 / 4.0) * 10), le / guoduValue) / 10.0);
        // int sValue = 90 * (pow(abs(adjust * (1 / 4.0) * 10), realS / 1.0) / 10.0);
        // int bValue = 90 * (pow(abs(adjust * (1 / 4.0) * 10), abs(0.5 - realB) / 0.5) / 10.0);
        int cut = hValue /* + sValue + bValue*/;
        if (realH + cut > 360)
        {
            adjust = (realH + cut) - 360;
        }
        else
        {
            adjust = realH + cut;
        }
    }
    if (adjust >= 360)
    {
        adjust = adjust - 360;
    }
    else if (adjust < 0)
    {
        adjust = 360 + adjust;
    }
    return adjust;
}

void change(uchar r, uchar g, uchar b, uchar &resultR, uchar &resultG, uchar &resultB, int colorCenter, int h, int minLength, int redLength, float adjust)
{
    if (adjust == 0)
    {
        return;
    }
    //计算距离村色的距离
    int length = abs(h - colorCenter);
    if (minLength == redLength && 45 - h > 0)
    {
        length = h;
    }
    else if (minLength == redLength && 45 - h < 0)
    {
        length = 360 - h;
    }
    else
    {
        length = abs(h - colorCenter);
    }

    if (length > 60)
    {
        length = 60;
    }
    int sign = 1;
    if (adjust < 0)
    {
        sign = -1;
    }
    adjust = pow(abs(adjust) * 10, exp((60 - length) / 60.0) / 2.7) / 10.0;
    float lum = b * 0.299f + g * 0.587f + r * 0.114f;
    float maskB = std::max(0.0f, std::min(b - lum, 255.0f)) / 255.0f;
    float maskG = std::max(0.0f, std::min(g - lum, 255.0f)) / 255.0f;
    float maskR = std::max(0.0f, std::min(r - lum, 255.0f)) / 255.0f;
    float lumMask = (1.0f - (maskB * 0.299f + maskG * 0.587f + maskR * 0.114f)) * adjust * sign;
    resultB = cv::saturate_cast<uchar>(b * (1.0f + lumMask) - lum * lumMask);
    resultG = cv::saturate_cast<uchar>(g * (1.0f + lumMask) - lum * lumMask);
    resultR = cv::saturate_cast<uchar>(r * (1.0f + lumMask) - lum * lumMask);
}


jobject testHsb(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,HueChange changeData){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img,img,CV_RGBA2BGR);
    Mat result(img.size(), img.type());
    cvtColor(img,img,CV_BGR2HSV_FULL);
    int width = img.cols * img.channels();
    int height = img.rows;
    uchar channel = img.channels();
    cvtColor(result,result,CV_BGR2HSV_FULL);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,CV_RGBA2BGR);

    int red = 360;
    int orange = 45;
    int yellow = 90;
    int green = 135;
    int cyan = 180;
    int blue = 225;
    int purple = 270;
    int carmine = 315;
    for (int i = 0; i < height; i++)
    {
        uchar *in = img.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        uchar *maskIn = maskMt.ptr<uchar>(i);
        for (int j = 0; j < width; j += channel)
        {
            int h = in[j]/255.0*360;
            int b = in[j+2]/255.0*360;
            int s = in[j+1]/255.0*360;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                out[j + 2] = in[j+2];
                out[j + 1] = in[j+1];
                out[j] = in[j];
                continue;
            }

            short redLength = 0;
            //LOGI("数值为：%d",h);
            if (180 - h > 0)
            {
                redLength = h;
            }
            else
            {
                redLength = 360 - h;
            }
            short greenLength = abs(135 - h);
            short orangeLength = abs(45 - h);
            short blueLength = abs(225 - h);
            short yellowLength = abs(90 - h);
            short cyanLength = abs(180 - h);
            short purpleLength = abs(270 - h);
            short carmineLength = abs(315 - h);
            short colorCenter;
            //计算距离村色的最小距离是哪个
            int minLength = MIN(redLength, MIN(greenLength, MIN(blueLength, MIN(yellowLength, MIN(cyanLength, MIN(orangeLength, MIN(purpleLength,carmineLength)))))));
            float percentage = in[j+1]/255.0;
            //=======================修改色相===========================
            changeH(changeData.rH*percentage,redLength,h);
            changeH(changeData.gH*percentage,greenLength,h);
            changeH(changeData.bH*percentage,blueLength,h);
            changeH(changeData.cH*percentage,cyanLength,h);
            changeH(changeData.yH*percentage,yellowLength,h);
            changeH(changeData.mH*percentage,carmineLength,h);
            changeH(changeData.oH*percentage,orangeLength,h);
            changeH(changeData.pH*percentage,purpleLength,h);
            //LOGI("原始数据：%d,修改后的数据：%d",in[j]/255.0*360,h);
            //=======================修改色相End===========================

            //=======================修改饱和度===========================
            changeS(changeData.rS*percentage,redLength,s);
            changeS(changeData.gS*percentage,greenLength,s);
            changeS(changeData.bS*percentage,blueLength,s);
            changeS(changeData.cS*percentage,cyanLength,s);
            changeS(changeData.yS*percentage,yellowLength,s);
            changeS(changeData.mS*percentage,carmineLength,s);
            changeS(changeData.oS*percentage,orangeLength,s);
            changeS(changeData.pS*percentage,purpleLength,s);
            //=======================修改饱和度End===========================

            //=======================修改明度===========================
            changeB(changeData.rB*percentage,redLength,b,h);
            changeB(changeData.gB*percentage,greenLength,b,h);
            changeB(changeData.bB*percentage,blueLength,b,h);
            changeB(changeData.cB*percentage,cyanLength,b,h);
            changeB(changeData.yB*percentage,yellowLength,b,h);
            changeB(changeData.mB*percentage,carmineLength,b,h);
            changeB(changeData.oB*percentage,orangeLength,b,h);
            changeB(changeData.pB*percentage,purpleLength,b,h);

            if(changeData.rB !=0 || changeData.oB !=0 || changeData.yB !=0 || changeData.gB !=0 || changeData.cB !=0 || changeData.bB !=0 || changeData.mB !=0 || changeData.pB !=0){
                h = (int)(h/360.0*255);
                s = (int)(s/360.0*255);
                b = (int)(b/360.0*255);
                out[j + 2] = in[j+2] * (1 - maxMask / 255.0) + b * (maxMask / 255.0);
                out[j + 1] = in[j+1] * (1 - maxMask / 255.0) + s * (maxMask / 255.0);
                out[j] = in[j] * (1 - maxMask / 255.0) + h * (maxMask / 255.0);
            }else{
                h = (int)(h/360.0*255);
                s = (int)(s/360.0*255);
                b = (int)(b/360.0*255);
                out[j + 2] = in[j+2] * (1 - maxMask / 255.0) + b * (maxMask / 255.0);
                out[j + 1] = in[j+1] * (1 - maxMask / 255.0) + s * (maxMask / 255.0);
                out[j] = in[j] * (1 - maxMask / 255.0) + h * (maxMask / 255.0);
            }
        }
    }
    cvtColor(result,result,CV_HSV2BGR_FULL);
    cvtColor(result,result,CV_BGR2RGBA);
    img.release();
    maskMt.release();
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    result.release();
    return resultBitmap;
}

/**
 * 修改色相
 * @param adjustT 修改强度
 * @param length 距离定义纯色的距离
 * @param tmpH 当前像色的色相
 * @return 色相
 */
void changeH(float adjustT,int length,int &tmpH) {
    if (adjustT == 0) {
        return;
    }
    float adjust = adjustT;
    int t = 60 - length;
    if (t < 0) {
        t = 0;
    }
    //adjust = pow(abs(adjust), exp(a));
    //使用的是生长曲线函数
    int change =  change = 1 / ((1 / 45.0) + 1.1 * pow(1.1, -t));
    int h = tmpH + change * adjustT;
    if (h > 360){
        h = h - 360;
    } else if (h < 0){
        h = 360 - abs(h);
    }
    tmpH = h;
}

/**
 * 修改明度
 * @param adjustT 修改强度
 * @param length 距离定义纯色的距离
 * @param tmpS 当前明度
 * @return 明度
 */
void changeB(float adjustT,int length,int &tmpB,int tmpH){
    if (adjustT == 0){
        return;
    }
    float adjust = adjustT;
    int t = 60 - length;
    if (t < 0){
        t = 0;
    }
    int change = 0;
    change = 1/((1/125.0)+1.1*pow(1.2,-t));
    //LOGI("改变数据为：%d",change);
    int b = tmpB + change*adjustT;
    if (b > 360){
        b = 360;
    } else if (b < 0){
        b = 0;
    }
    tmpB = b;
}

/**
 * 修改饱和度
 * @param adjustT 修改强度
 * @param length 距离定义纯色的距离
 * @param tmpS 当前明度
 * @return 明度
 */
void changeS(float adjustT,int length,int &tmpS){
    if (adjustT == 0){
        return;
    }
    float adjust = adjustT;
    int t = 60 - length;
    if (t < 0){
        t = 0;
    }
    int change = 0;
    change = 1/((1/125.0)+1.1*pow(1.2,-t));
    int s = tmpS + change*adjustT;
    if (s > 360){
        s = 360;
    } else if (s < 0){
        s = 0;
    }
    tmpS = s;
}
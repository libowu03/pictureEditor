//
// Created by libowu on 6/3/22.
//

#include "Mask.h"
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <opencv2/utils/utils.h>
#include <opencv2/hsl/ColorSpace.hpp>

using namespace std;
using namespace cv;
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

jobject makeThresholdMask(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint threshold,jboolean isShowModel,jboolean isAddModel){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
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
    Mat result(img.size(),img.type());
    cvtColor(result,result,CV_RGBA2BGR);
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;

    for (int i = 0; i < height; i++)
    {
        const uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel)
        {
            uchar r = in[j+2];
            uchar g = in[j+1];
            uchar b = in[j];
            uchar max = MAX(r,MAX(g,b));
            if (max > threshold){
                if (isAddModel){
                    out[j+2] = 255;
                    out[j+1] = 255;
                    out[j+0] = 255;
                }else{
                    out[j+2] = 0;
                    out[j+1] = 0;
                    out[j+0] = 0;
                }
            }else{
                int max = MAX(maskIn[j],MAX(maskIn[j+1],maskIn[j+2]));
                if (max == 0){
                    out[j+2] = 0;
                    out[j+1] = 0;
                    out[j+0] = 0;
                }else{
                    out[j] = maskIn[j];
                    out[j + 1] = maskIn[j+1];
                    out[j + 2] = maskIn[j+2];
                }
            }
        }
    }
    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    result.release();
    img.release();
    maskMt.release();
    return resultBitmap;

}

/**
 * 将蒙版未选中区域转换为透明度
 * @param env
 * @param clazz
 * @param mask
 * @return
 */
jobject maskToAlphaMask(JNIEnv *env, jclass clazz, jobject mask){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    Mat result(img.size(),img.type());
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            uchar r = in[j+2];
            uchar g = in[j+1];
            uchar b = in[j+0];
            uchar max = MAX(r,MAX(g,b));
            out[j+2] = r;
            out[j+1] = g;
            out[j+0] = b;
            out[j+3] = max;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, false, mask);
    AndroidBitmap_unlockPixels(env, mask);
    result.release();
    img.release();
    return resultBitmap;
}

jobject makeHsbMask(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint targetR, jint targetG, jint targetB, jfloat allowExtentH, jfloat allowExtentS, jfloat allowExtentB, jboolean isShowModel,jboolean isAddModel){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
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
    Mat result(img.size(),img.type());
    cvtColor(result,result,CV_RGBA2BGR);
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;

    //色相容差值[0-360]
    float hDuration = allowExtentH;
    //明度容差值[0-1]
    float sDuration = allowExtentS;
    //饱和度容差值[0-1]
    float bDuration = allowExtentB;
    uchar targetRgbArray[] = {static_cast<uchar>(targetB), static_cast<uchar>(targetG), static_cast<uchar>(targetR)};

    float targetHsb[] = {0, 0, 0};
    BGR2HSB(targetRgbArray, targetHsb);
    for (int i = 0; i < height; i++)
    {
        const uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel)
        {
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];
            uchar rgbArray[] = {b, g, r};
            float hsb[] = {0, 0, 0};
            BGR2HSB(rgbArray, hsb);
            int lengOne = abs(hsb[0] - targetHsb[0]);
            int lengTwo = abs(MIN(hsb[0],targetHsb[0]) + (360-MAX(hsb[0],targetHsb[0])) );
            if (
                    (MIN(lengOne,lengTwo) <= hDuration) &&
                    (fabs(hsb[1] - targetHsb[1]) <= sDuration) &&
                    (fabs(hsb[2] - targetHsb[2]) <= bDuration))
            {
                if (isAddModel){
                    out[j] = 255;
                    out[j + 1] = 255;
                    out[j + 2] = 255;
                }else{
                    out[j] = 0;
                    out[j + 1] = 0;
                    out[j + 2] = 0;
                }
            }else{
                int max = MAX(maskIn[j],MAX(maskIn[j+1],maskIn[j+2]));
                if (max == 0){
                    out[j] = 0;
                    out[j + 1] = 0;
                    out[j + 2] = 0;
                }else{
                    out[j] = maskIn[j];
                    out[j + 1] = maskIn[j+1];
                    out[j + 2] = maskIn[j+2];
                }
            }
        }
    }
    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    result.release();
    img.release();
    maskMt.release();
    return resultBitmap;
}

/**
 * 将图片转换成蒙版
 * @param env
 * @param bitmap
 * @return
 */
jobject imageToMask(JNIEnv *env, jobject bitmap){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    Mat result = img.clone();
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            uchar a = in[j + 3];
            uchar b = in[j + 2];
            uchar g = in[j + 1];
            uchar r = in[j];
            if (a != 0){
                int gg = 0;
            }
            uchar max = MAX(r, MAX(g, b));
            //uchar re = a/255.0*max;
            uchar re = max;
            out[j+3] = 255;
            out[j+2] = re;
            out[j+1] = re;
            out[j] = re;
        }
    }
    cvtColor(result,result,CV_RGBA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    img.release();
    result.release();
    return resultBitmap;
}

/**
 * 将图片转换成蒙版
 * @param env
 * @param bitmap
 * @return
 */
jobject imageToMaskPro(JNIEnv *env, jobject bitmap,float pro){
    if (pro < 1){
        pro = 1;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    Mat result = img.clone();
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            uchar a = in[j + 3];
            uchar b = in[j + 2];
            uchar g = in[j + 1];
            uchar r = in[j];
            uchar max = MAX(r, MAX(g, b));
            int re = a/255.0*max*pro;
            if (re > 255){
                re = 255;
            }
            out[j+3] = 255;
            out[j+2] = re;
            out[j+1] = re;
            out[j] = re;
        }
    }
    cvtColor(result,result,CV_RGBA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    img.release();
    result.release();
    return resultBitmap;
}

/**
 * 读取RGB作为蒙版（假如像素点值为255，2，46，使用的通道是R，则最后生成的蒙版为255，255，255，如果使用的是G通道，则为：2，2，2）
 * @param channelType 需要知道的通道，0：R，1：G，2：B
 * @param bitmap
 * @param needToOvalValue 需要大于该值才被选中
 * @param isUseDefaultIfNotOver 如果没有超出部分，是否使用默认方式，不适用默认方式填充，则直接填充黑色
 * @return
 */
jobject getRGBMask(JNIEnv *env,jobject bitmap,int channelType,int needToOvalValue,bool isUseDefaultIfNotOver){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    Mat result = img.clone();
    uchar channel = img.channels();
    int width = channel*img.cols;
    int height = img.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            uchar a = in[j + 3];
            uchar b = in[j + 2];
            uchar g = in[j + 1];
            uchar r = in[j];
            uchar max = MAX(r, MAX(g, b));
            int re = a/255.0*max;
            if (re > 255){
                re = 255;
            }
            if (channelType == 0 && r >= needToOvalValue){
                re = a/255.0*r;
            }else if (channelType == 1 && g >= needToOvalValue){
                re = a/255.0*g;
            }else if (channelType == 2 && b >= needToOvalValue){
                re = a/255.0*b;
            }else if (!isUseDefaultIfNotOver){
                re = 0;
            }
            out[j+3] = 255;
            out[j+2] = re;
            out[j+1] = re;
            out[j] = re;
        }
    }
    cvtColor(result,result,CV_RGBA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    img.release();
    result.release();
    return resultBitmap;
}

/**
 * 合并两个蒙层
 * @param env
 * @param maskOne
 * @param maskTwo
 * @return
 */
jobject getMaskAdd(JNIEnv *env,jobject maskOne,jobject maskTwo){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, maskOne, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskOne, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat maskMapOne(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;
    CV_Assert(AndroidBitmap_getInfo(env, maskTwo, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                      infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskTwo, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    //原图
    cv::Mat maskMapTwo(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);

    Mat result = maskMapOne.clone();

    uchar channel = maskMapOne.channels();
    int width = channel*maskMapOne.cols;
    int height = maskMapOne.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inOne = maskMapOne.ptr<uchar>(i);
        const uchar *inTwo = maskMapTwo.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            int r = inOne[j+2]+inTwo[j+2];
            int g = inOne[j+1]+inTwo[j+1];
            int b = inOne[j]+inTwo[j];
            if(r > 255){
                r = 255;
            }
            if(g > 255){
                g = 255;
            }
            if(b > 255){
                b = 255;
            }
            out[j+3] = 255;
            out[j+2] = r;
            out[j+1] = g;
            out[j] = b;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, false, maskOne);
    AndroidBitmap_unlockPixels(env, maskOne);
    AndroidBitmap_unlockPixels(env, maskTwo);
    maskMapOne.release();
    maskMapTwo.release();
    result.release();
    result.release();
    return resultBitmap;
}

/**
 * 合并两个蒙层
 * @param env
 * @param maskOne
 * @param maskTwo
 * @return
 */
jobject getMaskCut(JNIEnv *env,jobject maskOne,jobject maskTwo){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, maskOne, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskOne, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat maskMapOne(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;
    CV_Assert(AndroidBitmap_getInfo(env, maskTwo, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskTwo, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    //原图
    cv::Mat maskMapTwo(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);

    Mat result = maskMapOne.clone();

    uchar channel = maskMapOne.channels();
    int width = channel*maskMapOne.cols;
    int height = maskMapOne.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inOne = maskMapOne.ptr<uchar>(i);
        const uchar *inTwo = maskMapTwo.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            int r = inOne[j+2]-inTwo[j+2];
            int g = inOne[j+1]-inTwo[j+1];
            int b = inOne[j]-inTwo[j];
            if(r < 0){
                r = 0;
            }
            if(g < 0){
                g = 0;
            }
            if(b < 0){
                b = 0;
            }
            out[j+3] = 255;
            out[j+2] = r;
            out[j+1] = g;
            out[j] = b;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, false, maskOne);
    AndroidBitmap_unlockPixels(env, maskOne);
    AndroidBitmap_unlockPixels(env, maskTwo);
    maskMapOne.release();
    maskMapTwo.release();
    result.release();
    result.release();
    return resultBitmap;
}

jobject KeepIntersectMask(JNIEnv *env,jobject maskOne,jobject maskTwo){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, maskOne, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskOne, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat maskMapOne(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;
    CV_Assert(AndroidBitmap_getInfo(env, maskTwo, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskTwo, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    //原图
    cv::Mat maskMapTwo(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);

    Mat result = maskMapOne.clone();

    uchar channel = maskMapOne.channels();
    int width = channel*maskMapOne.cols;
    int height = maskMapOne.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inOne = maskMapOne.ptr<uchar>(i);
        const uchar *inTwo = maskMapTwo.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            int r = inTwo[j+2];
            int g = inTwo[j+1];
            int b = inTwo[j+0];
            int r2 = inOne[j+2];
            int g2 = inOne[j+1];
            int b2 = inOne[j+0];
            int max = MAX(r, MAX(g,b));
            int max2 = MAX(r2, MAX(g2,b2));
            int resultR = 0;
            int resultG = 0;
            int resultB = 0;
            if(max != 0 && max2 != 0){
                resultR = inTwo[j+2]+inOne[j+2];
                resultG = inTwo[j+1]+inOne[j+1];
                resultB = inTwo[j]+inOne[j];
            }
            if(resultR > 255){
                resultR = 255;
            }
            if(resultG > 255){
                resultG = 255;
            }
            if(resultB > 255){
                resultB = 255;
            }
            out[j+3] = 255;
            out[j+2] = resultR;
            out[j+1] = resultG;
            out[j] = resultB;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, false, maskOne);
    AndroidBitmap_unlockPixels(env, maskOne);
    AndroidBitmap_unlockPixels(env, maskTwo);
    maskMapOne.release();
    maskMapTwo.release();
    result.release();
    result.release();
    return resultBitmap;
}

/**
 * 获取瞳孔区域蒙版
 * @param env
 * @param sourceMask
 * @param eyeMask
 * @return
 */
jobject getPupilMaskByEyeMask(JNIEnv *env,jobject sourceMask,jobject eyeMask){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, sourceMask, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, sourceMask, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat bitmapSource(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;
    CV_Assert(AndroidBitmap_getInfo(env, eyeMask, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, eyeMask, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    //原图
    cv::Mat bitmapEyeMask(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);

    Mat result = bitmapEyeMask.clone();

    uchar channel = bitmapEyeMask.channels();
    int width = channel*bitmapEyeMask.cols;
    int height = bitmapEyeMask.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inOne = bitmapSource.ptr<uchar>(i);
        const uchar *inTwo = bitmapEyeMask.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            int r = inTwo[j+2];
            int g = inTwo[j+1];
            int b = inTwo[j+0];
            int max = MAX(r, MAX(g,b));
            if(max != 255){
                out[j+2] = 0;
                out[j+1] = 0;
                out[j] = 0;
                out[j+3] = 255;
                continue;
            }
            int rS = inOne[j+2];
            int gS = inOne[j+1];
            int bS = inOne[j+0];
            int maxS = MAX(rS, MAX(gS,bS));
            if(maxS < 125){
                out[j+2] = 255;
                out[j+1] = 255;
                out[j] = 255;
                out[j+3] = 255;
            }else{
                out[j+2] = 0;
                out[j+1] = 0;
                out[j] = 0;
                out[j+3] = 255;
            }
        }
    }
    jobject resultBitmap = matToBitmap(env, result, false, eyeMask);
    AndroidBitmap_unlockPixels(env, sourceMask);
    AndroidBitmap_unlockPixels(env, eyeMask);
    bitmapSource.release();
    bitmapEyeMask.release();
    result.release();
    result.release();
    return resultBitmap;
}

/**
 * 读取蒙版的位置信息
 * 数组分别是:左，右，上，下
 */
vector<int> getMaskRect(JNIEnv *env,jobject bitmap){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    int left = -1;
    int right = -1;
    int top = -1;
    int bottom = -1;
    int channel = img.channels();
    LOGI("渠道数量为%d,宽度为：%d,高度为：%d",channel,img.rows,img.cols);
    for (int i = 0; i < img.rows; i++)
    {
        Vec4b *ptrTop = img.ptr<Vec4b>(i);
        Vec4b *ptrBottom = img.ptr<Vec4b>(img.rows - i);
        /*if(img.rows - i < i){
            break;
        }*/
        for (int j = 0; j < img.cols; j++)
        {
            /*if(j > img.cols - j){
                continue;
            }*/
            //上往下读取
            //读取左边
            bool isChooseAreaLeftTop = (ptrTop[j][0] != 0 || ptrTop[j][1] != 0 || ptrTop[j][2] != 0) && ptrTop[j][3] != 0;
            if(left == -1 && isChooseAreaLeftTop){
                left = j;
            }else if(j < left && isChooseAreaLeftTop){
                left = j;
            }
            if(top == -1 && isChooseAreaLeftTop){
                top = i;
            }
            //读取右边
            bool isChooseAreaRightTop = (ptrTop[img.cols-j][0] != 0 || ptrTop[img.cols-j][1] != 0 || ptrTop[img.cols-j][2] != 0) && ptrTop[img.cols-j][3] != 0;
            if(right == -1 && isChooseAreaRightTop){
                right = img.cols-j;
            }else if(img.cols-j > right && isChooseAreaRightTop){
                right = img.cols-j;
            }
            if(top == -1 && isChooseAreaRightTop){
                top = i;
            }

            //下往上读取
            bool isChooseAreaLeftBottom = (ptrBottom[j][0] != 0 || ptrBottom[j][1] != 0 || ptrBottom[j][2] != 0) && ptrBottom[j][3] != 0;
            if(left == -1 && isChooseAreaLeftBottom){
                left = j;
            }else if(j < left && isChooseAreaLeftBottom){
                left = j;
            }
            if(bottom == -1 && isChooseAreaLeftBottom){
                bottom = img.rows - i;
            }
            //读取右边
            bool isChooseAreaRightBottom = (ptrBottom[img.cols-j][0] != 0 || ptrBottom[img.cols-j][1] != 0 || ptrBottom[img.cols-j][2] != 0) && ptrBottom[img.cols-j][3] != 0;
            if(right == -1 && isChooseAreaRightBottom){
                right = img.cols-j;
            }else if(img.cols-j > right && isChooseAreaRightBottom){
                right = img.cols-j;
            }
            if(bottom == -1 && isChooseAreaRightBottom){
                bottom = img.rows - i;
            }
        }
    }
//    LOGI("上下左右风别为：%d,%d,%d,%d",top,bottom,left,right);
    cvtColor(img,img,CV_BGRA2RGBA);
    AndroidBitmap_unlockPixels(env, bitmap);
    img.release();
    return {left,right,top,bottom};
}
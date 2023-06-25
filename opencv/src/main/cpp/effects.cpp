//
// Created by Administrator on 2022/12/6.
//

#include "effects.h"
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
#include "include/opencv2/utils/Commen.h"

/**
 *
 * @param env
 * @param clazz
 * @param bitmap
 * @param listener
 * @return
 */
jobject imageAddParticles(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jint size) {
    int tmpSize = size;
    /*if(tmpSize > 100){
        tmpSize = 100;
    }*/
    if(tmpSize < 0){
        tmpSize = 0;
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

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    Mat result(img.size(),img.type());
    int width = img.cols * img.channels();
    int height = img.rows;
    uchar channel = img.channels();

    CvRNG cvrgn = cvRNG(cvGetTickCount());

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
                out[j + 2] = b;
                out[j + 1] = g;
                out[j] = r;
                continue;
            }

            int ran = cvRandInt(&cvrgn)%10;
            int ranTwo = cvRandInt(&cvrgn)%10;
            int resultR = r;
            int resultG = g;
            int resultB = b;
            if(ran%2 == 0){
                if(ranTwo%2 == 0){
                    resultR = r + tmpSize/100.0*50;
                    resultG = g + tmpSize/100.0*50;
                    resultB = b + tmpSize/100.0*50;
                }else{
                    resultR = r - tmpSize/100.0*50;
                    resultG = g - tmpSize/100.0*50;
                    resultB = b - tmpSize/100.0*50;
                }
            }
            if(resultR > 255){
                resultR = 255;
            }else if(resultR < 0){
                resultR = 0;
            }
            if(resultG > 255){
                resultG = 255;
            }else if(resultG < 0){
                resultG = 0;
            }
            if(resultB > 255){
                resultB = 255;
            }else if(resultB < 0){
                resultB = 0;
            }
            out[j + 2] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
            out[j] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
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
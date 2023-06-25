#include <iostream>
#include "opencv2/opencv.hpp"
#include <math.h>
#include <jni.h>
#include "opencv2/utils/Commen.h"
#include "opencv2/skin/SoftSkin.h"
#include "opencv2/skin/SkinPdf.h"
#include "opencv2//filter/GaussFilter.h"
#include "opencv2/skin/SkinDetect.h"
#include "opencv2/skin/CommenMixLayer.h"
#include <android/bitmap.h>
#include <opencv2/utils/utils.h>
#include "opencv2/lut/Lut.h"
using namespace std;
using namespace cv;

/**
 *
 * @param env
 * @param clazz
 * @param bitmap 需要进行磨皮的图片
 * @param lutMap 滤镜图片，位置是assets/Lut/SoftSkinFilter.png
 * @param ratio 磨皮强度
 * @return 处理好的图片
 */
jobject SoftSkin(JNIEnv *env, jclass clazz, jobject bitmap,jobject lutMap,jobject mask,int ratio){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    cvtColor(src,src,COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat srcMask(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(srcMask,srcMask,COLOR_RGBA2BGRA);

    AndroidBitmapInfo map;
    void *pixelsMap;
    CV_Assert(AndroidBitmap_getInfo(env, lutMap, &map) >= 0);
    CV_Assert(map.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              map.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, lutMap, &pixelsMap) >= 0);
    CV_Assert(pixelsMap);
    cv::Mat mapLut(map.height, map.width, CV_8UC4, pixelsMap);
    cvtColor(mapLut,mapLut,COLOR_RGBA2BGRA);
    SoftskinChannelMethod(src.data,src.cols,src.rows,src.step,mapLut.data,ratio,srcMask.data);
    cvtColor(src,src,COLOR_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, src, true, bitmap);
    src.release();
    mapLut.release();
    srcMask.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, lutMap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}



int SoftskinChannelMethod(unsigned char *srcData, int width, int height, int stride, unsigned char *lightMap, int ratio,unsigned char *maskB)
{
    int ret = 0;
    unsigned char *greenData = (unsigned char *)malloc(sizeof(unsigned char) * stride * height);
    unsigned char *gaussData = (unsigned char *)malloc(sizeof(unsigned char) * stride * height);
    unsigned char *curveData = (unsigned char *)malloc(sizeof(unsigned char) * stride * height);
    unsigned char *skinData = (unsigned char *)malloc(sizeof(unsigned char) * stride * height);
    unsigned char *pSrc = srcData;
    unsigned char *pGreen = greenData;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            pGreen[0] = pSrc[0];
            pGreen[1] = pSrc[0];
            pGreen[2] = pSrc[0];
            pSrc += 4;
            pGreen += 4;
        }
    }
    /*Mat tmpSrc(Size(width,height),CV_8UC4,srcData);
    vector<Mat> channels;
    cv::split(tmpSrc, channels);
    Mat tmpGreen(tmpSrc.size(),tmpSrc.type());
    channels[1] = channels[0];
    channels[2] = channels[0];
    merge(channels,tmpGreen);
    greenData = tmpGreen.data;
    pGreen = greenData;*/
//    tmpSrc.release();

    memcpy(gaussData, greenData, sizeof(unsigned char) * height * stride);
    memcpy(curveData, srcData, sizeof(unsigned char) * height * stride);
    ret = f_LUTFilter(curveData, width, height, stride, lightMap);
    Mat tmpGauss(Size(width,height),CV_8UC4,gaussData);
    Size dsize = Size(tmpGauss.cols * 0.3, tmpGauss.rows * 0.3);
    resize(tmpGauss,tmpGauss,dsize);
    float hpRadius = 10.0f * tmpGauss.cols * tmpGauss.rows / (594 * 677);
    cv::GaussianBlur(tmpGauss, tmpGauss, cv::Size(51, 51), 0, 0);
    //ret = f_FastGaussFilter(tmpGauss.data, tmpGauss.cols, tmpGauss.rows, stride, hpRadius);
    dsize = Size(width, height);
    resize(tmpGauss,tmpGauss,dsize);
    int tmpH = tmpGauss.rows;
    int tmpW = tmpGauss.cols;
    pSrc = srcData;
    free(gaussData);
    pGreen = greenData;
    unsigned char *pCurve = curveData;
    unsigned char *pGauss = tmpGauss.data;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            int t;
            t = CLIP3(pGauss[0] - pGreen[0] + 128, 0, 255);
            t = ModeSuperposition(t, t);
            t = ModeSuperposition(t, t);
            t = t * 220 / 255;
            pGreen[0] = CLIP3((pCurve[0] * t + (255 - t) * pSrc[0]) / 255, 0, 255);
            pGreen[1] = CLIP3((pCurve[1] * t + (255 - t) * pSrc[1]) / 255, 0, 255);
            pGreen[2] = CLIP3((pCurve[2] * t + (255 - t) * pSrc[2]) / 255, 0, 255);
            pGreen += 4;
            pGauss += 4;
            pSrc += 4;
            pCurve += 4;
        }
    }

    tmpGauss.release();
    memcpy(skinData, greenData, sizeof(unsigned char) * height * stride);
    int maskSmoothRadius = 3 * width * height / (594 * 677);
    ret = f_SkinPDF(skinData, width, height, stride);
    Mat tmpSkinGauss(Size(width,height),CV_8UC4,skinData);
    Size dSkinsize = Size(tmpSkinGauss.cols * 0.3, tmpSkinGauss.rows * 0.3);
    resize(tmpSkinGauss,tmpSkinGauss,dSkinsize);
    cv::GaussianBlur(tmpSkinGauss, tmpSkinGauss, cv::Size(51, 51), 0, 0);
    //ret = f_FastGaussFilter(tmpSkinGauss.data, tmpSkinGauss.cols, tmpSkinGauss.rows, stride, maskSmoothRadius);
    dSkinsize = Size(width, height);
    resize(tmpSkinGauss,tmpSkinGauss,dSkinsize);
    pGauss = tmpSkinGauss.data;
    free(skinData);
    pSrc = srcData;
    pGreen = greenData;
    int k = ratio * 128 / 100;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            uchar maxMask = MAX(maskB[0], MAX(maskB[1], maskB[2]));
            if(maxMask == 255){
                int a = 0;
            }
            if (maxMask == 0)
            {
                pSrc += 4;
                pGauss += 4;
                pGreen += 4;
                maskB +=4;
                continue;
            }
            uchar r = pSrc[2];
            uchar g = pSrc[1];
            uchar b = pSrc[0];
            int mask = (pGauss[0] + pGauss[1] + pGauss[2]) / 3;
            int tb = CLIP3((pSrc[0] * (255 - mask) + pGreen[0] * mask) / 255, 0, 255);
            int tg = CLIP3((pSrc[1] * (255 - mask) + pGreen[1] * mask) / 255, 0, 255);
            int tr = CLIP3((pSrc[2] * (255 - mask) + pGreen[2] * mask) / 255, 0, 255);
            pSrc[0] = b * (1 - maxMask / 255.0) + CLIP3((pSrc[0] * (128 - k) + tb * k) >> 7, 0, 255) * (maxMask / 255.0);
            pSrc[1] = g * (1 - maxMask / 255.0) + CLIP3((pSrc[1] * (128 - k) + tg * k) >> 7, 0, 255) * (maxMask / 255.0);
            pSrc[2] = r * (1 - maxMask / 255.0) + CLIP3((pSrc[2] * (128 - k) + tr * k) >> 7, 0, 255) * (maxMask / 255.0);
            pSrc += 4;
            pGauss += 4;
            pGreen += 4;
            maskB +=4;
        }
    }
    tmpSkinGauss.release();
    free(greenData);
    free(curveData);
//    free(maskB);
    return ret;
}
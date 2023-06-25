//
// Created by libowu on 2021/11/28.
//
#include <jni.h>
#include <string>
#include "include/opencv2/utils/Commen.h"
#include <iostream>
#include "opencv2/opencv.hpp"
#include "net.h"
#include "opencv2/utils/utils.h"
#include <android/asset_manager_jni.h>
#include "opencv2/beauty/BeautyPeople.h"
#include "opencv2/beauty/FaceLandmark.h"
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

//Reference:Image Warping with Scattered Data Interpolation

int f_IDW(unsigned char* srcData, int width, int height, int stride, int inputPoints[], int outputPoints[], int pointNum)
{
    int ret = 0;
    unsigned char* pSrc = srcData;
    int aa, bb, cc, dd, pos, pos1, xx, yy;
    double r1, r2, r3;
    unsigned char *pSrcL1;
    unsigned char *pSrcL2;
    unsigned char* tempData = (unsigned char*)malloc(sizeof(unsigned char)* height * stride);
    memcpy(tempData, srcData, sizeof(unsigned char) * height * stride);
    unsigned char* p = tempData;
    double w = 0, x_in, y_in, sumw, v;
    double u = 1;
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            x_in = 0, y_in = 0;
            //F function compute
            sumw = 0;
            for(int k = 0; k < pointNum; k++)
            {
                int t = (k << 1);
                v = 1.0 / (pow((double)((i - inputPoints[t]) * (i - inputPoints[t]) + (j - inputPoints[t + 1]) * (j - inputPoints[t + 1])),u));
                sumw += v;
                if(i == inputPoints[t] && j == inputPoints[t + 1])
                    w = 1.0;
                else
                    w = v;
                x_in += w * (outputPoints[t] + i - inputPoints[t]);
                y_in += w * (outputPoints[t + 1] + j - inputPoints[t + 1]);
            }
            w = 1.0 / sumw;
            x_in = x_in * w;
            y_in = y_in * w;
            //interpolation
            x_in = CLIP3(x_in, 0, width - 1);
            y_in = CLIP3(y_in, 0, height - 1);
            xx = (int)x_in;
            yy = (int)y_in;
            if (x_in ==0 && y_in == 0){
                pSrc += 4;
                continue;
            }
            pSrcL1 = p + yy * stride;
            pSrcL2 = p + CLIP3((yy + 1), 0, height - 1) * stride;
            pos = (xx << 2);
            aa = pSrcL1[pos];
            bb = pSrcL1[pos + 4];
            cc = pSrcL2[pos];
            dd = pSrcL2[pos + 4];
            r1 = aa + (bb - aa) * (x_in - xx);
            r2 = cc + (dd - cc) * (x_in - xx);
            r3 = r1 + (r2 - r1) * (y_in - yy);
            pSrc[0]=(unsigned char)(CLIP3(r3, 0, 255));//B
            aa = pSrcL1[pos + 1];
            bb = pSrcL1[pos + 4 +1];
            cc = pSrcL2[pos + 1];
            dd = pSrcL2[pos + 4 + 1];
            r1 = aa + (bb - aa) * (x_in - xx);
            r2 = cc + (dd - cc) * (x_in - xx);
            r3 = r1 + (r2 - r1) * (y_in - yy);
            pSrc[1]=(unsigned char)(CLIP3(r3, 0, 255));//G
            aa = pSrcL1[pos + 2];
            bb = pSrcL1[pos + 4 + 2];
            cc = pSrcL2[pos + 2];
            dd = pSrcL2[pos + 4 + 2];
            r1 = aa + (bb - aa) * (x_in - xx);
            r2 = cc + (dd - cc) * (x_in - xx);
            r3 = r1 + (r2 - r1) * (y_in - yy);
            pSrc[2]=(unsigned char)(CLIP3(r3, 0, 255));//R
            aa = pSrcL1[pos + 3];
            bb = pSrcL1[pos + 4 + 3];
            cc = pSrcL2[pos + 3];
            dd = pSrcL2[pos + 4 + 3];
            r1=aa + (bb - aa) * (x_in - xx);
            r2=cc + (dd - cc) * (x_in - xx);
            r3=r1 + (r2 - r1) * (y_in - yy);
            pSrc[3]=(unsigned char)(CLIP3(r3, 0, 255));//A
            pSrc += 4;
        }
    }
    free(tempData);
    return ret;
};



/*************************************************************************
*Function: AutoFaceLift
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*facePoints: 101 face points of image.
*intensity: intensity of facelift, [0, 100]
*Return: 0-OK,other failed
**************************************************************************/

int autoThineFace(unsigned char* srcData, int width, int height, int stride, int facePoints[], int intensity)
{
    int ret = 0;
    int K1, K2;
    int P5x = facePoints[2 * 272];
    int P5y = facePoints[2 * 272 + 1];
    int P6x = facePoints[2 * 246];
    int P6y = facePoints[2 * 246 + 1];
    int P7x = facePoints[2 * 209];
    int P7y = facePoints[2 * 209 + 1];
    int P8x = facePoints[2 * 182];
    int P8y = facePoints[2 * 182 + 1];
    int P9x = facePoints[2 * 207];
    int P9y = facePoints[2 * 207 + 1];
    int P0x = facePoints[2 * 820];
    int P0y = facePoints[2 * 820 + 1];
    //compute P' after facelift
    float K = 0.1f * intensity / 100;
    int P6X, P6Y, P8X, P8Y;
    P6X = P6x + (P0x - P6x) * K;
    P6Y = P6y + (P0y - P6y) * K;
    P8X = P8x + (P0x - P8x) * K;
    P8Y = P8y + (P0y - P8y) * K;
    //compute face rectangle
    float dis = sqrt(((float)facePoints[2 * 820] - facePoints[2 * 209])*((float)facePoints[2 * 820] - facePoints[2 * 209]) + ((float)facePoints[2 * 820 + 1] - facePoints[2 * 209 + 1])*((float)facePoints[2 * 820 + 1] - facePoints[2 * 209 + 1])) * 1.1;
    int minx = CLIP3(P0x - dis, 0, width - 1);
    int miny = CLIP3(P0y - dis, 0, height - 1);
    int maxx = CLIP3(P0x + dis, 0, width - 1);
    int maxy = CLIP3(P0y + dis, 0, height - 1);
    int aPoints[2 * 9] = {minx, miny, minx, maxy, maxx, maxy, maxx, miny, P5x, P5y, P6x, P6y, P7x, P7y, P8x, P8y, P9x, P9y};
    int bPoints[2 * 9] = {minx, miny, minx, maxy, maxx, maxy, maxx, miny, P5x, P5y, P6X, P6Y, P7x, P7y, P8X, P8Y, P9x, P9y};
    ret = f_IDW(srcData, width, height, stride, bPoints, aPoints, 9);
    return ret;
};

jobject thinFace(JNIEnv *env, jclass clazz, jobject bitmap,jint strength){
    std::vector<FaceKeyPoint>  result = FaceLandmark::detector->detect(env, bitmap);
    if (result.size() == 0){
        return bitmap;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img_src(info.height, info.width, CV_8UC4, pixels);
    cv::Mat bgr = img_src.clone();
    int point[973 * 2] = {0};
    //有可能识别到多张脸，只选取第一张脸
    for (int i = 0; i < 973; i++) {
        point[i * 2] = result[i].p.x;
        point[i * 2 + 1] = result[i].p.y;
    }
    cv::cvtColor(img_src,img_src, cv::COLOR_RGB2BGRA);
    autoThineFace(bgr.ptr<uchar>(), bgr.cols, bgr.rows, bgr.step, point, strength);
    jobject resultBitmap = matToBitmap(env, bgr, true, bitmap);
    img_src.release();
    bgr.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

